#ifndef BRICK_H
#define BRICK_H
#include <vector>
#include <memory>
#include "board.h"
#include "block.h"

using std::unique_ptr;
using std::vector;

class Board;

struct point {
    int r, c;
};

class Brick {
    int level;
    Board *board;
    char type;
    bool at_bottom;
    vector<unique_ptr<Block>> component; // all block contained in the brick
    public:

        Brick(int level, char givenType, unsigned int seed, Board* board = nullptr); // constructor
        char getType(); // return type of brick
        int getLevel(); // return level of brick
        void setBoard(Board *board); // set field board to board
        // no effect if it is not possible to apply the effect (at edge of the board or have block in the way)
        // when calling moveLeft, moveRight, moveDown, component can have any number of Block
        void moveLeft(); // move left one column
        void moveRight(); // move right one column
        bool moveDown(); // move down one row
        // when calling clockwise and counterclockwise, brick should be a complete brick (i.e. 4 blocks w/ correct positions to form type)
        void clockwise(); // rotate clockwise 90 degree
        void counterclockwise(); // rotate counterclockwise 90 degree
        void removeBlock(int r, int c); // remove the block with r and c from component
        vector<Block*> getBlocks();
        bool atBottom(); // determine if it is at bottom of grid
        char createType(unsigned int seed);
        void clear(); // clear components
        void copyComponent(const Brick &other);
        point getBotLeft(); // return bottom left point of the brick, no matter if there exist block or not
		point getTopLeft(); // return top left point of the brick, no matter if there exist block or not
		int bottomCount(); // count number of lines in the bottom line
		void setStar(); // draw stars for level 4
		
    private:
        bool checkOccupied(int r, int c); // true if can move to (r c), false otherwise 
        void changeS(); // rotate S clockwise/counterclockwise by 90 degree
        void changeZ(); // rotate Z clockwise/counterclockwise by 90 degree
        void changeI(); // rotate I clockwise/counterclockwise by 90 degree
        bool changePoint(vector<point> oldPt, vector<point> newPt); // change component to blocks with newPt coords from blocks with oldPt coords
		bool contains(int r, int c);
		void adjustBoard(); // show components on board	
};

#endif


