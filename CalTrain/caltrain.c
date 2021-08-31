#include <pthread.h>
#include "caltrain.h"



void
station_init(struct station *station)
{
	station->waiting = 0;
	station->count = 0;
	station->boardingPassengers = 0;
	pthread_mutex_init( &station->lock, NULL);
	pthread_cond_init( &station->leaveStation, NULL);
	pthread_cond_init( &station->waitingPassenger, NULL);
}

void
station_load_train(struct station *station, int count)
{
    pthread_mutex_lock(&station->lock);
	if(count == 0 || station->waiting == 0){
        pthread_mutex_unlock(&station->lock);
        return;
	}
    station->count = count;
    //there are empty seats or waiting passengers
    pthread_cond_broadcast(&station->waitingPassenger);
    pthread_cond_wait(&station->leaveStation,&station->lock);
    pthread_mutex_unlock(&station->lock);
}

void
station_wait_for_train(struct station *station)
{
    pthread_mutex_lock(&station->lock);
    station->waiting ++;
    //train is filled
    while(station->count == 0)
         pthread_cond_wait(&station->waitingPassenger,&station->lock);
    station->boardingPassengers ++ ;
    station->waiting --;
    station->count--;
    pthread_mutex_unlock(&station->lock);
}

void
station_on_board(struct station *station)
{
    pthread_mutex_lock(&station->lock);
    station->boardingPassengers--;
    //either no empty seats or no waiting passenger and all passengers boarded
    if((station->count == 0 || station->waiting == 0) && station->boardingPassengers == 0){
        pthread_cond_signal(&station->leaveStation);
    }
    pthread_mutex_unlock(&station->lock);
}
