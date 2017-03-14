#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <iostream>
#include <utility>
#include "common.hpp"
#include "board.hpp"
using namespace std;

class Player {

public:
    Player(Side side);
    ~Player();

    Move *doMove(Move *opponentsMove, int msLeft);

    // Flag to tell if the player is running within the test_minimax context
    bool testingMinimax;
    void setBoard(Board *aBoard) { this->board = aBoard; }
    std::pair<int, Move*> minimax(Board *board, int depth, bool maximizingPlayer);
    std::pair<int, Move*> negamax(Board *board, Side playingSide, int depth, int alpha, int beta);
private:
    Board *board;
    Side side;
};

#endif
