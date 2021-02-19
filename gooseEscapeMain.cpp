/*
Name: Goose Game
Description: Run away from Mr. Goose
Author: Philip DeCroos and Kyle Ferrier
Date: November 27, 2020

References: 
Random numbers - http://www.math.uaa.alaska.edu/~afkjm/csce211/fall2018/handouts
				/RandomFunctions.pdf
*/

////// Beginning of gooseEscapeMain.cpp //////

#include <BearLibTerminal.h>
#include <cmath>
#include <iostream>
#include <ctime>
using namespace std;
#include "gooseEscapeUtil.hpp"
#include "gooseEscapeActors.hpp"
#include "gooseEscapeConsole.hpp"
#include "gooseEscapeGamePlay.hpp"

//set up the console.   Don't modify this line!
Console out;

int main()
{
	//Set up the window.  Don't edit these two lines
    terminal_open();
	terminal_set(SETUP_MESSAGE);
	
	srand(time(NULL));

	/*
	The code below provides a skeleton of the game play.
  	
	Initialise the gameWorld array and any obstacles in the world
	This array stores ascii char values
	*/
	
	int gameWorld[NUM_BOARD_X][NUM_BOARD_Y] = {0};
    
    //Generate walls of 10 and 15 tiles long, vertical and horizontal, respectively
    const int WALL_LENGTH_1 = 10, WALL_LENGTH_2 = 15;
    generateWall(gameWorld, 1, WALL_LENGTH_1);
    generateWall(gameWorld, 0, WALL_LENGTH_2);
    
    //Generate a random number 5 landmines, mysteries and jumboosts.
    const int TOTAL_ITEMS = 5;
    
    for (int numItems = 0; numItems < TOTAL_ITEMS; numItems++)
    {
        generateObject(gameWorld, KABOOM);
        generateObject(gameWorld, MYSTERY);
    	
		//Generate jumboosts in random directions.
		int direction = rand() % 4;
    	
        generateObject(gameWorld, (RIGHT * (direction == 1))
                                 + (LEFT * (direction == 2))
                                 + (DOWN * (direction == 3))
                                 + (UP * (direction == 4)));
    }
    
    //Generate the safe zone in a radnom spot.
	generateObject(gameWorld, WINNER);
  	
	printWorld(gameWorld);
	
	//Initialise the characters in random places that don't match other things
    int player_x0 = 0;
    int player_y0 = 0;
    
    do
    {
    	player_x0 = rand_x();
    	player_y0 = rand_y();
	} while (gameWorld[player_x0][player_y0] != 0);
	
	Actor player(PLAYER_CHAR, player_x0, player_y0);
	
	int goose_x0 = 0;
	int goose_y0 = 0;
	
	do
	{
		goose_x0 = rand_x();
		goose_y0 = rand_y();
	} while (gameWorld[goose_x0][goose_y0] != 0
		|| (goose_x0 == player_x0 && goose_y0 == player_y0));
	
	Actor goose(MONSTER_CHAR, goose_x0, goose_y0);
  	
	// Printing the instructions
    out.writeLine("Escape the Goose! " + goose.get_location_string()
		+ " Use the arrow keys to move.");
	out.writeLine("If the goose catches you, you're toast!");
	out.writeLine("Watch out for landmines and use jump boosters to your advantage.");
	out.writeLine("Mystery cubes either stun you or give the ability to"
					"jump 1 space for 5 moves.");
	
/*
    This is the main game loop.  It continues to let the player give input
    as long so long as they have not: pressed escape or close, been captured by the
	goose, stepped on a mine, or reached the win tile.
	This is checked in the gameOver function.

    All key presses start with "TK_" then the character.  So "TK_A" is the "a"
    key being pressed.
*/
    int keyEntered = TK_A;  // can be any valid value that is not ESCAPE or CLOSE
    bool isPlayersTurn = true;
    
    keyEntered = terminal_read();
    
    while(keyEntered != TK_ESCAPE && keyEntered != TK_CLOSE
    	&& !gameOver(gameWorld, player, goose))
	{
	    if (isPlayersTurn)
	    {
	    	keyEntered = terminal_read();
	    	
    	    movePlayer(keyEntered, player, gameWorld);
    	    player.update_effects(gameWorld);
    	    jumpBoost(player, gameWorld);
    	    effectMessage("You", player);
		}
		else
		{
			moveGoose(player, goose, gameWorld);
			goose.update_effects(gameWorld);
			jumpBoost(goose, gameWorld);
			effectMessage("The goose", goose);
		}
		
	    isPlayersTurn = !isPlayersTurn;
  	}

    if (keyEntered != TK_CLOSE)
    {
        //once the loop has ended, the game is over
        out.writeLine("Game has ended");
    
        // output game over conidtion.
        out.writeLine(endMessage(player, goose, gameWorld));
        
        // Wait until user closes the window
        while (terminal_read() != TK_CLOSE);
    }

	//game is done, close it  
    terminal_close();
}

////// End of gooseEscapeMain.cpp //////
