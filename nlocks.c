#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

int data[10];
pthread_mutex_t mutex[10];

void *mythread(void *arg)
{
	int x = (*(int *)arg);

	for (int i = 0; i < 1000; i++)
	{
		pthread_mutex_lock(&mutex[x]);
		data[x]++;
		pthread_mutex_unlock(&mutex[x]);
	}
	return NULL;
}

int main(int argc, char const *argv[])
{
	pthread_t *threads = (pthread_t *)malloc(10 * sizeof(pthread_t));
	int *index = (int *)malloc(10 * sizeof(int));

	for (int i = 0; i < 10; i++)
		pthread_mutex_init(&mutex[i], NULL);

	for (int i = 0; i < 10; i++)
	{
		index[i] = i;
		pthread_create(&threads[i], NULL, mythread, &index[i]);
	}

	for (int i = 0; i < 1000; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			pthread_mutex_lock(&mutex[j]);
			data[j]++;
			pthread_mutex_unlock(&mutex[j]);
		}
	}

	for (int i = 0; i < 10; i++)
		pthread_join(threads[i], NULL);

	free(threads);
	free(index);

	for (int i = 0; i < 10; i++)
	{
		printf("%d\n", data[i]);
	}

	// sleep(10000);
	return 0;
}