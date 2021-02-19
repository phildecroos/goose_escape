////// Beginning of gooseEscapeGamePlay.hpp //////

#ifndef GOOSE_ESCAPE_GAMEPLAY
#define GOOSE_ESCAPE_GAMEPLAY
#include "gooseEscapeUtil.hpp"
#include "gooseEscapeActors.hpp"
#include "gooseEscapeConsole.hpp"

/*This file is all about the game world.  You will modify this to add
    constants and function prototypes.  Modify gooseGamePlay.cpp to
	actually add functionality.
*/

/*
    Declare constants to indicate various game world features in the board
    array.  Modify them to fit what you would like to do in the game.  You can
    change the type if you choose to store your game board as something other
    than intengers.
*/	
// Going further:  Learn how to use an enum for these values
const int SHALL_NOT_PASS = int('o'); // Wall
const int WINNER = int('%'); //Safe Zone
const int KABOOM = int('#'); //Landmine

// Jump Boosts
const int RIGHT = int('>');
const int LEFT = int('<');
const int UP = int('^');
const int DOWN = int('v');

//display characters
const int PLAYER_CHAR = int('@');
const int MONSTER_CHAR = int('G');

/*
    Game play function prototypes are give below.
*/

int rand_x();

int rand_y();

void generateWall(int world[NUM_BOARD_X][NUM_BOARD_Y], bool direction, int length);

void generateObject(int world[NUM_BOARD_X][NUM_BOARD_Y], int character);

void printWorld(int world[NUM_BOARD_X][NUM_BOARD_Y]);

void movePlayer(int key, Actor & player, int board[NUM_BOARD_X][NUM_BOARD_Y]);

void moveGoose(Actor const & player, Actor & goose,
	int board[NUM_BOARD_X][NUM_BOARD_Y]);

void effectMessage(string whoDoneIt, Actor const & being);

void jumpBoost(Actor & character, int world[NUM_BOARD_X][NUM_BOARD_Y]);

bool captured(Actor const & player, Actor const & goose);

bool steppedOn(int world[NUM_BOARD_X][NUM_BOARD_Y], Actor const & being,
    int character);

bool gameOver(int world[NUM_BOARD_X][NUM_BOARD_Y], Actor const & player,
	Actor const & goose);

string endMessage(Actor const & player, Actor const & goose,
	int gameWorld[NUM_BOARD_X][NUM_BOARD_Y]);

#endif

////// End of gooseEscapeGamePlay.hpp //////
