#ifndef BOARD_H
#define BOARD_H

#include "brick.h"
#include "block.h"
#include <vector>
#include <iostream>
#include <memory>
#include <string>

using std::unique_ptr;
using std::vector;
using std::string;

class Block;
class Brick;

class Board {
	int level, height, width, score, noClear, special, player; // special: 0 -> nothing, 1 -> blind, 2 -> heavy
	vector<vector<Block*>> grid;
	vector<unique_ptr<Brick>> bricks;
	Brick* curBrick = nullptr;
	unique_ptr<Brick> nextBrick;
	unsigned int seed;
	bool isRandom;
	bool lost = false;

    public:
        Board(int level, int height, int width, unsigned int seed, int player); // constructor
        Board(const Board &other); // copy constructor
        Block* getAt(int r, int c); // get pointer of block at position (r,c)
        vector<vector<Block*>> getGrid(); // get grid of board
        void setAt(int r, int c, Block* b); // set b to have "coordinate" (r,c)
        void setRandom(bool isRandom); // set random to isRandom
        bool getRandom(); // return random
        bool getLost(); // return lost
        void lostGame(); // let lost be ture
        int getPlayer(); // return player
        int getHeight(); // return height
        int getWidth(); // return width
        int getLevel(); // return level
        int getScore(); // return score
        unsigned int getSeed(); // return seed
        Brick* getCurBrick(); // return curBrick
        Brick* getNextBrick(); // return nextBrick
        int getSpecial(); // return special
        int update(); // update board so rows are cleared, and return number of rows cleared
        void moveLeft(); // move left one column
        void moveRight(); // move right one column
        void moveDown(); // move down one row
        void clockwise(); // rotate clockwise 90 degree
        void counterclockwise(); // rotate counterclockwise 90 degree
        void levelup(); // increase level by one
        void leveldown(); // decrease level by one
        void drop(); // drop all bricks
        char displayAt(int r, int c); // return the type of block at (r,c)
        void removeBrick(Brick *brick, bool change_score = true); // remove brick and change player score if change_score = true
        void createBrick(char type = 'A'); // create brick with type
        void createNext(char type = 'A'); // create next brick with type
        bool atBottom(); // check if brick cannot be move any further downwards
        void changeCurrent(char type); // change the current brick to type
        void changeSpecial(int special); // change special action to special
        Board &operator=(const Board &other);

    private:
        void dropBlock(int r, int c); // drop the block at r,c
        void dropBrick(Brick *brick);
        bool canDrop(int r, int c);	// determine if block at r,c can drop
        int canClearRow(); // produce the row that can be cleared or -1 if none can be cleared

};

#endif // BOARD_H
