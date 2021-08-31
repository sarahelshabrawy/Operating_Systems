#include <pthread.h>
#include <stdio.h>

struct station {
	unsigned int waiting;
    unsigned int boardingPassengers;
    int count;
    pthread_cond_t waitingPassenger;
	pthread_cond_t leaveStation;
  	pthread_mutex_t lock;
	
};

void station_init(struct station *station);

void station_load_train(struct station *station, int count);

void station_wait_for_train(struct station *station);

void station_on_board(struct station *station);