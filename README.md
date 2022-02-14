# Multiplayer_Tetris
Play Tetris locally with friends!

By: Alex Lin, William Hou, Jasmine Xu



Written in C++, tetris game with multiple features such as arbitrary (logical) widths and height. Supports single player, as well as up to 7 players. 

Detailed Features:

Commandline input for initialization, can be used to enable multiplayer, customize width/height, and colour blind mode

In-game commandline inputs include but not limited to (see demo and design pdf for more details):

(multiplier) drop, left, right, hint

Power ups given for gaining set amount of points, 3 special moves to make opponent blocks fall faster, cover their board, or choose their next block

Hint command: when used, computer computes the best combination for you current block to fall, where the maximum points can be gained, or the lowest the block can reach

Auto-correct: input does not have to be typed out entirely, program can detect and make sense of what users mean if a match can be found. For example, "dr" will be recognized as "drop" command.

4 different levels with different difficulties, user can also choose random or give a seed for the sequence of types of blocks that will be next (ex. sequence.txt = "I" will only allow I blocks to appear)

the TX.[in|args] were used as tests inputs and arguments to detect any memory leak, segmentation faults, etc.

The follow are some combination of features:

Normal Game looks like:

<img width=500 src="https://github.com/pxlin-09/Multiplayer_Tetris/blob/master/s1.png"> 

Single player with colourblind mode:

<img width=500 src="https://github.com/pxlin-09/Multiplayer_Tetris/blob/master/s2.png"> 

Multiplayer of 4, with customized width and height:

<img width=500 src="https://github.com/pxlin-09/Multiplayer_Tetris/blob/master/s3.png"> 
