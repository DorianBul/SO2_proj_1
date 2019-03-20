#include <Philosopher.h>

Philosopher::Philosopher()
{
	left_fork_acquired = false;
	right_fork_acquired = false;
}

Philosopher::~Philosopher()
{

}

void Philosopher::init(const uint slots)
{
	for (;;)
	{
		if (!lock_chair_picking.test_and_set())
		{
			for (uint i = 0 ; i < slots; ++i) // take a seat
			{
				if (!tableSlotTaken[i])
				{
					tableSlotTaken[i] = true;
					id = i;
					phil_state = (State)gen.roll_int(0,3);
#ifdef QUICKTESTMODE
					hungerChart[id].store(gen.roll_int(HUNGER_THRESHOLD,HUNGER_THRESHOLD+10));
#else
					hungerChart[id].store(gen.roll_int(HUNGER_THRESHOLD*2,HUNGER_THRESHOLD*3));
#endif
					getState();
					lock_chair_picking.clear();
					return;
				}
			}
			lock_chair_picking.clear();
			return;	// shouldn't happen but in case of "shouldn't"
		}
		else
		{
			usleep(5000); // 5 ms
		}
	}
}

void Philosopher::run()
{
	for(;;)
	{
		if (!killThemAll.test_and_set())
		{
			killThemAll.clear();
			return;
		}
		else if (hungerChart[id] < HUNGER_THRESHOLD)	// 60 sec in therory to drop from 100 to 0
		{
			if (hungerChart[id] == STARVING_THRESHOLD)
			{
				phil_state = State::D_E_A_D;
				getState();
				taskTimersChart[id].store(0);
				return;
			}
			else if (eat())
			{
#ifdef QUICKTESTMODE
				waitForTimer(gen.roll_int(5,10)); // 0.5 - 1.0 sec
#else
				phil_state = (State)gen.roll_int(0,3);
				getState();
				waitForTimer(gen.roll_int(30,100)); // 3.0 - 10.0 sec
#endif
			}
		}
		else
		{
			phil_state = (State)gen.roll_int(0,3);
			getState();
#ifdef QUICKTESTMODE
			waitForTimer(gen.roll_int(20,30)); // 2.0 - 3.0 sec
#else
			waitForTimer(gen.roll_int(30,100)); // 3.0 - 10.0 sec
#endif
		}
		usleep(5000); // 5 ms
	}
}

bool Philosopher::eat()
{
	phil_state = State::Waiting_for_fork;
	getState();

	int i = gen.roll_int(8,13);								// retries amount | 4 up to 13 sec
	uint index_l = id%(phil_amount-1);						// lower  fork id
	uint index_r = (id+1==phil_amount) ? id : id+1;			// higher fork id

	while (--i)
	{
		index_l = id%(phil_amount-1);						// lower  fork id refresh in case we add new philosopher
		index_r = (id+1==phil_amount) ? id : id+1;			// higher fork id refresh in case we add new philosopher

		if (!left_fork_acquired)
		{
			if (tableForkReservation[index_l].load() == id || tableForkReservation[index_l].load() == -1)
			{
				if (tableForkAvailable[index_l].exchange(false))
				{
					tableForkOwner[index_l].store(id);
					if (tableForkReservation[index_l].load() == id) tableForkReservation[index_l].store(-1);
					left_fork_acquired = true;
					phil_state = State::Waiting_for_second_fork;
					getState();
				}
				else if (tableForkReservation[index_l].load() == -1)
				{
					tableForkReservation[index_l].store(id);
				}
			}
			usleep(5000); // 5 ms
		}
		else
		{
			if (tableForkReservation[index_r].load() == id || tableForkReservation[index_r].load() == -1)
			{
				if (tableForkAvailable[index_r].exchange(false))
				{
					tableForkOwner[index_r].store(id);
					if (tableForkReservation[index_r].load() == id) tableForkReservation[index_r].store(-1);
					right_fork_acquired = true;
				}
				else if (tableForkReservation[index_r].load() == -1)
				{
					tableForkReservation[index_r].store(id);
				}
			}
			usleep(5000); // 5 ms
		}

		if (left_fork_acquired && right_fork_acquired)
		{
			phil_state = State::Eating;
			getState();
			waitForTimer(gen.roll_int(50,80));
#ifdef QUICKTESTMODE
			hungerChart[id].store(gen.roll_int(15,19));
#else
			hungerChart[id].store(gen.roll_int(70,90));
#endif
			tableForkAvailable[index_l].store(true);
			left_fork_acquired = false;
			tableForkAvailable[index_r].store(true);
			right_fork_acquired = false;
			usleep(10000); // 10 ms
			return true;
		}
		waitForTimer(gen.roll_int(5,10)); // 0,5s - 1,0s
	}

	if (left_fork_acquired)
	{
		if (tableForkReservation[index_l].load() == id)
		{
			tableForkReservation[index_l].store(-1);
		}
		tableForkAvailable[index_l].store(true);
		tableForkOwner[index_l].store(-1);
		left_fork_acquired = false;
	}
	if (right_fork_acquired)
	{
		if (tableForkReservation[index_r].load() == id)
		{
			tableForkReservation[index_r].store(-1);
		}
		tableForkAvailable[index_r].store(true);
		tableForkOwner[index_r].store(-1);
		right_fork_acquired = false;
	}
	return false;
}

void Philosopher::waitForTimer(int time)	// time * 0,1s
{
	taskTimersChart[id].store(time);
	for(;;)
	{
		if (taskTimersChart[id].load() < 1) return;
		else if (!killThemAll.test_and_set())
		{
			killThemAll.clear();
			return;
		}
		else usleep(20000); //20ms
	}
}


