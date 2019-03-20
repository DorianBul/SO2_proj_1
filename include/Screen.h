#ifndef SCREEN_H
#define SCREEN_H

#include <ncurses.h>
#include <Philosopher.h>
#include <string>
#include <iostream>

#define WIDTH 79

extern uint phil_amount;

#define HEIGHT phil_amount*2+1

class Screen
{
public:
	Screen();
	~Screen();

	void init_and_run();

	char* printState(int index)
	{
		static constexpr char* const philosopherStates[]
		{
			"Thinking",
			"Watching_stars",
			"Sleeping",
			"Writing_thesis",
			"Waiting_for_fork",
			"Waiting_for_second_fork",
			"Eating",
			"Dead"
		};
	    return philosopherStates[index];
	  }

private:
	WINDOW *ncWindow;
	int ch;

};

#endif // SCREEN_H
