#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define MAX_RESOURCES 5
int available_resources = MAX_RESOURCES;
pthread_mutex_t mtx;

int decrease_count (int count){
	pthread_mutex_lock(&mtx);
	if (available_resources < count){
		pthread_mutex_unlock(&mtx);
		return -1;
	}
	else{
		available_resources -= count;
		printf("Got %d resources %d remaining\n", count, available_resources);
	}
	pthread_mutex_unlock(&mtx);	
	return 0;
}

int increase_count (int count) {
	pthread_mutex_lock(&mtx);
	available_resources += count;
	printf("Released %d resourcres %d remaining\n", count, available_resources);
	pthread_mutex_unlock(&mtx);	
	return 0;
}

void *solve(void *arg){
	int count = (int)arg;
	///asteptam pana avem suficiente resurse disponibile
	while (decrease_count(count)==-1);
	increase_count(count);
}

int main() {
	///initializem mutexul
	if (pthread_mutex_init(&mtx, NULL)){
		perror(NULL);
		return errno;
	}
	srand(time(0));
	
	pthread_t *thd = malloc(MAX_RESOURCES*sizeof(pthread_t));
	printf("Number of available resources: %d\n",MAX_RESOURCES);
	int count[6];	
	for (int i=0; i<MAX_RESOURCES; ++i){
		count[i]=rand()%(MAX_RESOURCES+1);
		if (pthread_create (&thd[i], NULL, solve, count[i])){
			perror(NULL);
			return errno;
		}
	}
	for (int i=0; i<MAX_RESOURCES; ++i){
		pthread_join(thd[i], NULL);
	}
	free(thd);
	pthread_mutex_destroy(&mtx);
	return 0;
}
