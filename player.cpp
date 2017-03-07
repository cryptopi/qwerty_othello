#include "player.hpp"

/*
 * Constructor for the player; initialize everything here. The side your AI is
 * on (BLACK or WHITE) is passed in as "side". The constructor must finish
 * within 30 seconds.
 */
Player::Player(Side side) {
    // Will be set to true in test_minimax.cpp.
    testingMinimax = false;

    this->board = new Board();
    this->side = side;
}

/*
 * Destructor for the player.
 * Frees up any resources allocated to it during use
 */
Player::~Player() {
    delete this->board;
}

/*
 * Compute the next move given the opponent's last move. Your AI is
 * expected to keep track of the board on its own. If this is the first move,
 * or if the opponent passed on the last move, then opponentsMove will be
 * nullptr.
 *
 * msLeft represents the time your AI has left for the total game, in
 * milliseconds. doMove() must take no longer than msLeft, or your AI will
 * be disqualified! An msLeft value of -1 indicates no time limit.
 *
 * The move returned must be legal; if there are no valid moves for your side,
 * return nullptr.
 */
Move *Player::doMove(Move *opponentsMove, int msLeft) {
    if (opponentsMove != nullptr) {
        this->board->doMove(opponentsMove, this->side == BLACK ? WHITE : BLACK);
    }
    
    std::pair<int, Move*> results = this->minimax(this->board, 2, true);
    
    Move *nextMove = results.second;
    
    if (nextMove != nullptr) {
        this->board->doMove(nextMove, this->side);
    }
    
    return nextMove;
}

std::pair<int, Move*> Player::minimax(Board *board, int depth, bool maximizingPlayer) {
    //determine the color of the person playing in the tree
    Side playingSide;
    if (maximizingPlayer) {
        playingSide = this->side;
    } else {
        playingSide = this->side == BLACK ? WHITE : BLACK;
    }
    
    //recursive base case
    if (depth == 0 || !board->hasMoves(playingSide)) {
        return std::pair<int, Move*>(board->getScore(this->side, this->testingMinimax), nullptr);
    }

    
    if (maximizingPlayer) {
        //find move that results in highest score
        Move *moveMade = nullptr;
        int bestValue = 0;
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                Move *testMove = new Move(i, j);
                //only check valid moves
                if (this->board->checkMove(testMove, playingSide)) {
                    Board *childBoard = this->board->copy();
                    childBoard->doMove(testMove, playingSide);
                    std::pair<int, Move*> childResults = this->minimax(childBoard, depth - 1, false);
                    int boardScore = childResults.first;
                    //first move being checked
                    if (moveMade == nullptr) {
                        moveMade = new Move(i, j);
                        bestValue = boardScore;
                    }
                    else {
                        if (boardScore > bestValue) {
                            moveMade = new Move(i, j);
                            bestValue = boardScore;
                        }
                    }
                    delete childBoard;
                }
                delete testMove;
            }
        }
        return std::pair<int, Move*>(bestValue, moveMade);
    } else {
        //find move that results in lowest score because enemy is playing
        Move *moveMade = nullptr;
        int lowestValue = 0;
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                Move *testMove = new Move(i, j);
                //only check valid moves
                if (this->board->checkMove(testMove, playingSide)) {
                    Board *childBoard = this->board->copy();
                    childBoard->doMove(testMove, playingSide);
                    std::pair<int, Move*> childResults = this->minimax(childBoard, depth - 1, true);
                    int boardScore = childResults.first;
                    //first move being checked
                    if (moveMade == nullptr) {
                        moveMade = new Move(i, j);
                        lowestValue = boardScore;
                    }
                    else {
                        if (boardScore < lowestValue) {
                            moveMade = new Move(i, j);
                            lowestValue = boardScore;
                        }
                    }
                    delete childBoard;
                }
                delete testMove;
            }
        }
        return std::pair<int, Move*>(lowestValue, moveMade);
    }
}

//random method
// Move *Player::doMove(Move *opponentsMove, int msLeft) {
//     if (opponentsMove != nullptr) {
//         std::cerr << "Updating board with move " << opponentsMove->toString() << " for " << (this->side == BLACK ? WHITE : BLACK) << std::endl;
//         this->board->doMove(opponentsMove, this->side == BLACK ? WHITE : BLACK);
//         std::cerr << "Step 0" << std::endl;
//     }
//     
//     //get random move
//     Move *nextMove = nullptr;
//     for (int i = 0; i < BOARD_SIZE; i++) {
//         for (int j = 0; j < BOARD_SIZE; j++) {
//             Move *testMove = new Move(i, j);
//             if (this->board->checkMove(testMove, this->side)) {
//                 nextMove = new Move(i, j);
//                 delete testMove;
//                 break;
//             } else {
//                 delete testMove;
//             }
//         }
//     }
//     
//     std::cerr << "Computed move " << (nextMove == nullptr ? "PASS" : nextMove->toString()) << " for " << this->side << std::endl;
//     if (nextMove != nullptr) {
//         this->board->doMove(nextMove, this->side);
//     }
//     
//     return nextMove;
// }



//heuristc method
// Move *Player::doMove(Move *opponentsMove, int msLeft) {
//     if (opponentsMove != nullptr) {
//         // std::cerr << "Updating board with move " << opponentsMove->toString() << " for " << (this->side == BLACK ? WHITE : BLACK) << std::endl;
//         this->board->doMove(opponentsMove, this->side == BLACK ? WHITE : BLACK);
//     }
//     
//     //get next best move
//     Move *nextMove = nullptr;
//     int highestScore = 0;
//     for (int i = 0; i < BOARD_SIZE; i++) {
//         for (int j = 0; j < BOARD_SIZE; j++) {
//             Move *testMove = new Move(i, j);
//             if (this->board->checkMove(testMove, this->side)) {
//                 Board *boardClone = this->board->copy();
//                 boardClone->doMove(testMove, this->side);
//                 int moveScore = boardClone->getScore(this->side, this->testingMinimax);
//                 if (nextMove == nullptr) {
//                     nextMove = new Move(i, j);
//                     highestScore = moveScore;
//                 }
//                 else {
//                     if (moveScore > highestScore) {
//                         nextMove = new Move(i, j);
//                         highestScore = moveScore;
//                     }
//                 }
//                 delete boardClone;
//             }
//             delete testMove;
//         }
//     }
//     
//     std::cerr << "Computed move " << (nextMove == nullptr ? "PASS" : nextMove->toString()) << " for " << this->side << std::endl;
//     if (nextMove != nullptr) {
//         this->board->doMove(nextMove, this->side);
//     }
//     
//     return nextMove;
// }
