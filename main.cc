#include <iostream>
#include <vector>
#include <string>
#include "game.h"
#include "window.h"
using namespace std;

int main(int arg_num, char** args) {
    cin.exceptions(ios::failbit);
    int player = 2;
    bool textOnly = false;
    unsigned int seed = 1;
    vector<string> files;
	files.emplace_back("biquadris_sequence1.txt");
	files.emplace_back("biquadris_sequence2.txt");
    int level = 0;
    int width = 11;
    int height = 15;
    bool enable_bonus = false;
    bool color_blind = false;

    try {
	    for (int i = 1; i < arg_num; i++) {
	        string arg = string(args[i]);
	        if (arg == "-enablebonus") {
	            enable_bonus = true;
	        } else if (arg == "-text") {
	            textOnly = true;
	        } else if (arg == "-seed") {
	            if (i < arg_num - 1) {
	                i += 1;
	            } else {
	                break;
	            }
	            seed = stoi(args[i]);
	        } else if (arg.substr(0, 11) == "-scriptfile") {
	            int idx = stoi(arg.substr(11)) - 1;
	            if (i < arg_num - 1) {
	                i += 1;
	            } else {
	                break;
	            }
	            files[idx] = args[i];
	        } else if (arg == "-startlevel") {
	            if (i < arg_num - 1) {
	                i += 1;
	            } else {
	                break;
	            }
	            level = stoi(args[i]);
	        }
	    }
	} catch (std::exception &ex) {
		cout << "Invalid Arguments" << endl;
		return 1;
	}

    if (enable_bonus) {
        // input # of players
        cout << "Enter number of players: (min:1, max:7)" << endl;
        string cmd;
        cin >> cmd;
        std::string::const_iterator it = cmd.begin();
        while (it != cmd.end() && std::isdigit(*it)) ++it;
        if (!cmd.empty() && it == cmd.end()) {
            player = stoi(cmd);
            player = (player < 1)? 1 : player;
	    player = (player > 7)? 7 : player;
        } else {
            cout << "Invalid Input. Using default value." << endl;
        }

		try {
			cout << "Enter width: (min:10, max:20)" << endl;
				cin >> width;
			cout << "Enter height: (min:10, max:20)" << endl;
			cin >> height;
		} catch (std::exception &ex) {
			cin.clear();
			cin.ignore();
			cout << "Invalid Input. Using default value." << endl;
		}
        width = (width < 10)? 10 : width;
        height = (height < 10)? 10 : height;
		width = (width > 20)? 20 : width;
        height = (height > 20)? 20 : height;
		if (!textOnly) {
			cout << "Do you wish to turn on Color Blind Mode? (Y for yes, any other for no):" << endl;
			cin >> cmd;
			if (cmd == "Y" || cmd == "y") color_blind = true;
		}
    }

    for (int j = 2; j < player; j++) {
        if (j % 2 == 0) {
            files.emplace_back(files[0]);
        } else {
            files.emplace_back(files[1]);
        }
    }
    int block_size = 20; 
    if (player > 3) block_size = 15;
    if (player > 5) block_size = 10;

    if (!textOnly) {
	int window_width = block_size*(player * width) + block_size*(player -1);
    	int window_height = block_size * (height + 3) + 100 + 2*block_size;
	Xwindow xw{window_width, window_height};
	Game g{player, level, textOnly, seed, files, width, height, &xw, color_blind, block_size};
	g.playGame();
    } else {
	Game g{player, level, textOnly, seed, files, width, height, nullptr, color_blind, block_size};
	g.playGame();
    } 
}
