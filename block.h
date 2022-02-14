#ifndef BLOCK_H
#define BLOCK_H
#include "brick.h"

class Brick;

class Block {
    Brick *brick; // the brick containing this block
    int r, c;
    char type;
    public:
		Block(Brick *brick); // constructor
		Block(int r, int c, char type, Brick *brick); // constructor
        void setBrick(Brick *brick); // set the brick containing this block to be brick
        void set(int r, int c, char type);
        int getRow(); // return row of block
        int getCol(); // return column of block
        char getType(); // return type of block
        void setRC(int r, int c); // set r and c to be r and c
        void removeFromBrick(); // remove this block from brick;
};

#endif
