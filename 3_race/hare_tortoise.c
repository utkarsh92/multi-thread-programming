#include "cs_thread.h"

struct Repositioning
{
	char player;  // T for turtle and H for hare
	int time;	  // At what time god interrupt's
	int distance; // How much does god move any of the player.
				  // distance can be negetive or positive.
				  // using this distance if any of players position is less than zero then bring him to start line.
				  // If more than finish_distance then make him win.
				  // If time is after completion of game than you can ignore it as we will already have winner.
};

struct race
{

	//	Don't change these variables.
	//	speeds are unit distance per unit time.
	int printing_delay;
	int turtle_speed;					// speed of Turtle
	int hare_speed;						// speed of hare
	int hare_sleep_time;				// how much time does hare sleep (in case he decides to sleep)
	int hare_turtle_distance_for_sleep; // minimum lead which hare has on turtle after which hare decides to move
										// Any lead greater than this distance and hare will ignore turtle and go to sleep
	int finish_distance;				// Distance between start and finish line
	struct Repositioning *reposition;	// pointer to array containing Randomizer's decision
	int repositioning_count;			// number of elements in array of repositioning structure

	//	Add your custom variables here.
	char winner;
	int race_on;
	int clk;
	int turn;
	int hare_position, turtle_position;
	struct lock lock0;
	struct condition cond0, cond1, cond2, cond3;
};

void *Turtle(void *race);
void *Hare(void *race);
void *Randomizer(void *race);
void *Report(void *race);
void begin_turn(struct race *race, int turn);
void end_turn(struct race *race, int turn);

char init(struct race *race)
{
	pthread_t *threads = (pthread_t *)malloc(4 * sizeof(pthread_t));

	lock_init(&race->lock0);
	cond_init(&race->cond0);
	cond_init(&race->cond1);
	cond_init(&race->cond2);
	cond_init(&race->cond3);

	race->clk = 0;
	race->turn = 0;
	race->hare_position = 0;
	race->turtle_position = 0;
	race->race_on = 1;

	//test parameters
	// race->printing_delay = 1;
	// race->hare_sleep_time = 2;
	// race->hare_turtle_distance_for_sleep = 200;
	// race->finish_distance = 1000;

	lock_acquire(&race->lock0);
	pthread_create(&threads[0], NULL, Randomizer, (void *)race);
	pthread_create(&threads[1], NULL, Hare, (void *)race);
	pthread_create(&threads[2], NULL, Turtle, (void *)race);
	pthread_create(&threads[3], NULL, Report, (void *)race);
	lock_release(&race->lock0);

	for (int i = 0; i < 4; i++)
		pthread_join(threads[i], NULL);

	return race->winner;
}

void *Randomizer(void *arg)
{
	int turn = 0;
	int index = 0;
	struct race *race = (struct race *)arg;

	while (race->race_on)
	{
		begin_turn(race, turn);

		if (race->race_on && (race->repositioning_count != 0) && (index <= race->repositioning_count))
		{
			if (race->reposition[index].time == race->clk)
			{
				//God will move someone
				if (race->reposition[index].player == 'T')
				{
					race->turtle_position += race->reposition[index].distance;
					if (race->turtle_position < 0)
						race->turtle_position = 0;

					printf("**GOD moved Turtle to %d at time %d**\n", race->turtle_position, race->clk);
				}
				else
				{
					race->hare_position += race->reposition[index].distance;
					if (race->hare_position < 0)
						race->hare_position = 0;

					printf("**GOD moved Hare to %d at time %d**\n", race->hare_position, race->clk);
				}

				index++;
			}
		}

		// sleep(1);
		end_turn(race, turn);
	}
	return NULL;
}

void *Hare(void *arg)
{
	int turn = 1;
	int sleep_count = 0;
	struct race *race = (struct race *)arg;

	while (race->race_on)
	{
		begin_turn(race, turn);

		if (race->race_on)
		{
			if (sleep_count <= 0)
			{
				if ((race->hare_position - race->turtle_position) <= race->hare_turtle_distance_for_sleep)
				{
					// printf("Hare PANICS and sleep...");
					race->hare_position += race->hare_speed;
					sleep_count = race->hare_sleep_time;
				}
				else
				{
					// printf("Hare goes to sleep again...");
					sleep_count = race->hare_sleep_time;
				}
			}
			else
			{
				// printf("Hare sleeps...");
				sleep_count--;
			}
		}

		// sleep(1);
		end_turn(race, turn);
	}
	return NULL;
}

void *Turtle(void *arg)
{
	int turn = 2;
	struct race *race = (struct race *)arg;

	while (race->race_on)
	{
		begin_turn(race, turn);

		if (race->race_on)
		{
			// printf("Turtle moves...\n");
			race->turtle_position += race->turtle_speed;
		}

		// sleep(1);
		end_turn(race, turn);
	}

	return NULL;
}

void *Report(void *arg)
{
	int turn = 3;
	struct race *race = (struct race *)arg;

	while (race->race_on)
	{
		begin_turn(race, turn);

		if (race->clk % race->printing_delay == 0)
			printf("Report (End of time %d) ==> Hare: %d, Turtle: %d\n", race->clk, race->hare_position, race->turtle_position);

		if ((race->hare_position >= race->finish_distance) && (race->turtle_position < race->finish_distance))
		{
			race->winner = 'H';
			printf("======HARE WINS======\n");
			race->race_on = 0;
		}
		else if ((race->turtle_position >= race->finish_distance))
		{
			race->winner = 'T';
			printf("======TURTLE WINS======\n");
			race->race_on = 0;
		}

		// sleep(1);
		end_turn(race, turn);
	}
	return NULL;
}

void begin_turn(struct race *race, int turn)
{
	switch (turn)
	{
	case 0:
		lock_acquire(&race->lock0);
		while (race->turn != turn)
			cond_wait(&race->cond0, &race->lock0);

		// printf("Current Time: %d\n", race->clk);
		break;
	case 1:
		lock_acquire(&race->lock0);
		while (race->turn != turn)
			cond_wait(&race->cond1, &race->lock0);
		break;
	case 2:
		lock_acquire(&race->lock0);
		while (race->turn != turn)
			cond_wait(&race->cond2, &race->lock0);
		break;
	case 3:
		lock_acquire(&race->lock0);
		while (race->turn != turn)
			cond_wait(&race->cond3, &race->lock0);
		break;
	}
}

void end_turn(struct race *race, int turn)
{
	race->turn = (race->turn + 1) % 4;

	switch (turn)
	{
	case 0:
		cond_signal(&race->cond1, &race->lock0);
		lock_release(&race->lock0);
		break;
	case 1:
		cond_signal(&race->cond2, &race->lock0);
		lock_release(&race->lock0);
		break;
	case 2:
		cond_signal(&race->cond3, &race->lock0);
		lock_release(&race->lock0);
		break;
	case 3:
		race->clk++;
		cond_signal(&race->cond0, &race->lock0);
		lock_release(&race->lock0);
		break;
	}
}
