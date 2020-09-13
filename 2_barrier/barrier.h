#include <pthread.h>

#include "cs_thread.h"

struct barrier_t
{
	int count;				 //no of threads reached
	int limit;				 //no of threads required before barrier opens
	struct lock lock_t;		 //mutex var
	struct condition cond_t; //condition var
};

void barrier_init(struct barrier_t *b, int i);
void barrier_wait(struct barrier_t *b);