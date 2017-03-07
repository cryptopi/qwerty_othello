#include "board.hpp"

/*
 * Make a standard 8x8 othello board and initialize it to the standard setup.
 */
Board::Board() {
    taken.set(3 + 8 * 3);
    taken.set(3 + 8 * 4);
    taken.set(4 + 8 * 3);
    taken.set(4 + 8 * 4);
    black.set(4 + 8 * 3);
    black.set(3 + 8 * 4);
}

/*
 * Destructor for the board.
 */
Board::~Board() {
}

/*
 * Returns a copy of this board.
 */
Board *Board::copy() {
    Board *newBoard = new Board();
    newBoard->black = black;
    newBoard->taken = taken;
    return newBoard;
}

bool Board::occupied(int x, int y) {
    return taken[x + 8*y];
}

bool Board::get(Side side, int x, int y) {
    return occupied(x, y) && (black[x + 8*y] == (side == BLACK));
}

void Board::set(Side side, int x, int y) {
    taken.set(x + 8*y);
    black.set(x + 8*y, side == BLACK);
}

bool Board::onBoard(int x, int y) {
    return(0 <= x && x < 8 && 0 <= y && y < 8);
}


/*
 * Returns true if the game is finished; false otherwise. The game is finished
 * if neither side has a legal move.
 */
bool Board::isDone() {
    return !(hasMoves(BLACK) || hasMoves(WHITE));
}

/*
 * Returns true if there are legal moves for the given side.
 */
bool Board::hasMoves(Side side) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Move move(i, j);
            if (checkMove(&move, side)) return true;
        }
    }
    return false;
}

/*
 * Returns true if a move is legal for the given side; false otherwise.
 */
bool Board::checkMove(Move *m, Side side) {
    // Passing is only legal if you have no moves.
    if (m == nullptr) return !hasMoves(side);

    int X = m->getX();
    int Y = m->getY();

    // Make sure the square hasn't already been taken.
    if (occupied(X, Y)) return false;

    Side other = (side == BLACK) ? WHITE : BLACK;
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if (dy == 0 && dx == 0) continue;

            // Is there a capture in that direction?
            int x = X + dx;
            int y = Y + dy;
            if (onBoard(x, y) && get(other, x, y)) {
                do {
                    x += dx;
                    y += dy;
                } while (onBoard(x, y) && get(other, x, y));

                if (onBoard(x, y) && get(side, x, y)) return true;
            }
        }
    }
    return false;
}

/*
 * Modifies the board to reflect the specified move.
 */
void Board::doMove(Move *m, Side side) {
    // A nullptr move means pass.
    if (m == nullptr) return;

    // Ignore if move is invalid.
    if (!checkMove(m, side)) return;

    int X = m->getX();
    int Y = m->getY();
    Side other = (side == BLACK) ? WHITE : BLACK;
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if (dy == 0 && dx == 0) continue;

            int x = X;
            int y = Y;
            do {
                x += dx;
                y += dy;
            } while (onBoard(x, y) && get(other, x, y));

            if (onBoard(x, y) && get(side, x, y)) {
                x = X;
                y = Y;
                x += dx;
                y += dy;
                while (onBoard(x, y) && get(other, x, y)) {
                    set(side, x, y);
                    x += dx;
                    y += dy;
                }
            }
        }
    }
    set(side, X, Y);
}

/*
 * Current count of given side's stones.
 */
int Board::count(Side side) {
    return (side == BLACK) ? countBlack() : countWhite();
}

/*
 * Current count of black stones.
 */
int Board::countBlack() {
    return black.count();
}

/*
 * Current count of white stones.
 */
int Board::countWhite() {
    return taken.count() - black.count();
}

/**
 * @brief gets the heuristic score of this board
 *
 * @param side the side to compute the score for
 * @param testingMinimax true if we are testing minimax, false otherwise
 *
 * @return the score of this board for the provided side
 */
int Board::getScore(Side side, bool testingMinimax) {
    int totalScore = 0;
    //simple scoring function for testing
    if (testingMinimax) {
        if (side == BLACK) {
            totalScore = this->countBlack() - this->countWhite();
        } else {
            totalScore = this->countWhite() - this->countBlack();
        }
    } else {
        //weights for pieces in corresponding squares
        const int CORNER_WEIGHT = 3;
        const int EDGE_WEIGHT = 2;
        const int NEXT_TO_CORNER_WEIGHT = -2;
        const int DIAGONAL_TO_CORNER_WEIGHT = -3;
        const int OTHER_WEIGHT = 1;
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                //score occupied squares only
                if (this->occupied(i, j)) {
                    //different positions are weighted differently
                    Position pos = Board::getSquarePosition(i, j);
                    int additionalScore = 0;
                    if (pos == CORNER) {
                        additionalScore = CORNER_WEIGHT;
                    } else if (pos == DIAGONAL_TO_CORNER) {
                        additionalScore = DIAGONAL_TO_CORNER_WEIGHT;
                    } else if (pos == NEXT_TO_CORNER) {
                        additionalScore = NEXT_TO_CORNER_WEIGHT;
                    } else if (pos == EDGE) {
                        additionalScore = EDGE_WEIGHT;
                    } else if (pos == OTHER) {
                        additionalScore = OTHER_WEIGHT;
                    }
                    //if piece is of opposite color, has opposite score
                    if (!this->get(side, i, j)) {
                        additionalScore = -additionalScore;
                    }
                    totalScore += additionalScore;
                }
            }
        }
    }
    
    return totalScore;
}

/**
 * @brief determines the position of the square at the provided coordinates
 *
 * @param x the x coordinate of the square to test
 * @param y the y coordinate of the square to test
 *
 * @return the position of the square at the provided coordinates
 */
Position Board::getSquarePosition(int x, int y) {
    Position pos;
    bool isEdge = (x == 0) || (x == BOARD_SIZE - 1) || (y == 0) || (y == BOARD_SIZE - 1);
    bool isCorner = 
            (x == 0 && y == 0) || 
            (x == 0 && y == BOARD_SIZE - 1) || 
            (x == BOARD_SIZE - 1 && y == 0) || 
            (x == BOARD_SIZE - 1 && y == BOARD_SIZE - 1);
    bool isDiagonalToCorner = 
            (x == 1 && y == 1) || 
            (x == 1 && y == BOARD_SIZE - 2) || 
            (x == BOARD_SIZE - 2 && y == 1) || 
            (x == BOARD_SIZE - 2 && y == BOARD_SIZE - 2);
    bool isNextToCorner = 
            (x == 1 && y == 0) ||
            (x == 0 && y == 1) ||
            (x == 0 && y == BOARD_SIZE - 2) ||
            (x == 1 && y == BOARD_SIZE - 1) ||
            (x == BOARD_SIZE - 2 && y == BOARD_SIZE - 1) ||
            (x == BOARD_SIZE - 1 && y == BOARD_SIZE - 2) ||
            (x == BOARD_SIZE - 2 && y == 0) ||
            (x == BOARD_SIZE - 1 && y == 1);
    if (isCorner) {
        pos = CORNER;
    } else if (isDiagonalToCorner) {
        pos = DIAGONAL_TO_CORNER;
    } else if (isNextToCorner) {
        pos = NEXT_TO_CORNER;
    } else if (isEdge) {
        pos = EDGE;
    } else {
        pos = OTHER;
    }
    
    return pos;
}


/*
 * Sets the board state given an 8x8 char array where 'w' indicates a white
 * piece and 'b' indicates a black piece. Mainly for testing purposes.
 */
void Board::setBoard(char data[]) {
    taken.reset();
    black.reset();
    for (int i = 0; i < 64; i++) {
        if (data[i] == 'b') {
            taken.set(i);
            black.set(i);
        } if (data[i] == 'w') {
            taken.set(i);
        }
    }
}
