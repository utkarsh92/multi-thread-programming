#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "barrier.h"

void barrier_init(struct barrier_t *barrier, int nthreads)
{
	barrier->count = 0;
	barrier->limit = nthreads;
	lock_init(&barrier->lock_t);
	cond_init(&barrier->cond_t);
	return;
}

void barrier_wait(struct barrier_t *barrier)
{
	lock_acquire(&barrier->lock_t);

	barrier->count++;

	if (barrier->count == barrier->limit)
	{
		//Nth thread enters and opens barrier
		barrier->count = 0;
		cond_broadcast(&barrier->cond_t, &barrier->lock_t);
	}
	else
	{
		//N-1 threads on wait
		// while (barrier->count != barrier->limit)
		while (cond_wait(&barrier->cond_t, &barrier->lock_t) != 0);
	}

	lock_release(&barrier->lock_t);

	return;
}
