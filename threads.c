#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

int counter = 0;
pthread_mutex_t mutex;

void *mythread(void *arg)
{
	pthread_mutex_lock(&mutex);
	counter++;
	pthread_mutex_unlock(&mutex);
	return NULL;
}

int main(int argc, char const *argv[])
{
	if (argc != 2)
	{
		printf("invalid arguments...\n");
		return 1;
	}

	long n = strtol(argv[1], NULL, 10);
	pthread_t *threads = (pthread_t *)malloc(n * sizeof(pthread_t));
	pthread_mutex_init(&mutex, NULL);

	for (long i = 0; i < n; i++)
		pthread_create(&threads[i], NULL, mythread, NULL);

	for (long i = 0; i < n; i++)
		pthread_join(threads[i], NULL);

	pthread_mutex_destroy(&mutex);
	free(threads);

	printf("%d\n", counter);
	sleep(10000);
	return 0;
}