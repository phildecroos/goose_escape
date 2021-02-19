////// Beginning of gooseEscapeActors.hpp //////

#ifndef GOOSE_ESCAPE_ACTORS
#define GOOSE_ESCAPE_ACTORS
#include <cmath>
#include <iostream>
#include <BearLibTerminal.h>
#include "gooseEscapeUtil.hpp"

/*
    Modify this class to contain more characteristics of the "actor".  Add
    functions that will be useful for playing the game that are specific to
    the Actor.
    
    Feel free to add additional Classes to your program.
*/

/* 
    Going further:  Learn the other syntax for implementing a class that is
    more appropriate for working with multiple files, and improve the class code.
*/

class Actor
{
  private:
    int actorChar;      
    int location_x, location_y;
    int speed;
    int countdown;
    bool justBoosted;

  public:
    Actor()
    {
        actorChar = int('A');
        location_x = MIN_SCREEN_X;
        location_y = MIN_SCREEN_Y;
        speed = NORMAL_SPEED;
        countdown = 0;
        justBoosted = false;
        put_actor();
    }

    Actor(char initPlayerChar, int x0, int y0)
    {
        change_char(initPlayerChar);
        location_x = MIN_SCREEN_X;
        location_y = MIN_SCREEN_Y;
        speed = NORMAL_SPEED;
        justBoosted = false;
        countdown = 0;
        
        update_location(x0,y0);
    }
    
    int get_x() const
    {
        return location_x;
    }
    
    int get_y() const
    {
        return location_y;
    }
    
    int get_speed() const
    {
    	return speed;
	}
	
	int get_countdown() const
	{
		return countdown;
	}
	
	void set_boosted()
	{
		justBoosted = true;
	}
	
	int get_boosted() const
	{
		return justBoosted;
	}
    
    string get_location_string() const
    {
        char buffer[80];
        itoa(location_x,buffer,10);
        string formatted_location = "(" + string(buffer) + ",";
        itoa(location_y,buffer,10);
        formatted_location += string(buffer) + ")";
        return formatted_location;
    }
    
    void change_char(char new_actor_char)
    {
        actorChar = min(int('~'),max(int(new_actor_char),int(' ')));
    }

    bool can_move(int delta_x, int delta_y) const
    {
        int new_x = location_x + delta_x;
        int new_y = location_y + delta_y;

        return new_x >= MIN_BOARD_X && new_x <= MAX_BOARD_X
          && new_y >= MIN_BOARD_Y && new_y <= MAX_BOARD_Y;
    }

    void update_location(int delta_x, int delta_y)
    {
        if (can_move(delta_x, delta_y))
        {
            terminal_clear_area(location_x, location_y, 1, 1);
			location_x += delta_x;
            location_y += delta_y;
            put_actor();
            
            //Reset each time the actor moves.
            justBoosted = false;
        }
    }
    
    void put_actor() const
    {
        terminal_put(location_x, location_y, actorChar);
        terminal_refresh();
    }
    
    void apply_effect(int effectSpeed, int board[NUM_BOARD_X][NUM_BOARD_Y])
    {
    	const int EFFECT_DURATION = 5;
    	
    	speed = effectSpeed;
    	countdown = EFFECT_DURATION;
    	
    	//Clears the powerup from the world memory so that it cannot be used twice.
    	board[location_x][location_y] = 0;
	}
    
	void update_effects(int board[NUM_BOARD_X][NUM_BOARD_Y])
	{
		/*
		The effects are only applied if the actor does not already have an effect on
		cooldown. If the effect is already applied, the countdown decreases.
		Otherwise, a new effect will be applied if the actor is on a mystery spot.
		*/
		if (speed != NORMAL_SPEED)
		{
			countdown --;
			
			if (countdown == 0)
			{
				speed = NORMAL_SPEED;
			}
		}
		else if (board[location_x][location_y] == MYSTERY)
		{
			//Randomly choose if the mystery is a stun or a jump
			if (rand() % 2)
			{
				apply_effect(FAST_SPEED, board);
			}
			else
			{
				apply_effect(STUNNED, board);
			}
		}
	}
};
#endif

////// End of gooseEscapeActors.hpp //////
