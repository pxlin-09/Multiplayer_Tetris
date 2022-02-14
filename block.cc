#include "block.h"

Block::Block(Brick *brick) : brick(brick) {}

Block::Block(int r, int c, char type, Brick *brick) 
	: brick(brick), r(r), c(c), type(type) 
{}

void Block::set(int r, int c, char type) {
	this->r = r;
	this->c = c;
	this->type = type;
}

int Block::getRow() {
    return r;
}

int Block::getCol() {
    return c;
}

char Block::getType() {
    return type;
}

void Block::setBrick(Brick *brick) {
    this->brick = brick;
}

void Block::setRC(int r, int c) {
	this->r = r;
	this->c = c;
}

void Block::removeFromBrick() {
    brick->removeBlock(r,c);
}
