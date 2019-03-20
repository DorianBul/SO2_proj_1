#ifndef PHILOSOPHER_H
#define PHILOSOPHER_H

#include <deque>
#include <vector>
#include <atomic>
#include <thread>
#include <time.h>
#include <unistd.h>

#include <Generator.h>		// <random>

/*
 * leaving this as defined will force the philosophers to eat only
 * to avoid test mode just comment the define below and rebuild :)
 */
//#define QUICKTESTMODE

using namespace std;

extern vector<thread> 			vecOfThreads;
extern atomic_flag 				killThemAll;
extern atomic_flag 				newPhilosopherRequested;

extern vector<bool> 			tableSlotTaken;
static atomic_flag 				lock_chair_picking = ATOMIC_FLAG_INIT;

extern deque<atomic<bool>> 		tableForkAvailable;
extern deque<atomic<int>> 		tableForkOwner;
extern deque<atomic<int>> 		tableForkReservation;

extern deque<atomic<int>> 		hungerChart;
extern deque<atomic<int>> 		taskTimersChart;
extern uint 					phil_amount;

#define HUNGER_THRESHOLD 20
#define STARVING_THRESHOLD -60

enum class State
{
	Thinking = 0x0,
	Watching_stars,
	Sleeping,
	Writing_thesis,
	Waiting_for_fork,
	Waiting_for_second_fork,
	Eating,
	D_E_A_D
};

extern deque<atomic<State>> stateTable;

class Philosopher
{
public:
	Philosopher();
	~Philosopher();

	void init(const uint slots);

	void run();

	bool eat();

	void waitForTimer(int time);

	uint getId() { return id; }
	void getState() { stateTable[id] = phil_state; }

private:
	uint id;
	State phil_state;

	bool left_fork_acquired;
	bool right_fork_acquired;

	Generator gen;
};

#endif // PHILOSOPHER_H
