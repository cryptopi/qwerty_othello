#ifndef __COMMON_H__
#define __COMMON_H__

#include <iostream>
#include <string>

#define BOARD_SIZE (8)

enum Side { 
    WHITE, BLACK
};

enum Position {
    CORNER, EDGE, NEXT_TO_CORNER, DIAGONAL_TO_CORNER, OTHER
};

class Move {
   
public:
    int x, y;
    Move(int x, int y) {
        this->x = x;
        this->y = y;        
    }
    ~Move() {}

    int getX() { return x; }
    int getY() { return y; }

    void setX(int x) { this->x = x; }
    void setY(int y) { this->y = y; }
    std::string toString() { return std::string("Move: (" + std::to_string(x) + ", " + std::to_string(y) + ")"); }
};

#endif
