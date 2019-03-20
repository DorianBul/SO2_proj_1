//==============================================================================\\
//																				\\
//								 _________________________						\\
//								/ _____________________  /|						\\
//							   / / ___________________/ / |						\\
//							  / / /| |               / /  |						\\
//							 / / / | |              / / . |						\\
//							/ / /| | |             / / /| |						\\
//						   / / / | | |            / / / | |						\\
//						  / / /  | | |           / / /| | |						\\
//						 / /_/___| | |__________/ / / | | |						\\
//						/________| | |___________/ /  | | |						\\
//						| _______| | |__________ | |  | | |						\\
//						| | |    | | |_________| | |__| | |						\\
//						| | |    | |___________| | |____| |						\\
//						| | |   / / ___________| | |_  / /						\\
//						| | |  / / /           | | |/ / /						\\
//						| | | / / /            | | | / /						\\
//						| | |/ / /             | | |/ /							\\
//						| | | / /              | | ' /							\\
//						| | |/_/_______________| |  /							\\
//						| |____________________| | /							\\
//						|________________________|/								\\
//																				\\
//																				\\
//==============================================================================\\
								Autor: nieznany

#include <iostream>
#include <string.h>

#include <Philosopher.h>	// <atomic> + <vector> + <thread> + <time.h> + <unistd.h>
#include <Screen.h>			// <ncurses.h>

using namespace std;

vector<thread> 				vecOfThreads;
vector<bool> 				tableSlotTaken;
deque<atomic<bool>> 		tableForkAvailable;
deque<atomic<int>> 			tableForkOwner;
deque<atomic<int>> 			tableForkReservation;
deque<atomic<int>> 			hungerChart;
deque<atomic<int>> 			taskTimersChart;
deque<atomic<State>> 		stateTable;
atomic_flag 				killThemAll = ATOMIC_FLAG_INIT;	// reversed flag
atomic_flag 				newPhilosopherRequested = ATOMIC_FLAG_INIT;
uint 						phil_amount = 5;

void terminate_philosopher_threads()
{
	killThemAll.clear();
	for (auto it = taskTimersChart.begin(); it != taskTimersChart.end(); it++) (*it)=0;	// speed up terminating other threads
}

void philosopher_thread()
{
	Philosopher phil;
	phil.init(phil_amount);
	phil.run();
	cout << "Philosopher " << phil.getId() << " task ended." << endl;
	return;
}

void ncurses_thread()
{
	Screen screen;
	screen.init_and_run();
	terminate_philosopher_threads();
	cout << "NCthread ended." << endl;
	return;
}

void hungerAndTimers_thread()
{
	uint hungerTimer = 0;
	for (;;)
	{
		if (!killThemAll.test_and_set())
		{
			killThemAll.clear();
			cout << "Timers task ended." << endl;
			return;
		}

		if (++hungerTimer > 5)
		{
			hungerTimer = 0;
			for (auto it = hungerChart.begin(); it != hungerChart.end(); it++) if ((*it)>(STARVING_THRESHOLD)) (*it)--;
		}

		int i = 0;
		for (auto it = taskTimersChart.begin(); it != taskTimersChart.end(); it++)
		{
			if ((hungerChart[i++].load() < HUNGER_THRESHOLD) && ((*it) > 30)) (*it).store(30);
			else if ((*it)>0) (*it)--;
		}

		if (!newPhilosopherRequested.test_and_set())
		{
			newPhilosopherRequested.clear();
		}
		else
		{
			tableSlotTaken.push_back(false);
			tableForkAvailable.emplace_back(true);
			tableForkOwner.emplace_back(-1);
			tableForkReservation.emplace_back(-1);
			hungerChart.emplace_back(234952);
			taskTimersChart.emplace_back(0);
			phil_amount++;
			vecOfThreads.push_back(thread(philosopher_thread));
			tableForkReservation[0].store(-1);
			tableForkAvailable[0].store(true);
			newPhilosopherRequested.clear();
		}

		usleep(100000); // 100 ms
	}

}

int main()
{
	killThemAll.test_and_set();	// true to keep them alive

	cout << "Choose number of philosophers: " << endl;
	cin >> phil_amount;


	for (uint i = 0; i < phil_amount ; ++i)
	{
		tableSlotTaken.push_back(false);
		tableForkAvailable.emplace_back(true);
		tableForkOwner.emplace_back(-1);
		tableForkReservation.emplace_back(-1);
		hungerChart.emplace_back(234952);
		taskTimersChart.emplace_back(0);
		vecOfThreads.push_back(thread(philosopher_thread));
	}

	thread ncwindow(ncurses_thread);
	thread hungerCounter(hungerAndTimers_thread);

	ncwindow.join();
	hungerCounter.join();

	for (thread& th : vecOfThreads) if (th.joinable())	th.join();

	cout << "MAIN FUNCTION END" << endl;

	return 0;
}
