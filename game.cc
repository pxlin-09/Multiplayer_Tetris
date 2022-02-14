#include "game.h"
#include <iostream>
#include <string>
#include <vector>
#include "board.h"
#include <sstream>
#include <iomanip>
#include <fstream>
#include "window.h"

using namespace std;

struct point;

Game::Game(int playerCount, int level, bool textOnly, unsigned int seed, vector<string> files, int width, int height, 
		Xwindow *xw, bool color_blind, int block_size) 
	: playerCount(playerCount), playerTurn(0), playerLost(-1), level(level),height(height),width(width), block_size(block_size), color_blind(color_blind), textOnly(textOnly),xw(xw), seed(seed), files(files), 
	  hi_score(0) {	
    for (int i = 0; i < playerCount; i++) {
        boards.emplace_back(level, height + 3, width, seed, i+1);
    }
}

// produces a string with all alphabet in str to upper case
string upperCase(string str) {
  int len = str.length();
  string out = "";
  for (int i = 0; i < len; i++) {
    if ('a' <= str[i] && str[i] <= 'z') {
      char ch = str[i]-'a'+'A';
      out += ch;
    } else {
      out += str[i];
    }
  }
  return out;
}

// produces a string with all alphabet in str to lower case
string lowerCase(string str) {
  int len = str.length();
  string out = "";
  for (int i = 0; i < len; i++) {
    if ('A' <= str[i] && str[i] <= 'Z') {
      char ch = str[i]-'A'+'a';
      out += ch;
    } else {
      out += str[i];
    }
  }
  return out;
}

// finds the index of a closest match of in to cmd or produces -1 if no match
int findCommand(string in, vector<string> cmds) {
    int len = in.length();
    int found = false;
    int idx = -1;
    string in_up = upperCase(in);
    string in_lo = lowerCase(in);
    int cmds_count = cmds.size();
    for (int i = 0; i < cmds_count; i++) {
    	if (in_lo == cmds[i] || in_up == cmds[i]) return i;
	}
    for (vector<string>::iterator it = cmds.begin(); it != cmds.end(); it++) {
        if (!found) idx++;
        if (in_lo == (*it).substr(0,len) || in_up == (*it).substr(0,len)) {
            if (found) return -1;
        	found = true;
        }
    }
    if (!found) return -1;
    return idx;
}

// reads input and produces the command match in cmds or throws exception
string Game::getInput(vector<string> cmds, bool &integer_input, int &n) {
    cin.exceptions(std::ios::failbit|std::ios::eofbit);
    integer_input = true;
    try {
    	if (cmds.size() >= 20) cout << "Player " << playerTurn + 1 << " enter command:" << endl;
        cin >> n;
    } catch (std::exception &ex) {
        integer_input = false;
        cin.clear();
    }
    string in;
    cin >> in;
    int idx = findCommand(in,cmds);
    if (idx == -1 || n < 0) throw InvalidInput{"Invalid Input"};
    if (cin.fail() || cin.eof()) throw;
    return cmds[idx];
}

string getFileInput(string cmd, vector<string> cmds, bool &integer_input, int &n) {
    int i = 0;
    if (!integer_input) {
        integer_input = true;
        std::string::const_iterator it = cmd.begin();
        while (it != cmd.end() && std::isdigit(*it)) {
            if (*it <= '9' && *it >= '0') {
                n = n * 10 + *it - '0';
            }
            it++;
            i++;
        }
        if (it == cmd.begin()) integer_input = false;
    }
    string in = cmd.substr(i);
    int idx = findCommand(in,cmds);
    if (idx == -1 || n < 0) cout << "Invalid Input" << endl;
    return cmds[idx];
}

void Game::display() {
    if (!textOnly) {
    	WDisplay d{this, block_size};
   		d.reNewWindows(); 
    }
    if (gameOver()) return;
    cout << "hiscore:" << setfill(' ') << setw(boards[0].getWidth() - 8) << hi_score << endl;
    // level + 4 space + int, 6 spaces inbetween
    for (int i = 0; i < playerCount; i++) {
        cout << "Player:" << string (boards[i].getWidth() - 8, ' ') << i+1;
        if (i < playerCount - 1) {
            cout << string(6, ' ');
        } else {
            cout << endl;
        }
    }
    
    for (int i = 0; i < playerCount; i++) {
        cout << "Level:" << string (boards[i].getWidth() - 7, ' ') << boards[i].getLevel();
        if (i < playerCount - 1) {
            cout << string(6, ' ');
        } else {
            cout << endl;
        }
    }

    // same as above but score int can take more spaces from the 4
    for (int i = 0; i < playerCount; i++) {
        cout << "Score:" << setfill(' ') << setw(boards[i].getWidth() - 6) << boards[i].getScore();
        if (i < playerCount - 1) {
            cout << "      ";
        } else {
            cout << endl;
        }
    }

    for (int i = 0; i < playerCount; i++) {
        cout << string (boards[i].getWidth(), '-');
        if (i < playerCount - 1) {
            cout << string(6, ' ');
        } else {
            cout << endl;
        }
    }

    int height = boards[0].getHeight();
    int width = boards[0].getWidth();
    for (int r = 0; r < height; r++) {
        for (int i = 0; i < playerCount; i++) {
            for (int c = 0; c < width; c++) {
                cout << boards[i].displayAt(r,c);
            }
            if (i < playerCount - 1) {
                cout << string(6, ' ');
            } else {
                cout << endl;
            }
        }
    }
    for (int i = 0; i < playerCount; i++) {
        cout << string (boards[i].getWidth(), '-');
        if (i < playerCount - 1) {
            cout << string(6, ' ');
        } else {
            cout << endl;
        }
    }
	
    char type = boards[playerTurn].getNextBrick()->getType();
    for (int i = 0; i < playerCount; i++) {
        cout << "Next:" << string(boards[i].getWidth() - 5, ' ');
        if (i < playerCount - 1) {
            cout << string(6, ' ');
        } else {
            cout << endl;
        }
    }

    if (type == 'S') {
		for (int i = 0; i < playerCount; i++) {
            if (i != playerTurn) {
                cout << string(boards[i].getWidth(), ' ');
            } else {
                cout << " SS" << string(boards[i].getWidth() - 3, ' ');
            }
            if (i < playerCount - 1) {
                cout << string(6, ' ');
            } else {
                cout << endl;
            }
        }
        for (int i = 0; i < playerCount; i++) {
            if (i != playerTurn) {
                cout << string(boards[i].getWidth(), ' ');
            } else {
                cout << "SS" << string(boards[i].getWidth() - 2, ' ');
            }
            if (i < playerCount - 1) {
                cout << string(6, ' ');
            } else {
                cout << endl;
            }
        }
	} else if (type == 'Z') {
		for (int i = 0; i < playerCount; i++) {
            if (i != playerTurn) {
                cout << string(boards[i].getWidth(), ' ');
            } else {
                cout << "ZZ" << string(boards[i].getWidth() - 2, ' ');
            }
            if (i < playerCount - 1) {
                cout << string(6, ' ');
            } else {
                cout << endl;
            }
        }
        for (int i = 0; i < playerCount; i++) {
            if (i != playerTurn) {
                cout << string(boards[i].getWidth(), ' ');
            } else {
                cout << " ZZ" << string(boards[i].getWidth() - 3, ' ');
            }
            if (i < playerCount - 1) {
                cout << string(6, ' ');
            } else {
                cout << endl;
            }
        }
	} else if (type == 'J') {
		for (int i = 0; i < playerCount; i++) {
            if (i != playerTurn) {
                cout << string(boards[i].getWidth(), ' ');
            } else {
                cout << "J" << string(boards[i].getWidth() - 1, ' ');
            }
            if (i < playerCount - 1) {
                cout << string(6, ' ');
            } else {
                cout << endl;
            }
        }
        for (int i = 0; i < playerCount; i++) {
            if (i != playerTurn) {
                cout << string(boards[i].getWidth(), ' ');
            } else {
                cout << "JJJ" << string(boards[i].getWidth() - 3, ' ');
            }
            if (i < playerCount - 1) {
                cout << string(6, ' ');
            } else {
                cout << endl;
            }
        }
	} else if (type == 'L') {
		for (int i = 0; i < playerCount; i++) {
            if (i != playerTurn) {
                cout << string(boards[i].getWidth(), ' ');
            } else {
                cout << "  L" << string(boards[i].getWidth() - 3, ' ');
            }
            if (i < playerCount - 1) {
                cout << string(6, ' ');
            } else {
                cout << endl;
            }
        }
        for (int i = 0; i < playerCount; i++) {
            if (i != playerTurn) {
                cout << string(boards[i].getWidth(), ' ');
            } else {
                cout << "LLL" << string(boards[i].getWidth() - 3, ' ');
            }
            if (i < playerCount - 1) {
                cout << string(6, ' ');
            } else {
                cout << endl;
            }
        }
	} else if (type == 'O') {
		for (int i = 0; i < playerCount; i++) {
            if (i != playerTurn) {
                cout << string(boards[i].getWidth(), ' ');
            } else {
                cout << "OO" << string(boards[i].getWidth() - 2, ' ');
            }
            if (i < playerCount - 1) {
                cout << string(6, ' ');
            } else {
                cout << endl;
            }
        }
        for (int i = 0; i < playerCount; i++) {
            if (i != playerTurn) {
                cout << string(boards[i].getWidth(), ' ');
            } else {
                cout << "OO" << string(boards[i].getWidth() - 2, ' ');
            }
            if (i < playerCount - 1) {
                cout << string(6, ' ');
            } else {
                cout << endl;
            }
        }
	} else if (type == 'T') {
		for (int i = 0; i < playerCount; i++) {
            if (i != playerTurn) {
                cout << string(boards[i].getWidth(), ' ');
            } else {
                cout << "TTT" << string(boards[i].getWidth() - 3, ' ');
            }
            if (i < playerCount - 1) {
                cout << string(6, ' ');
            } else {
                cout << endl;
            }
        }
        for (int i = 0; i < playerCount; i++) {
            if (i != playerTurn) {
                cout << string(boards[i].getWidth(), ' ');
            } else {
                cout << " T" << string(boards[i].getWidth() - 2, ' ');
            }
            if (i < playerCount - 1) {
                cout << string(6, ' ');
            } else {
                cout << endl;
            }
        }
	} else if  (type == 'I'){
		for (int i = 0; i < playerCount; i++) {
            cout << string(boards[i].getWidth(), ' ');
            if (i < playerCount - 1) {
                cout << string(6, ' ');
            } else {
                cout << endl;
            }
        }
        for (int i = 0; i < playerCount; i++) {
            if (i != playerTurn) {
                cout << string(boards[i].getWidth(), ' ');
            } else {
                cout << "IIII" << string(boards[i].getWidth() - 4, ' ');
            }
            if (i < playerCount - 1) {
                cout << string(6, ' ');
            } else {
                cout << endl;
            }
        }
	}
}

// determines if cmd is a command that moves the current brick
bool moveCmd(string cmd) {
	vector<string> cmds = {"left", "right", "down", "clockwise", "counterclockwise", "cc"};
	size_t len = cmds.size();
	for (size_t i = 0; i < len; i++) {
		if (cmd == cmds[i]) return true;
	}
	return false;
}

void Game::prepBoards() {
	for (int i = 0; i < playerCount; i++) {
        if (level == 0) {
            char type;
            do {
                input_file[i].get(type);
                if (type != ' ') {
                    boards[i].createBrick(type);
                }
            } while (type == ' ');
        } else {
            boards[i].createBrick();
        }
    }
}

void Game::updateHiScore() {
    int hi = hi_score;
    for (int i = 0; i < playerCount; i++) {
        if (hi < boards[i].getScore()) hi = boards[i].getScore();
    }
    if (hi > hi_score) hi_score = hi;
}

bool Game::gameOver() {
	size_t len = boards.size();
	for (size_t i = 0; i < len; i++) {
		if (!boards[i].getLost()) return false;
	}
	return true;
}

void Game::playGame() {
    ifstream sequencefile;
    for (int i = 0; i < playerCount; i++) {
        input_file.emplace_back(files[i]);
    }
    vector<string> cmds = {"left", "right", "down", "clockwise", "counterclockwise",
                            "drop", "levelup", "leveldown", "norandom", "random",
                            "sequence", "I", "J", "L", "O", "S", "Z", "T", "restart", "cc", "hint"};
    prepBoards();
    prepNext();
	display();
	
    while (true) {
    	bool ask_for_hint = false;
		string cmd;
    	if (gameOver()) {
    		int winner = 0;
    		int current_hi = -1;
    		for (int i = 0; i < playerCount; i++) {
    			if (boards[i].getScore() > current_hi) {
    				winner = i + 1;
    				current_hi = boards[i].getScore();
				}
			}
			cout << "Player " << winner << " has won with a score of " << current_hi << "!" << endl;
			cout << "Play again? (Y for yes, anything else for no):" << endl;
			cin >> cmd;
			if (cmd != "Y" && cmd != "y") break;
    		restartGame();
    		display();
		}
		int cleared;
		int n;
		bool integer_input;
		if (sequencefile.is_open()) {
            sequencefile >> cmd;
            std::string::const_iterator it = cmd.begin();
            integer_input = false;
            while (it != cmd.end() && std::isdigit(*it)) ++it;
            if (!cmd.empty() && it == cmd.end()) {
                n = stoi(cmd);
                sequencefile >> cmd;
                integer_input = true;
            }/*
            if (sequencefile.eof()) {
                sequencefile.close();
            } */
	    if (cmd != "\0") {
            	cmd = getFileInput(cmd, cmds, integer_input, n);
	    }else if (sequencefile.eof()) {
	    	sequencefile.close();
	    }
	    }

		if (!sequencefile.is_open()) {
            try {
                cmd = getInput(cmds, integer_input, n);
            } catch (InvalidInput inv) {
                inv.display();
                continue;
            } catch (std::exception &ex) {
                break;
            }
		}

		if (!integer_input) {
            n = 1;
		}
		for (int i = 0; i < n; i++) {
		    if (cmd == "left") {
		        boards[playerTurn].moveLeft();
		    } else if (cmd == "right") {
		        boards[playerTurn].moveRight();
		    } else if (cmd == "down") {
		        boards[playerTurn].moveDown();
		    } else if (cmd == "clockwise") {
		        boards[playerTurn].clockwise();
		    } else if (cmd == "counterclockwise" || cmd == "cc") {
		        boards[playerTurn].counterclockwise();
		    } else if (cmd == "drop") {
		        cleared = boards[playerTurn].update();
		        updateHiScore();
		        boards[playerTurn].createBrick();
		        if (gameOver()) {
		        	break;
				}
		        if (boards[playerTurn].getLost()) {
		        	nextPlayer();
		        	prepNext();
		        	break;
				} 
		        if (i+1 == n) nextPlayer();
				prepNext();
		        if (i+1 != n) continue;
		        if (cleared >= 2) {
		        	display();
		        	specialActions();
		        }
		    } else if (cmd == "levelup") {
		        boards[playerTurn].levelup();
		    } else if (cmd == "leveldown") {
		        boards[playerTurn].leveldown();
		    } else if (cmd == "restart") {
		    	restartGame();
			} else if (cmd == "I" || cmd == "J" || cmd == "L"|| cmd == "O" ||
						cmd == "S" || cmd == "Z" || cmd == "T"){
				boards[playerTurn].changeCurrent(cmd[0]);
			} else if (cmd == "norandom") {
                if (boards[playerTurn].getLevel() == 3 || boards[playerTurn].getLevel() == 4) {
                    string fname;
                    ifstream infile;
		    while (true){
			    if (sequencefile.is_open()) {
		    		sequencefile >> fname;
			    }else {
				cin >> fname;
			    }
			infile.open(fname);
                        if (infile.is_open()) break;
                        infile.close();
                        cout << "Invalid Input" << endl;
			}
		   	
                    infile.close();
                    input_file[playerTurn].close();
                    input_file[playerTurn].open(fname);
                    boards[playerTurn].setRandom(false);
                }
			} else if (cmd == "random") {
			    boards[playerTurn].setRandom(true);
            } else if (cmd == "sequence") {
                string fname;
                while (true) {
                    cin >> fname;
                    sequencefile.open(fname);
                    if (sequencefile.is_open()) break;
                    sequencefile.close();
                    cout << "Invalid Input" << endl;
                }
            } else if (cmd == "hint") {
				displayHint();
				ask_for_hint = true;
				break;
			}
		    if (boards[playerTurn].getSpecial() == 2 && (cmd == "left" || cmd == "right")) {
				boards[playerTurn].moveDown();
				boards[playerTurn].moveDown();
				if (boards[playerTurn].atBottom()) {
					cleared = boards[playerTurn].update();
					if(gameOver()) continue;
					nextTurn();
					if (cleared >= 2) {
						display();
						specialActions();
					}
					break;
				}
			}
			if (i == n-1 && moveCmd(cmd) && boards[playerTurn].getLevel() > 2) {
			  boards[playerTurn].moveDown();
				if (boards[playerTurn].atBottom()) {
					cleared = boards[playerTurn].update();
					if(gameOver()) continue;
						nextTurn();
					if (cleared >= 2) {
						display();
						specialActions();
					}
					break;
				}
			}	
		}
		
		if (!ask_for_hint) display();
	}
}

void Game::restartGame() {
	boards.clear();
	for (int i = 0; i < playerCount; i++) {
		boards.emplace_back(level, height + 3, width, seed, i+1);
	}
	playerTurn = 0;
	playerLost = -1;
	for (int i = 0; i < playerCount; i++) {
        input_file[i].close();
        input_file[i].open(files[i]);
    }
    prepBoards();
    prepNext();
}

void Game::nextPlayer() {
	while (true) {
		playerTurn = (playerTurn + 1) % playerCount;
		if (!boards[playerTurn].getLost()) break;
	}
}

void Game::nextTurn() {
	boards[playerTurn].createBrick();
	nextPlayer();
	prepNext();
}

void Game::specialActions() {
	int alive = 0;
	for (int i = 0; i < playerCount; i++ ) {
		if (!boards[i].getLost()) alive++;
	}
	if (alive == 1) return; // no need to do special actions to yourself
	
	string cmd;
	int n;
	cout << "Choose a special action!" << endl;
	vector<string> cmds = {"blind", "heavy", "force"};
	vector<string> charcmds = {"I", "J", "L", "O", "S", "Z", "T"};
	bool integer_input;
	while(true) {
		try {
			cmd = getInput(cmds, integer_input, n);
			break;
		} catch (InvalidInput inv) {
			inv.display();
			continue;
		} catch (std::exception &ex) {
			return;
		}
	}
	if (cmd == "blind") {
		boards[playerTurn].changeSpecial(1);
	} else if (cmd == "heavy") {
		boards[playerTurn].changeSpecial(2);
	} else if (cmd == "force") {
		while(true) {
			try {
				cmd = getInput(charcmds, integer_input, n);
				break;
			} catch (InvalidInput inv) {
				inv.display();
				continue;
			} catch (std::exception &ex) {
				return;
			}
		}
		boards[playerTurn].changeCurrent(cmd[0]);
	}
}

void Game::prepNext() {
	if (boards[playerTurn].getLevel() == 0 ||
     ((boards[playerTurn].getLevel() == 3 || boards[playerTurn].getLevel() == 4)
      && !boards[playerTurn].getRandom())) {
        char type;
        do {
            if (input_file[playerTurn].get(type)) {
                if (type != ' ') {
                    boards[playerTurn].createNext(type);
                }
            } else {
                input_file[playerTurn].close();
                input_file[playerTurn].open(files[playerTurn]);
                input_file[playerTurn].get(type);
                if (type != ' ') {
                    boards[playerTurn].createNext(type);
                }
            }
        } while (type == ' ' || type == '\n');
    } else {
        boards[playerTurn].createNext();
    }
}

void Game::generateHintStats(char dir, int &lowest, int &moved_for_lowest, int &rotated_for_lowest, int &bot_count) {
	Brick *cur = boards[playerTurn].getCurBrick();
	int cur_c = cur->getBotLeft().c;
	int left_space = cur_c;
	int right_space = boards[playerTurn].getWidth() - cur_c;
	// we can try more than enough left or right because 
		// we only record the first best outcome
	int spaces = (dir == 'L')? left_space : right_space;
	spaces = boards[playerTurn].getWidth();
	for (int i = 0; i < spaces; i++) {
		int rotated = 0;
		for (int rot = 0; rot < 4; rot++) {
			Board new_b = boards[playerTurn];
			cur = new_b.getCurBrick();
			for (int k = 0; k < rot; k++) {
				new_b.clockwise();
				if (k == rot - 1 && new_b.getLevel() > 2) new_b.moveDown();
			}
			rotated = rot;
			for (int j = 0; j < i; j++) {
				if (!new_b.atBottom()) {
					if (dir == 'L') new_b.moveLeft();
					if (dir == 'R') new_b.moveRight();
				}
				if (j == i - 1 && new_b.getLevel() > 2) new_b.moveDown();
				if (new_b.getSpecial() == 2) {
					new_b.moveDown();
					new_b.moveDown();
				}
			}
			new_b.update();
			cur = new_b.getCurBrick();
			if (cur->getBotLeft().r >= lowest) {
				if (cur->bottomCount() > bot_count && cur->getBotLeft().r == lowest) {
					if (dir == 'L') moved_for_lowest = -i;
					if (dir == 'R') moved_for_lowest = i;
					rotated_for_lowest = rotated;
					bot_count = cur->bottomCount();
				} else if (cur->getBotLeft().r > lowest) {
					lowest = cur->getBotLeft().r;
					if (dir == 'L') moved_for_lowest = -i;
					if (dir == 'R') moved_for_lowest = i;
					rotated_for_lowest = rotated;
					bot_count = cur->bottomCount();
				}	
			}	
		}
	}
}

void Game::displayHint() {
	int lowest = 0;
	int bot_count = 0;
	int moved_for_lowest = 0;
	int rotated_for_lowest = 0;
	generateHintStats('L', lowest, moved_for_lowest, rotated_for_lowest, bot_count);
	generateHintStats('R', lowest, moved_for_lowest, rotated_for_lowest, bot_count);
	
	string out = "Try: ";
	if (rotated_for_lowest) out = out + to_string(rotated_for_lowest) +"clockwise ";
	if (moved_for_lowest == 0) {
		out += "drop";
	} else if (moved_for_lowest < 0) {
		out += to_string(-moved_for_lowest) + "left drop";
	} else {
		out = out + to_string(moved_for_lowest) + "right drop";
	}
	cout << out << endl;
	return;
}

WDisplay::WDisplay(Game *g, int size) : g(g), block_size(size) {} 

void WDisplay::drawBorders() {
	Xwindow *xw = g->xw;
	int width = g->width;
	xw->fillRectangle(0,50,xw->getWidth(), block_size, Xwindow::Black);
	xw->fillRectangle(0, xw->getHeight() - 50 - block_size, xw->getWidth(), block_size, Xwindow::Black);
	int divider_x = 0;
	for (int i = 1; i < g->playerCount; i++) {
		xw->fillRectangle(divider_x + width *block_size, 50+block_size, block_size, 
							xw->getHeight()-100 - 2*block_size, Xwindow::Black);
		divider_x += width *block_size + block_size;
	}
}

void WDisplay::drawBlocks() {
	int board_width = g->boards[0].getWidth();
	int board_height = g->boards[0].getHeight();
	for (int i = 0; i < g->playerCount; i++) {
		int previous_spaces = (board_width + 1) * i * block_size;
		for (int r = 0; r < board_height; r++) {
			for (int c = 0; c < board_width; c++) {
				char type = g->boards[i].displayAt(r,c);
				int color = getColor(type);
				if (type == ' ') continue;
				int window_x = previous_spaces + c*block_size;
				int window_y = 50 + block_size + r * block_size;
				g->xw->fillRectangle(window_x, window_y, block_size, block_size, color);
					
				
			}
		}
	}
	
}

int WDisplay::getColor(char type) {
	if (g->color_blind) return Xwindow::Grey;
	if (type == '*') return Xwindow::Brown;
	if (type == 'I') return Xwindow::Cyan;
	if (type == 'J') return Xwindow::Blue;
	if (type =='S') return Xwindow::Green;
	if (type == 'O') return Xwindow::Purple;
	if (type == 'Z') return Xwindow::Orange;
	if (type == 'L') return Xwindow::Pink;
	if (type == 'T') return Xwindow::Red;
	if (type == '?') return Xwindow::Yellow;
	return Xwindow::White;  // should never be here;
}

void WDisplay::drawHeadings() {
	int board_width = g->boards[0].getWidth();
	int indent_text = 10;
	g->xw->drawString(indent_text,10, "Hi score: " + to_string(g->hi_score));
	for (int i = 0; i < g->playerCount; i++) {
		int previous_spaces = (board_width + 1) * i * block_size;
		int level = g->boards[i].getLevel();
		int score = g->boards[i].getScore();
		g->xw->drawString(previous_spaces + indent_text, 25, "Player: " + to_string(i+1));
		g->xw->drawString(previous_spaces + indent_text, 35, "Level: " + to_string(level));
		g->xw->drawString(previous_spaces + indent_text, 45, "Score: " + to_string(score));
	}
}

void WDisplay::drawNext() {
	int board_width = g->boards[0].getWidth();
	int indent_text = 10;
	int from_bottom = g->xw->getHeight() - 40;
	int previous_spaces = (board_width + 1) * g->playerTurn * block_size;
	g->xw->drawString(previous_spaces + indent_text, from_bottom, "Next:");
	
	int extra_space = 3; // we left 3 empty rows at top of board for rotation
	vector<Block*> blocks = g->boards[g->playerTurn].getNextBrick()->getBlocks();
	size_t len = blocks.size();
	for (size_t i = 0; i < len; i++) {
		int x = block_size * blocks[i]->getCol() + previous_spaces + 50;  // + 50 to be next to the text "Next:"
		int y = block_size * (blocks[i]->getRow() - extra_space) + (from_bottom - 5);  // - 5 to move the blocks a little higher for better visual effects
		char type = blocks[i]->getType();
		int color = getColor(type);
		g->xw->fillRectangle(x, y, block_size, block_size, color);
	}
}

void WDisplay::gameOver() {
	g->xw->fillRectangle(g->xw->getWidth()/2,g->xw->getWidth()/2,
						g->xw->getWidth(), 20, Xwindow::White);
	g->xw->drawString(g->xw->getWidth()/2,g->xw->getWidth()/2, "Player "+to_string(g->playerTurn+1)+" won!");
}

void WDisplay::reNewWindows() {
	g->xw->fillRectangle(0,0,g->xw->getWidth(), g->xw->getHeight(), Xwindow::White);
	if (g->gameOver()) {
		gameOver();
		return;
	}
	drawHeadings();
	drawBorders();
	drawBlocks();
	drawNext();	
}
