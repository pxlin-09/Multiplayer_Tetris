#include "brick.h"
#include <algorithm>
#include <cstdlib>

using std::rand;
using std::find;
using std::make_unique;

Brick::Brick(int level, char givenType, unsigned int seed, Board *board)
	: level(level), board(board), at_bottom(false)
{

	auto b1 = make_unique<Block>(this);
	auto b2 = make_unique<Block>(this);
	auto b3 = make_unique<Block>(this);
	auto b4 = make_unique<Block>(this);
	if (givenType == 'A') {
        type = createType(seed);
	} else {
        type = givenType;
	}
	if (type == '*') {
		if (board == nullptr) return;
		int middle = board->getWidth()/2;
		b1->set(0,middle,type);
		component.push_back(std::move(b1));
		adjustBoard();
		while(moveDown()) {} // drop the block to bottom
		return;
	}
	if (type == 'S') {
		b1->set(3,1,type);
		b2->set(3,2,type);
		b3->set(4,0,type);
		b4->set(4,1,type);
	} else if (type == 'Z') {
		b1->set(3,0,type);
		b2->set(3,1,type);
		b3->set(4,1,type);
		b4->set(4,2,type);
	} else if (type == 'J') {
		b1->set(3,0,type);
		b2->set(4,0,type);
		b3->set(4,1,type);
		b4->set(4,2,type);
	} else if (type == 'L') {
		b1->set(3,2,type);
		b2->set(4,0,type);
		b3->set(4,1,type);
		b4->set(4,2,type);
	} else if (type == 'O') {
		b1->set(3,0,type);
		b2->set(3,1,type);
		b3->set(4,0,type);
		b4->set(4,1,type);
	} else if (type == 'T') {
		b1->set(3,0,type);
		b2->set(3,1,type);
		b3->set(3,2,type);
		b4->set(4,1,type);
	} else { // type == I 
		type = 'I';
		b1->set(3,0,type);
		b2->set(3,1,type);
		b3->set(3,2,type);
		b4->set(3,3,type);
	}
	component.push_back(std::move(b1));
	component.push_back(std::move(b2));
	component.push_back(std::move(b3));
	component.push_back(std::move(b4));
	if (board == nullptr) return;
	adjustBoard();
}

void Brick::adjustBoard() {
	size_t len = component.size();
	for (size_t i = 0; i < len; i++) {
		if (board->getAt(component[i]->getRow(), component[i]->getCol()) != nullptr) {
			if(type == '*'){
				return;
			} else {
				board->lostGame();
				return;
			}
		}
	}
	for (size_t i = 0; i < len; i++) {
		board->setAt(component[i]->getRow(), component[i]->getCol(), component[i].get());
	}
}

char Brick::createType(unsigned int seed) {
	vector<char> types = {'J', 'L', 'T', 'I', 'O', 'S', 'Z'};
	char type;
	srand(seed);
	if (level == 0) {
		// nothing
	} else if (level == 1) {
        type = types[rand() % 12 % 7];
	} else if (level == 2) {
        type = types[rand() % 7];
	} else if (level == 3) {
        type = types[6 - ((rand() % 9) % 7)];
	} else if (level == 4) {
		type = types[6 - ((rand() % 9) % 7)];
	}
	return type;
}

char Brick::getType() {
	return type;
}

int Brick::getLevel() {
    return level;
}

void Brick::setBoard(Board *board) {
	this->board = board;
	adjustBoard();
}

bool Brick::changePoint(vector<point> oldPt, vector<point> newPt) {
	size_t len = component.size();
	for (size_t i = 0; i < len; i++) {
        if (!checkOccupied(newPt[i].r, newPt[i].c)) {
        	return false;
        }
    }
    for (size_t i = 0; i < len; i++) {
        component[i]->setRC(newPt[i].r,newPt[i].c);
        board->setAt(newPt[i].r, newPt[i].c, component[i].get());
        component[i]->setBrick(this);
    }
    bool exist = false;
    for (size_t i = 0; i < oldPt.size(); i++) {
    	exist = false;
    	for (size_t j = 0; j < newPt.size(); j++) {
    		if (newPt[j].r == oldPt[i].r && newPt[j].c == oldPt[i].c) {
    			exist = true;
    		}
    	}
    	if (exist == false) {
			board->setAt(oldPt[i].r, oldPt[i].c, nullptr);
    	}
    }
    return true;
}

void Brick::moveLeft() {
    int row, col;
    vector<point> oldPt;
    vector<point> newPt;
    size_t len = component.size();
    for (size_t i = 0; i < len; i++) {
        row = component[i]->getRow();
        col = component[i]->getCol();
        point po{row, col};
        point pn{row, col - 1};
        oldPt.push_back(po);
        newPt.push_back(pn);
    }
    changePoint(oldPt, newPt);
}

void Brick::moveRight() {
    int row, col;
    vector<point> oldPt;
    vector<point> newPt;
    size_t len = component.size();
    for (size_t i = 0; i < len; i++) {
        row = component[i]->getRow();
        col = component[i]->getCol();
        point po{row, col};
        point pn{row, col + 1};
        oldPt.push_back(po);
        newPt.push_back(pn);
    }
    changePoint(oldPt, newPt);
}

bool Brick::moveDown() {
    int row, col;
    vector<point> oldPt;
    vector<point> newPt;
    size_t len = component.size();
    for (size_t i = 0; i < len; i++) {
        row = component[i]->getRow();
        col = component[i]->getCol();
        if (!checkOccupied(row+2, col)) {
        	at_bottom = true;
		}
        point po{row, col};
        point pn{row + 1, col};
        oldPt.push_back(po);
        newPt.push_back(pn);
    }
    point bot = getBotLeft();
    int botrow = bot.r;
    int botcol = bot.c;
    int toprow = getTopLeft().r;
    int size = component.size();
    int szcol = 0;
    bool rolremoved = false;
    if (type == 'S' && size == 2 && contains(botrow,botcol+1) && contains(botrow-2,botcol)) {
    	rolremoved = true;
    	szcol = botcol;
	}
	if (type == 'Z' && size == 2 && contains(botrow,botcol) && contains(botrow-2,botcol+1)) {
		rolremoved = true;
		szcol = botcol+1;
	}
	if (rolremoved) {
		for (size_t j = 0; j < len; ++j) {
			if (newPt[j].r == botrow - 1 && newPt[j].c == szcol) {
				newPt[j].r += 2;
			}
		}
	}
    bool exist = false;
    for (int i = botrow; i >= toprow; --i) {
    	exist = false;
    	for (size_t j = 0; j < len; ++j) {
    		if (oldPt[j].r == i) {
    			exist = true;
    			break;
			}
		}
		if (!exist) {
			rolremoved = true;
			for (size_t t = 0; t < len; ++t) {
				if (newPt[t].r <= i) {
					newPt[t].r += 1;
				}
			}
			toprow += 1;
		}
	}
	if (rolremoved) {
		for (size_t t = 0; t < len; ++t) {
			newPt[t].r -= 1;
		}
	}
    return changePoint(oldPt, newPt);
}

bool Brick::contains(int r, int c) {
	size_t len = component.size();
	for (size_t i = 0; i < len; i++) {
		if (component[i]->getRow() == r && component[i]->getCol() == c) {
            return true;
        }
	}
	return false;
}

void Brick::changeS() {
    point botleft = getBotLeft();
    int r = botleft.r;
    int c = botleft.c;
    vector<point> oldPt;
    vector<point> newPt;
    // if botleft has element
    if (contains(r,c)) {
        oldPt = {{r, c}, {r, c+1}, {r-1, c+1}, {r-1, c+2}};
        newPt = {{r, c+1}, {r-1, c}, {r-1, c+1}, {r-2, c}};
    } else {
        newPt = {{r, c}, {r, c+1}, {r-1, c+1}, {r-1, c+2}};
        oldPt = {{r, c+1}, {r-1, c}, {r-1, c+1}, {r-2, c}};
    }
    changePoint(oldPt, newPt);
}

void Brick::changeZ() {
    point botleft = getBotLeft();
    int r = botleft.r;
    int c = botleft.c;
    vector<point> oldPt;
    vector<point> newPt;
    // if botleft has element
    if (contains(r,c+1)) {
        oldPt = {{r, c+1}, {r, c+2}, {r-1, c}, {r-1, c+1}};
        newPt = {{r, c}, {r-1, c}, {r-1, c+1}, {r-2, c+1}};
    } else {
        newPt = {{r, c+1}, {r, c+2}, {r-1, c}, {r-1, c+1}};
        oldPt = {{r, c}, {r-1, c}, {r-1, c+1}, {r-2, c+1}};
    }
    changePoint(oldPt, newPt);
}

void Brick::changeI() {
    point botleft = getBotLeft();
    int r = botleft.r;
    int c = botleft.c;
    vector<point> oldPt;
    vector<point> newPt;
    // if horizontal
    if (contains(botleft.r,botleft.c+1)) {
        oldPt = {{r, c}, {r, c+1}, {r, c+2}, {r, c+3}};
        newPt = {{r, c}, {r-1, c}, {r-2, c}, {r-3, c}};
    } else {
        newPt = {{r, c}, {r, c+1}, {r, c+2}, {r, c+3}};
        oldPt = {{r, c}, {r-1, c}, {r-2, c}, {r-3, c}};
    }
    changePoint(oldPt, newPt);
}

void Brick::clockwise() {
    if (type == 'O') {
        return;
    } else if (type == 'S') {
        changeS();
        return;
    } else if (type == 'Z') {
        changeZ();
        return;
    } else if (type == 'I') {
        changeI();
        return;
    }
    point botleft = getBotLeft();
    int r = botleft.r;
    int c = botleft.c;
    vector<point> oldPt;
    vector<point> newPt;
    if (type == 'T') {
        if (contains(botleft.r,botleft.c+2)) {
            // bottom horizontal
            oldPt = {{r, c}, {r, c+1}, {r, c+2}, {r-1, c+1}};
            newPt = {{r, c}, {r-1, c}, {r-1, c+1}, {r-2, c}};
        } else if (contains(botleft.r-2,botleft.c)) {
            // left vertical
            oldPt = {{r, c}, {r-1, c}, {r-2, c}, {r-1, c+1}};
            newPt = {{r, c+1}, {r-1, c}, {r-1, c+1}, {r-1, c+2}};
        } else if (contains(botleft.r-1,botleft.c+2)) {
            // middle horizontal
            oldPt = {{r, c+1}, {r-1, c}, {r-1, c+1}, {r-1, c+2}};
            newPt = {{r-1, c}, {r, c+1}, {r-1, c+1}, {r-2, c+1}};
        } else {
            // middle vertical
            oldPt = {{r-1, c}, {r, c+1}, {r-1, c+1}, {r-2, c+1}};
            newPt = {{r, c}, {r, c+1}, {r, c+2}, {r-1, c+1}};
        }
    } else if (type == 'J') {
        if (contains(botleft.r,botleft.c) && contains(botleft.r,botleft.c+2)) {
            // bottom horizontal
            oldPt = {{r, c}, {r, c+1}, {r, c+2}, {r-1, c}};
            newPt = {{r, c}, {r-1, c}, {r-2, c+1}, {r-2, c}};
        } else if (contains(botleft.r-2,botleft.c)) {
            // left vertical
            oldPt = {{r, c}, {r-1, c}, {r-2, c}, {r-2, c+1}};
            newPt = {{r, c+2}, {r-1, c}, {r-1, c+1}, {r-1, c+2}};
        } else if (contains(botleft.r-1,botleft.c+2)) {
            // middle horizontal
            oldPt = {{r, c+2}, {r-1, c}, {r-1, c+1}, {r-1, c+2}};
            newPt = {{r, c}, {r, c+1}, {r-1, c+1}, {r-2, c+1}};
        } else {
            // middle vertical
            oldPt = {{r, c}, {r, c+1}, {r-1, c+1}, {r-2, c+1}};
            newPt = {{r, c}, {r, c+1}, {r, c+2}, {r-1, c}};
        }
    } else if (type == 'L') {
        if (contains(botleft.r,botleft.c+2)) {
            // bottom horizontal
            oldPt = {{r, c}, {r, c+1}, {r, c+2}, {r-1, c+2}};
            newPt = {{r, c}, {r-1, c}, {r-2, c}, {r, c+1}};
        } else if (contains(botleft.r-1,botleft.c) && contains(botleft.r-2,botleft.c)) {
            // left vertical
            oldPt = {{r, c}, {r-1, c}, {r-2, c}, {r, c+1}};
            newPt = {{r, c}, {r-1, c}, {r-1, c+1}, {r-1, c+2}};
        } else if (contains(botleft.r-1,botleft.c+1) && contains(botleft.r-1,botleft.c+2)) {
            // middle horizontal
            oldPt = {{r, c}, {r-1, c}, {r-1, c+1}, {r-1, c+2}};
            newPt = {{r-2, c}, {r, c+1}, {r-1, c+1}, {r-2, c+1}};
        } else {
            // middle vertical
            oldPt = {{r-2, c}, {r, c+1}, {r-1, c+1}, {r-2, c+1}};
            newPt = {{r, c}, {r, c+1}, {r, c+2}, {r-1, c+2}};
        }
    }
    changePoint(oldPt, newPt);
}

void Brick::counterclockwise() {
    if (type == 'O') {
        return;
    } else if (type == 'S') {
        changeS();
        return;
    } else if (type == 'Z') {
        changeZ();
        return;
    } else if (type == 'I') {
        changeI();
        return;
    }
    point botleft = getBotLeft();
    int r = botleft.r;
    int c = botleft.c;
    vector<point> oldPt;
    vector<point> newPt;
    if (type == 'T') {
        if (contains(botleft.r,botleft.c+2)) {
            // bottom horizontal + middle
            oldPt = {{r, c}, {r, c+1}, {r, c+2}, {r-1, c+1}};
            newPt = {{r-1, c}, {r, c+1}, {r-1, c+1}, {r-2, c+1}};
        } else if (contains(botleft.r-2,botleft.c)) {
            // left vertical + middle
            oldPt = {{r, c}, {r-1, c}, {r-2, c}, {r-1, c+1}};
            newPt = {{r, c}, {r, c+1}, {r, c+2}, {r-1, c+1}};
        } else if (contains(botleft.r-1,botleft.c+2)) {
            // middle horizontal + bottom middle
            oldPt = {{r, c+1}, {r-1, c}, {r-1, c+1}, {r-1, c+2}};
            newPt = {{r, c}, {r-1, c}, {r-2, c}, {r-1, c+1}};
        } else {
            // middle vertical + left middle
            oldPt = {{r-1, c}, {r, c+1}, {r-1, c+1}, {r-2, c+1}};
            newPt = {{r, c+1}, {r-1, c}, {r-1, c+1}, {r-1, c+2}};
        }
    }
    if (type == 'J') {
        if (contains(botleft.r,botleft.c) && contains(botleft.r,botleft.c+2)) {
            // bottom horizontal
            oldPt = {{r, c}, {r, c+1}, {r, c+2}, {r-1, c}};
            newPt = {{r, c}, {r, c+1}, {r-1, c+1}, {r-2, c+1}};
        } else if (contains(botleft.r-2,botleft.c)) {
            // left vertical
            oldPt = {{r, c}, {r-1, c}, {r-2, c}, {r-2, c+1}};
            newPt = {{r, c}, {r, c+1}, {r, c+2}, {r-1, c}};
        } else if (contains(botleft.r-1,botleft.c+2)) {
            // middle horizontal
            oldPt = {{r, c+2}, {r-1, c}, {r-1, c+1}, {r-1, c+2}};
            newPt = {{r, c}, {r-1, c}, {r-2, c}, {r-2, c+1}};
        } else {
            // middle vertical
            oldPt = {{r, c}, {r, c+1}, {r-1, c+1}, {r-2, c+1}};
            newPt = {{r, c+2}, {r-1, c}, {r-1, c+1}, {r-1, c+2}};
        }
    }
    if (type == 'L') {
        if (contains(botleft.r,botleft.c+2)) {
            // bottom horizontal
            oldPt = {{r, c}, {r, c+1}, {r, c+2}, {r-1, c+2}};
            newPt = {{r-2, c}, {r, c+1}, {r-1, c+1}, {r-2, c+1}};
        } else if (contains(botleft.r-1,botleft.c) && contains(botleft.r-2,botleft.c)) {
            // left vertical
            oldPt = {{r, c}, {r-1, c}, {r-2, c}, {r, c+1}};
            newPt = {{r, c}, {r, c+1}, {r, c+2}, {r-1, c+2}};
        } else if (contains(botleft.r-1,botleft.c+1) && contains(botleft.r-1,botleft.c+2)) {
            // middle horizontal
            oldPt = {{r, c}, {r-1, c}, {r-1, c+1}, {r-1, c+2}};
            newPt = {{r, c}, {r-1, c}, {r-2, c}, {r, c+1}};
        } else {
            // middle vertical
            oldPt = {{r-2, c}, {r, c+1}, {r-1, c+1}, {r-2, c+1}};
            newPt = {{r, c}, {r-1, c}, {r-1, c+1}, {r-1, c+2}};
        }
    }
    changePoint(oldPt, newPt);
}

bool Brick::checkOccupied(int r, int c) {
    if (r < 0 || r >= board->getHeight() || c < 0 || c >= board->getWidth()) {
        return false;
    }
    if (board->getGrid()[r][c] == nullptr) {
        return true;
    } else if (contains(r, c)) {
        return true;
    } else {
        return false;
    }
}

point Brick::getBotLeft() {
    int row = component[0]->getRow();
    int col = component[0]->getCol();
    size_t len = component.size();
    for (size_t i = 0; i < len; i++) {
        if (row < component[i]->getRow()) {
            row = component[i]->getRow();
        }
        if (col > component[i]->getCol()) {
            col = component[i]->getCol();
        }
    }
    return {row, col};
}

point Brick::getTopLeft() {
    int row = component[0]->getRow();
    int col = component[0]->getCol();
    size_t len = component.size();
    for (size_t i = 0; i < len; i++) {
        if (row > component[i]->getRow()) {
            row = component[i]->getRow();
        }
        if (col > component[i]->getCol()) {
            col = component[i]->getCol();
        }
    }
    return {row, col};
}

void Brick::removeBlock(int r, int c) {
    int row, col;
    int i = 0;
    for (vector<unique_ptr<Block>>::iterator it = component.begin(); it != component.end(); it++) {
    	row = component[i]->getRow();
    	col = component[i]->getCol();
    	if (row == r && col == c) {
    		component.erase(it);
    		break;
    	}
    	++i;
    }
    if (component.empty()) {
        board->removeBrick(this);
    }
}

bool Brick::atBottom() {
	return at_bottom;
}

void Brick::clear() {
	size_t len = component.size();
	for (size_t i = 0; i < len; i++) {
		board->setAt(component[i]->getRow(), component[i]->getCol(), nullptr);
	}
	component.clear();
}

vector<Block*> Brick::getBlocks() {
	vector<Block*> vec;
	size_t len = component.size();
	for (size_t i = 0; i < len; i++) {
		vec.push_back(component[i].get());
	}
	return vec;
}


void Brick::copyComponent(const Brick &other) {
	component.clear();
	size_t len = other.component.size();
	
	for (size_t i = 0; i < len; i++) {
		Block *src = other.component[i].get();
		auto dest = make_unique<Block>(src->getRow(), src->getCol(), type, this);
		component.push_back(std::move(dest));
		
	}
}
int Brick::bottomCount() {
	int botrow = getBotLeft().r;
	int count = 0;
	size_t len = component.size();
	for (size_t i = 0; i < len; i++) {
		if (component[i]->getRow() == botrow) {
			count++;
		}
	}
	return count;
}

void Brick::setStar() {
	if (type == '*') {
		Block *b = component[0].get();
		board->setAt(b->getRow(), b->getCol(), b);
	}
}
