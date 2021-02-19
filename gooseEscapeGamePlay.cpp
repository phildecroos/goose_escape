////// Beginning of gooseEscapeGamePlay.cpp //////

#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>
using namespace std;
#include <BearLibTerminal.h>
#include "gooseEscapeUtil.hpp"
#include "gooseEscapeActors.hpp"
#include "gooseEscapeConsole.hpp"
#include "gooseEscapeGamePlay.hpp"

extern Console out;


/*
With graphics, screens are given an x,y coordinate system with the origin
in the upper left corner.  So it means the coordinate axes are:
---------------->   x direction
|
|
|
|
|
V

y direction 
*/

// Generates random positions in the gameworld. Repeated many times in initial code.
int rand_x()
{
	return rand() % MAX_BOARD_X;
}

int rand_y()
{
	return rand() % MAX_BOARD_Y;
}

// Direction 1 (true) will represent a virtical wall.
void generateWall(int world[NUM_BOARD_X][NUM_BOARD_Y], bool direction, int length)
{
	/*
	The length of the wall is only considered for the upper bound of the axis
	on which the wall will be generated. This ensures the wall has enough space
	before the world boundary in order to generate in a linear fashion.
	*/
	int x_edit = rand() % (MAX_BOARD_X - !direction * length) + MIN_BOARD_X;
	int y_edit = rand() % (MAX_BOARD_Y - direction * length) + MIN_BOARD_Y;
	
	//Build a wall in the increasing direction.
	for (int counter = 0; counter < length; counter ++)
	{
		if (world[x_edit][y_edit] == 0)
		{
			world[x_edit][y_edit] = SHALL_NOT_PASS;
		}
		
		x_edit += !direction;
		y_edit += direction;
	}
}

/*
Places out the deired char object on the board array at any random location that
does not overlap another already generated object.
*/
void generateObject(int world[NUM_BOARD_X][NUM_BOARD_Y], int object)
{
    int object_x = 0;
    int object_y = 0;
    
    do
    {
        object_x = rand_x();
        object_y = rand_y();
    } while (world[object_x][object_y] != 0);
    
    world[object_x][object_y] = object;
}

// Print the game board array.
void printWorld(int world[NUM_BOARD_X][NUM_BOARD_Y])
{
	//Cycle through and update the terminal for each index of the game world.
	for (int xIndex = 0; xIndex < NUM_BOARD_X; xIndex ++)
	{
		for (int yIndex = 0; yIndex < NUM_BOARD_Y; yIndex ++)
		{
			//Outputs the character values of each ASCII array location.
			terminal_put(xIndex, yIndex, world[xIndex][yIndex]);
		}
	}
	
    terminal_refresh();
}

/*
Move the player to a new location based on their input and the current applied
movement effects. E.g., stunned or jump boosted
*/
void movePlayer(int key, Actor & player, int board[NUM_BOARD_X][NUM_BOARD_Y])
{
    int yMove = 0, xMove = 0;
    if (key == TK_UP)
        yMove = -player.get_speed();
    else if (key == TK_DOWN)
        yMove = player.get_speed();
    else if (key == TK_LEFT)
        xMove = -player.get_speed();
    else if (key == TK_RIGHT)
        xMove = player.get_speed();
	
	/*
	Checking if the position on the gameworld is a valid movement. If the player has
	the jump boost powerup and their move is invalid, they will smack into the object
	that makes the move invalid and stop short (i.e., they only move one or none
	spaces if the two space move is invalid).
	*/
	if (player.can_move(xMove, yMove) 
      && board[player.get_x() + xMove][player.get_y() + yMove] != SHALL_NOT_PASS)
	{
		player.update_location(xMove, yMove);
	}
	else
	{
		/*
		Integer division is used here to ensure a NORMAL_SPEED still results in a
		move of 0 if they can't move.
		*/
		player.update_location(xMove / FAST_SPEED, yMove / FAST_SPEED);
	}
}

/*
Checks the monster move, if it is valid, make the move and tell the moveGoose
function that the move was valid and was carried out.
*/
bool checkMonsterMove(int board[NUM_BOARD_X][NUM_BOARD_Y], Actor & goose,
	int move_x, int move_y)
{
	if (goose.can_move(move_x, move_y)
      && board[goose.get_x() + move_x][goose.get_y() + move_y]!= SHALL_NOT_PASS
	  && board[goose.get_x() + move_x][goose.get_y() + move_y]!= WINNER)
		{
			goose.update_location(move_x, move_y);
			
			return true;
		}
}


/*
The goose will track the player in a more randomised way than simply always
taking the shortest route, since a real goose doesn't have perfect tracking
senses. Different probabilities are established for if the goose moves on the
direct axis and for if the goose moves towards or away on that axis.
*/
void moveGoose(Actor const & player, Actor & goose,
	int board[NUM_BOARD_X][NUM_BOARD_Y])
{
	/*
	Percent prabability that the goose will move on the proper axis and if the
	goose will move towards or away from the player.
	*/
    int const PROBABILITY_MOVE_DIRECTLY = 95, PROBABILITY_MOVE_AWAY = 5,
		POS_NEG_DIFF = 2;
    
    //Find axis of player.
    bool greaterXDiff = abs(player.get_x() - goose.get_x())
					>= abs(player.get_y() - goose.get_y());
	
	//Find direction on each axis.
	bool lowerX = player.get_x() < goose.get_x(),
         lowerY = player.get_y() < goose.get_y();
    
    //Calculate direction based on percent probability
    bool moveDirect = rand() % 100 < PROBABILITY_MOVE_DIRECTLY;
    bool moveAway = rand() % 100 < PROBABILITY_MOVE_AWAY;
    
    //The x or y movement is increased according to the logic calculated above.
    int move_x = greaterXDiff * moveDirect || !greaterXDiff * !moveDirect;
    move_x = move_x * (move_x - POS_NEG_DIFF * (lowerX != moveAway));
    
	int move_y = !greaterXDiff * moveDirect || greaterXDiff * !moveDirect;
	move_y = move_y * (move_y - POS_NEG_DIFF * (lowerY != moveAway));
    
    //Change the distance moved based on the goose's speed. 0 Speed when stunned.
    move_x *= goose.get_speed();
    move_y *= goose.get_speed();
    
    /*
	Will try to move according to the probability, then on a different axis, then
	backwards on each axis. If all fails, the goose is stuck.
	*/
	if (checkMonsterMove(board, goose, move_x, move_y))
	{}
	else if (checkMonsterMove(board, goose, move_y, move_x))
	{}
	else if (checkMonsterMove(board, goose, -move_x, -move_y))
	{}
	else if (checkMonsterMove(board, goose, -move_y, -move_x))
	{}
}

/*
Provide textual feedback on current game events.
This would have been idealy coded directly in the apply_effect function in the Actor
class. However, the out.writeLine() function was not working there.
*/
void effectMessage(string whoDoneIt, Actor const & being)
{
	if (being.get_countdown() == 5 && being.get_speed() == FAST_SPEED)
	{
		out.writeLine(whoDoneIt + " activated a jump powerup for 5 moves!");
	}
	else if (being.get_countdown() == 5 && being.get_speed() == STUNNED)
	{
		out.writeLine(whoDoneIt + " got stunned for 5 moves!");
	}
	else if (being.get_boosted())
	{
		out.writeLine(whoDoneIt + " was launched by a jump booster!");
	}
}

/*
Apply the appropriate boost direction depending on which direciton the booster arrow
is pointing on the board
*/
void jumpBoost(Actor & character, int world[NUM_BOARD_X][NUM_BOARD_Y])
{
    const int JUMP_DISTANCE = 5;
	
	//Logic to set the movement with appropriate direction and distance.
	int xMove = (world[character.get_x()][character.get_y()] == RIGHT) 
                 * JUMP_DISTANCE
                 - (world[character.get_x()][character.get_y()] == LEFT)
                 * JUMP_DISTANCE;

    int yMove = (world[character.get_x()][character.get_y()] == DOWN) 
                 * JUMP_DISTANCE
                 - (world[character.get_x()][character.get_y()] == UP)
                 * JUMP_DISTANCE;

    bool hasMoved = false;
    
    //Repeats if there was a decision for movememnt, and until the decision acted on.
    while (xMove + yMove != 0)
    {
        // Jump the character if they can
        if (character.can_move(xMove, yMove) 
             && world[character.get_x() + xMove][character.get_y() + yMove] 
           != SHALL_NOT_PASS)
        {
        	/*
			Use up the booster, move the character, check if another jump needs to
        	be made (if the actor lands on another booster) and then carry on.
        	*/
			world[character.get_x()][character.get_y()] = 0;
			character.update_location(xMove, yMove);
            jumpBoost(character, world);
            
            character.set_boosted();
            
            //Break
            xMove = 0;
            yMove = 0;
        }
        /*
		If the jump boost lands on an invalid location, smack the actor into
		whateer caused said location to be invalid. E.g., launch the actor into the
		closest side of the boundary or wall that blocks the landing position.
		*/
        else if (!character.can_move(xMove, yMove)
			|| world[character.get_x() + xMove][character.get_y() + yMove]
			== SHALL_NOT_PASS)
        {
        	//Will decrease the launch distance until a valid landing spot is found.
			if (xMove)
            {
                xMove -= (world[character.get_x()][character.get_y()] == RIGHT)
                	- (world[character.get_x()][character.get_y()] == LEFT);
            }
            else
            {
                yMove -= (world[character.get_x()][character.get_y()] == DOWN)
					- (world[character.get_x()][character.get_y()] == UP);
            }
        }
    }
}

// If the goose has the same coordinates as the player, the player is captured.
bool captured(Actor const & player, Actor const & goose)
{
    return (player.get_x() == goose.get_x() && player.get_y() == goose.get_y());
}

// If the coordinates of the player contains an object, they stepped on it.
bool steppedOn(int world[NUM_BOARD_X][NUM_BOARD_Y], Actor const & being,
    int character)
{
    return world[being.get_x()][being.get_y()] == character;
}

// Check if the game needs to end.
bool gameOver(int world[NUM_BOARD_X][NUM_BOARD_Y], Actor const & player,
	Actor const & goose)
{
	return captured(player, goose)
			|| steppedOn(world, player, WINNER)
			|| steppedOn(world, player, KABOOM)
			|| steppedOn(world, goose, KABOOM);
}

/*
Output the reason the game was won or lost.
The player is a chicken if they quit early.
*/
string endMessage(Actor const & player, Actor const & goose,
	int gameWorld[NUM_BOARD_X][NUM_BOARD_Y])
{
	if (steppedOn(gameWorld, player, WINNER))
        {
            return "You got to a safe zone and won!";
        }
        else if (steppedOn(gameWorld, player, KABOOM))
        {
            return "You stepped on a mine and got blown to kingdom come: You lose!";
        }
        else if (steppedOn(gameWorld, goose, KABOOM))
        {
            return "The goose stepped on a mine and was incapacitated: You won!";
        }
        else if (captured(player, goose))
        {
            return "The goose was too powerful and caught you!";
        }
        else
        {
        	return "You may not be a goose, but you are a CHICKEN!";
		}
}

////// End of gooseEscapeGamePlay.cpp //////
