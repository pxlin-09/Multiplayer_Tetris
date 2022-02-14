#include "board.h"
#include <cmath>

using std::vector;
using std::make_unique;


Board::Board(int level, int height, int width, unsigned int seed, int player)
	: level(level), height(height), width(width), score(0), noClear(0), special(0), 
	   player(player), seed(seed), isRandom(true){
	lost = false;
	curBrick = nullptr;
	isRandom = false;
	noClear = 0;
	for (int i = 0; i < height; i++) {
		grid.emplace_back();
		for (int ii = 0; ii < width; ii++) {
			grid[i].emplace_back(nullptr);
		}
	}
}

Board::Board(const Board &other) {
	width = other.width;
	height = other.height;
	player = other.player;
	level = other.level;
	seed = other.seed;
	score = other.score;
	special = other.special;
	size_t len = other.bricks.size();
	noClear = other.noClear;
	lost = other.lost;
	isRandom = other.isRandom;
	for (int i = 0; i < height; i++) {
		grid.emplace_back();
		for (int ii = 0; ii < width; ii++) {
			grid[i].emplace_back(nullptr);
		}
	}
	for (size_t i = 0; i < len; i++) {
		Brick* src = other.bricks[i].get();
		auto dest = make_unique<Brick>(src->getLevel(), src->getType(), seed);
		dest->copyComponent(*src);
		dest->setBoard(this);
		if (dest->getType() == '*') dest->setStar();
		bricks.push_back(std::move(dest));
	}
	if(bricks.size() > 0) curBrick = bricks.back().get();
}

int Board::getPlayer() {
	return player;
}

bool Board::getLost() {
	return lost;
}

void Board::lostGame() {
	lost = true;
}

Block *Board::getAt(int r, int c) {
	return grid[r][c];
}

void Board::setAt(int r, int c, Block *b) {
	grid[r][c] = b;
}

void Board::setRandom(bool isRandom) {
    this->isRandom = isRandom;
}

bool Board::getRandom() {
	return isRandom;
}

int Board::getLevel() {
    return level;
}

int Board::getScore() {
    if (score >= pow (10, width)) {
        return pow (10, width) - 1;
    }
    return score;
}

unsigned int Board::getSeed() {
    return seed;
}

Brick* Board::getNextBrick() {
    return nextBrick.get();
}

vector<vector<Block*>> Board::getGrid() {
	return grid;
}

int Board::getSpecial() {
	return special;
}

int Board::update() {
	int cleared = 0;
	drop();
	while (true) {
		int clearRow = canClearRow();
		if (clearRow == -1) {
			break;
		} else {
			while (true) {
				if (clearRow == -1) break;
				cleared++;
				for (int i = 0; i < width; i++) {
					grid[clearRow][i]->removeFromBrick();
				}
				grid[clearRow].clear();
				for (int i = 0; i < width; i++) {
					grid[clearRow].emplace_back(nullptr);
				}
				clearRow = canClearRow();
			}
		}
		drop();
	}
	drop(); // makes sure everything is dropped, this should make no difference
	if (cleared) {
        score += pow(cleared + level, 2);
	} else {
		if(level >= 4) noClear += 1;
	}
	if (level == 4 && noClear >= 5) {
		createBrick('*');
		noClear = 0;
	}
	special = 0;
	return cleared;
}

void Board::moveLeft() {
    curBrick->moveLeft();
}

void Board::moveRight() {
    curBrick->moveRight();
}

void Board::moveDown() {
    curBrick->moveDown();
}

void Board::clockwise() {
    curBrick->clockwise();
}

void Board::counterclockwise() {
    curBrick->counterclockwise();
}

void Board::levelup() {
    if (level != 4) level += 1;
}

void Board::leveldown() {
    if (level != 0) level -= 1;
    if (level < 4) noClear = 0;
}

void Board::drop() {
	int len = bricks.size();
	for (int i = 0; i < len; i++) {
		dropBrick(bricks[i].get());
	}
}

void Board::dropBrick(Brick *brick) {
	while (true) {
		if (!brick->moveDown()) return;
	}

}

void Board::removeBrick(Brick *brick, bool change_score) {
    if (change_score) score += pow(brick->getLevel() + 1, 2);
	for (vector<unique_ptr<Brick>>::iterator it = bricks.begin(); it != bricks.end(); it++) {
		if ((*it).get() == brick) {
			bricks.erase(it);
			break;
		}
	}
}

int Board::canClearRow() {
	bool found = true;
	for (int r = height - 1; r >= 0; r--) {
		for (int c = 0; c < width; c++) {
			if (grid[r][c] == nullptr) {
				found = false;
				break;
			}
		}
		if (found) return r;
		found = true;
	}
	return -1;
}

bool Board::canDrop(int r, int c) {
	for (int i = r + 1; i != height; i++) {
		if (grid[i][c] == nullptr) return true;
	}
	return false;
}

int Board::getHeight() {
	return height;
}

int Board::getWidth() {
	return width;
}

void Board::dropBlock(int r, int c) {
	int new_r;
	for (int i = r+1; i != height; i++) {
		if (grid[i][c] != nullptr) {
			new_r = i-1;
			break;
		}
	}
	grid[new_r][c] = std::move(grid[r][c]);
	grid[r][c] = nullptr;
}

char Board::displayAt(int r, int c) {
	Block *b = grid[r][c];
	if (special == 1 && r >= 2+3 && r <= 11+3 && c >= 2 && c <= 8) {
		return '?';
	}
	if (b == nullptr) return ' ';
	return b->getType();
}

void Board::createBrick(char type) {
	if (type == '*') {
		auto b1 = std::make_unique<Brick>(level, type, seed, this);
		bricks.push_back(std::move(b1));
	}else if (curBrick == nullptr) {
		auto b1 = std::make_unique<Brick>(level, type, seed, this);
		bricks.push_back(std::move(b1));
		curBrick = bricks.back().get();
	} else {
		auto b1 = std::move(nextBrick);
		b1->setBoard(this);
		bricks.push_back(std::move(b1));
		curBrick = bricks.back().get();
	}
}

void Board::changeCurrent(char type) {
	curBrick->clear();
	removeBrick(curBrick, false);
	auto b1 = std::make_unique<Brick>(level, type, seed, this);
	bricks.push_back(std::move(b1));
	curBrick = bricks.back().get();
}

void Board::changeSpecial(int special) {
	this->special = special;
}

void Board::createNext(char type) {
	auto b1 = std::make_unique<Brick>(level, type, seed);
	nextBrick = std::move(b1);
}

bool Board::atBottom() {
	return curBrick->atBottom();
}

Brick *Board::getCurBrick() {
	return curBrick;
}

Board &Board::operator=(const Board &other) {
	width = other.width;
	height = other.height;
	player = other.player;
	level = other.level;
	seed = other.seed;
	special = other.special;
	score = other.score;
	size_t len = other.bricks.size();
	noClear = other.noClear;
	lost = other.lost;
	isRandom = other.isRandom;
	for (int i = 0; i < height; i++) {
		grid.emplace_back();
		for (int ii = 0; ii < width; ii++) {
			grid[i].emplace_back(nullptr);
		}
	}
	for (size_t i = 0; i < len; i++) {
		Brick* src = other.bricks[i].get();
		auto dest = make_unique<Brick>(src->getLevel(), src->getType(), seed);
		dest->copyComponent(*src);
		dest->setBoard(this);
		if (dest->getType() == '*') dest->setStar();
		bricks.push_back(std::move(dest));
	}
	if(bricks.size() > 0) curBrick = bricks.back().get();
	return *this;
}
