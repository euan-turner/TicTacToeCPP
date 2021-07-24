#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

const int PLAYER = 1;
const int AI = -1;
const int EMPTY = 0;

class Move {
    public:
        int row;
        int col;

        Move(int r=NULL, int c=NULL) {
            row = r;
            col = c;
        }
};

class Board {
    public:
        int state[3][3] = {
            {EMPTY, EMPTY, EMPTY},
            {EMPTY, EMPTY, EMPTY},
            {EMPTY, EMPTY, EMPTY},
        };
        int turns = 0;

        bool checkWin();
        int gameOver();
        void output();
        void reset();
        vector<Move> validMoves();
        bool isValidMove(Move move);
        void playMove(int token, Move move);
        void undoMove(Move move);
};

bool Board :: checkWin() {
    bool gameWon = false;

    //Check rows and columns
    for (int i = 0; i < 3; i++) {
        int rowSum = 0;
        int colSum = 0;
        for (int j = 0; j < 3; j++) {
            rowSum = rowSum + state[i][j];
            colSum = colSum + state[j][i];
        }
        if ((rowSum == 3)||(rowSum == -3)||(colSum == 3)||(colSum == -3)) {
            gameWon = true;
        }
    }
    //Check diagonals
    int forSum = state[2][0] + state[1][1] + state[0][2];
    int backSum = state[0][0] + state[1][1] + state[2][2];
    if ((forSum == 3)||(forSum == -3)||(backSum == 3)||(backSum == -3)) {
        gameWon = true;
    }
    return gameWon;
}

int Board :: gameOver() {
    bool won = checkWin();
    //Count 0s
    int numZeros = 0;
    for (int r = 0; r < 3; r++) {
        for (int c = 0; c < 3; c++) {
            if (state[r][c] == EMPTY) {
                numZeros ++;
            }
        }
    }
    if (won == true) {
        return 1;
    } else if (numZeros == 0) {
        return 0;
    } else {
        return -1;
    }
}

void Board :: output() {
    cout << "-------\n";
    for (int row = 0; row < 3; row++) {
        cout << "|";
        for (int col = 0; col < 3; col++) {
            int token = state[row][col];
            if (token == 1) {
                cout << '@';
            } else if (token == -1) {
                cout << '#';
            } else {
                cout << " ";
            }
            cout << "|";
        }
        cout << "\n";
        cout << "-------\n";
    }
}

void Board :: reset(){
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            state[row][col] = 0;
        }
    }
    turns = 0;

    cout << "Continue? (y/n): ";
    char answer;
    cin >> answer;
    if (answer == 'n') {
        exit(0);
    }
}

vector<Move> Board :: validMoves() {
    vector<Move> valid;
    for (int r = 0; r < 3; r++) {
        for (int c = 0; c < 3; c++) {
            if (state[r][c] == EMPTY){
                valid.push_back(Move(r,c));
            }
        }
    }
    return valid;
}

bool Board :: isValidMove(Move move) {
    if (state[move.row][move.col] == EMPTY) {
        return true;
    } else {
        return false;
    }
}

void Board :: playMove(int token, Move move) {
    state[move.row][move.col] = token;
}

void Board :: undoMove(Move move) {
    state[move.row][move.col] = EMPTY;
}

class Game {
    public:
        Board board = Board();
        int current = AI;

        void turn();
        Move findBestMove();
        int minimax(int depth, int alpha, int beta, bool isMax);
};

void Game :: turn() {
    board.output();
    Move move;

    if (current == AI) {
        move = findBestMove();
    } else {
        bool validMove = false;
        while (validMove == false) {
            cout << "Enter move number (top left to bottom right)\n:";
            int choice;
            cin >> choice;
            if ((choice > 9)||(choice < 1)) {
                continue;
            }
            choice --;
            move.row = choice / 3;
            move.col = choice % 3;
            if (board.isValidMove(move) == true) {
                validMove = true;
            } else {
                cout << "Invalid entry, try again";
            }
        }
    }

    board.playMove(current, move);
    board.turns ++;

    int status = board.gameOver();
    if (status == 1) {
        cout << "Game won\n";
        board.output();
        board.reset();
    } else if (status == 0) {
        cout <<"Game drawn\n";
        board.output();
        board.reset();
    }
    current = - current;
}

Move Game :: findBestMove() {
    Move bestMove;
    int bestEval = -100000;

    vector<Move> valid = board.validMoves();
    for (int m = 0; m < valid.size(); m++) {
        Move move = valid.at(m);

        board.playMove(AI, move);
        int moveEval = minimax(0, -100000, 100000, false);
        cout << "(" << move.row << "," << move.col << ")\t" << moveEval << "\n";
        board.undoMove(move);

        if (moveEval > bestEval) {
            bestEval = moveEval;
            bestMove = move;
        } 
    }
    return bestMove;
}

int Game :: minimax(int depth, int alpha, int beta, bool isMax) {
    int status = board.gameOver();
    if (status == 0) {
        return 0;
    } else if (status == 1) {
        if (isMax == true) {
            return -10 + depth;
        } else {
            return 10 - depth;
        }
    } else {
        vector<Move> valid = board.validMoves();
        int bestEval;

        if (isMax == true) {
            bestEval = -100000;
            for (int m = 0; m < valid.size(); m++) {
                Move move = valid.at(m);
                
                board.playMove(AI, move);
                bestEval = max(bestEval, minimax(depth+1, alpha, beta, false));
                board.undoMove(move);

                if (bestEval >= beta) {
                    break;
                }
                alpha = max(bestEval, alpha);
            }
            return bestEval;
        } else {
            bestEval = 100000;
            for (int m = 0; m < valid.size(); m++) {
                Move move = valid.at(m);

                board.playMove(PLAYER, move);
                bestEval = min(bestEval, minimax(depth+1, alpha, beta, true));
                board.undoMove(move);

                if (bestEval <= alpha) {
                    break;
                }
                beta = min(beta, bestEval);
            }
            return bestEval;
        }
    }
}

int main() {
    Game game;
    while (true) {
        game.turn();
    }
}