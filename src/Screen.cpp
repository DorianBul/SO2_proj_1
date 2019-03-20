#include <Screen.h>

Screen::Screen()
{

}

Screen::~Screen()
{

}

void Screen::init_and_run()
{
	//	INIT
	if ((ncWindow = initscr()) == NULL)
	{
		fprintf(stderr, "Brak ncurses!\n");
		return;
	}
	start_color();
	init_pair(1, COLOR_BLUE, COLOR_BLACK);
	init_pair(2, COLOR_CYAN, COLOR_BLACK);
	noecho();
	raw();
	curs_set(false);
	keypad(stdscr, true);
	nodelay(ncWindow, TRUE);
	//	DRAW SCREEN INIT
	attron(COLOR_PAIR(1));
	for(int i=0;i<=WIDTH;++i) mvprintw(0, i, "#");
	mvprintw(1, 0, "#        Press F4 to kill all the philosophers and end the simulation");
	mvprintw(1, WIDTH, "#");
	mvprintw(2, 0, "#        Press F8 to spawn new philosopher and add him to simulation");
	mvprintw(2, WIDTH, "#");
	for(int i=0;i<=WIDTH;++i) mvprintw(3, i, "#");

	int offset = 4;
	string thresholdsInfo = "Hunger tresholds: "+to_string(HUNGER_THRESHOLD)+"+ = OK | "+to_string(STARVING_THRESHOLD)+" to "
			+to_string(HUNGER_THRESHOLD)+" = hungry | "+to_string(STARVING_THRESHOLD)+" = starving or dead";
	mvprintw(offset, 3, thresholdsInfo.c_str());
	mvprintw(offset+1, 3, "Forks displayed as cyan ones are indeed the same one fork");
#ifdef QUICKTESTMODE
	init_pair(3, COLOR_RED, COLOR_BLACK);
	attron(COLOR_PAIR(3));
	mvprintw(offset+1, 64, "EAT_ONLY_TEST");
	attron(COLOR_PAIR(1));
#endif
	refresh();

	for (uint i = 0; i <= HEIGHT; ++i)	// side walls
	{
		mvprintw(i+offset, 0, "#");
		mvprintw(i+offset, WIDTH, "#");
		refresh();
	}
	char tempStringBuffer;
	string clearline = "#                                                                              #"; // full line till #
	for(;;)
	{
		if ((ch = getch()) != ERR)
		{
			if (ch == KEY_F(4))
			{
				delwin(ncWindow);
				endwin();
				refresh();
				return;
			}
			else if(ch == KEY_F(8))
			{
				newPhilosopherRequested.test_and_set();
				continue;
			}
		}

		for (uint i = 1; i <= phil_amount; ++i)
		{
			int j = i*2+offset+1 ;
			string temp = to_string(i) + ")";
			string temp2 = to_string(hungerChart[i-1].load());
			string temp3 = to_string(taskTimersChart[i-1].load());
			tempStringBuffer = temp3.back();
			if (temp3.length() == 1)
			{
				temp3.insert(temp3.length()-1,"0.");
			}
			else
			{
				temp3.insert(temp3.length()-1,".");
			}

			mvprintw(j, 0, clearline.c_str()); // full line till #
			if (i == 1) attron(COLOR_PAIR(2));
			mvprintw(j, 8, "---E free");
			if (!tableForkAvailable[i-1].load())
			{
				string temp4 = "Owner: " + to_string(tableForkOwner[i-1].load()+1);
				mvprintw(j, 13, temp4.c_str());
			}
			string temp5 = "R: " + to_string(tableForkReservation[i-1].load()+1);
			mvprintw(j, 30, temp5.c_str());
			if (i == 1) attron(COLOR_PAIR(1));
			mvprintw(++j, 0, clearline.c_str()); // full line till #
			mvprintw(j, 3, temp.c_str());
			mvprintw(j, 6, printState((int)stateTable[i-1].load()));
			mvprintw(j, 30, "Hunger: ");
			mvprintw(j, 38, temp2.c_str());
			mvprintw(j, 58, "Timer: ");
			mvprintw(j, 66, temp3.c_str());
			mvprintw(j, 71, "sec");
		}
		int endOffset = phil_amount*2+offset+3;
		mvprintw(endOffset, 0, clearline.c_str()); // full line till #
		attron(COLOR_PAIR(2));
		mvprintw(endOffset, 8, "---E free");
		if (!tableForkAvailable[0].load())
		{
			string temp4 = "Owner: " + to_string(tableForkOwner[0].load()+1);
			mvprintw(endOffset, 13, temp4.c_str());
		}
		string temp5 = "R: " + to_string(tableForkReservation[0].load()+1);
		mvprintw(endOffset, 30, temp5.c_str());
		attron(COLOR_PAIR(1));
		mvprintw(endOffset+1, 0, "#");
		mvprintw(endOffset+1, WIDTH, "#");
		for(int i=0;i<=WIDTH;++i) mvprintw(endOffset+2, i, "#");

		refresh();
		usleep(100000); // 100ms
	}
}
