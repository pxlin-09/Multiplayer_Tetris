#ifndef GAME_H
#define GAME_H
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include "window.h"
#include "board.h"

using std::vector;
using std::string;
using std::ifstream;

class Board;

class Game {
    int playerCount, playerTurn, playerLost, level, height, width, block_size;
	bool color_blind, textOnly;
	Xwindow *xw;
	vector<Board> boards;
    unsigned int seed;
    vector<string> files; 
	vector<ifstream> input_file;
	int hi_score;
	
    public:
        Game(int playerCount, int level, bool textOnly, unsigned int seed, vector<string> files, int width, int height,
		 Xwindow *xw, bool color_blind, int block_size); // constructor
        void playGame(); // read inputs and executive
        void resetGame(); // clear all fields and reset
        void display(); // display outputs
        void nextTurn(); // let the next player make moves
        void specialActions(); // read inputs of special actions
	friend class WDisplay;
	
    private:
    	void prepBoards(); // create the cur and next brick for the next drop
    	void prepNext();
    	void updateHiScore();
    	void restartGame();
    	void nextPlayer();
    	bool gameOver();
    	void displayHint();
    	string getInput(vector<string> cmds, bool &integer_input, int &n);
    	void generateHintStats(char dir, int &lowest, int &moved_for_lowest, int &rotated_for_lowest, int &bot_count);
};

class WDisplay {
		Game *g;
		int block_size;
	public:
		WDisplay(Game *g, int size); // display game boards
		void reNewWindows(); // redraw graphics
	private:
		void drawBorders();
		void drawBlocks();
		void drawHeadings();
		void drawNext();
		void gameOver();
		int getColor(char type);
};


class InvalidInput {
		string inform;
	public:
		InvalidInput(string s) : inform(s) {}
		void display() {std::cout << inform << std::endl;}
};

#endif


