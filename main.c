#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include "parser.h"
#include "buffer_mag.h"
#include "timer_mag.h"

/*buffer with packets that have to be sent */
buffer_mag_t buffer_prior;
/* data parsed from config.conf */
init_data_t init;

pthread_mutex_t mutex;
pthread_t pt;

int can_fixed_id = -1;
int can_periodic_id = -1;
int can_poason_id = -1;
int lin_fixed_id = -1;
int lin_periodic_id = -1;
int lin_poason_id = -1;

/*number of periodic packets and next time when periodic packets have to be sent*/
int can_num_period = 0;
int can_period_time[NUM_PACKETS];
int lin_num_period = 0;
int lin_period_time[NUM_PACKETS];

/*threads which fill buffer with fixed and periodic messages */
void *can_fixed(void *param); 
void *can_periodic(void *param); 
void *lin_fixed(void *param); 
void *lin_periodic(void *param); 


void initializeData() {

   pthread_mutex_init(&mutex, NULL);

}

/* set necessary values and create threads for existing CAN packets' types*/
void prepare_can() {

	int i;

	for (i = 0; i<3; i++) {
		if (init.can.streams[i].mode == FIXED)
			can_fixed_id = i;
		else if (init.can.streams[i].mode == PERIODIC)
			can_periodic_id = i;
		else
			can_poason_id = i;
	} 

	/* Create threads and set number of periodic packets and period_time array */
	if (can_fixed_id != -1) {
		pthread_create(&pt,NULL,can_fixed,NULL);
		//printf("Can fixed id:%d\n",can_fixed_id);
	}
	
	if (can_periodic_id != -1) {
		while (init.can.streams[can_periodic_id].packets[can_num_period].time !=0) {
			can_period_time[can_num_period] = init.can.streams[can_periodic_id].packets[can_num_period].time;
			can_num_period++;
		}
		pthread_create(&pt,NULL,can_periodic,NULL);
		//printf("Can periodic id:%d\n",can_periodic_id);
	}

}

/* set necessary values and create threads for existing LIN packets' types*/
void prepare_lin() {

	int i; 

	for (i = 0; i<3; i++) {
		if (init.lin.streams[i].mode == FIXED)
			lin_fixed_id = i;
		else if (init.lin.streams[i].mode == PERIODIC)
			lin_periodic_id = i;
		else
			lin_poason_id = i;
	} 

	/* Create threads and set number of periodic packets and period_time array */
	if (lin_fixed_id != -1) {
		pthread_create(&pt,NULL,lin_fixed,NULL);
		//printf("Lin fixed id:%d\n",lin_fixed_id);
	}
	
	if (lin_periodic_id != -1) {
		while (init.lin.streams[lin_periodic_id].packets[lin_num_period].time !=0) {
			lin_period_time[lin_num_period] = init.lin.streams[lin_periodic_id].packets[lin_num_period].time;
			lin_num_period++;
		}
		pthread_create(&pt,NULL,lin_periodic,NULL);
		//printf("Lin periodic id:%d\n",lin_periodic_id);
	}

}

/* Returns time of periodic packet which is next to be sent to the bus*/
int min_time(int* p_time, int* num, int count) {
	int i;
	int min = p_time[0];
	*num = 0;
	for (i=1; i <count;i++) {

		if (p_time[i] < min) {
			min = p_time[i];
			*num = i;
		}
	}
	return min;
}

/* Thread for fixed can packets */
void *can_fixed(void *param) {
  int i = 0;
	int start_time = 0;
	int end_time = init.can.streams[can_fixed_id].packets[0].time;

	while (buffer_prior.size != -1) {

		usleep((end_time-start_time)*1000);
		start_time = end_time;

		pthread_mutex_lock(&mutex);
    buffer_add(&buffer_prior, CAN, init.can.streams[can_fixed_id].packets[i]); 
   	pthread_mutex_unlock(&mutex);

		i++;
		/*This next statement can be changed to something more precise */
		end_time = init.can.streams[can_fixed_id].packets[i].time;
		if(end_time == 0)
			break;
	}
	printf("No more fixed packets!\n");
}

/* Thread for periodic can packets */
void *can_periodic(void *param) {
	int i =0;
	int end_time = min_time(can_period_time, &i, can_num_period);
	int start_time = 0;
	
	while (buffer_prior.size != -1) {
		usleep((end_time - start_time)*1000);

		pthread_mutex_lock(&mutex);
    buffer_add(&buffer_prior, CAN, init.can.streams[can_periodic_id].packets[i]); 
   	pthread_mutex_unlock(&mutex);

		can_period_time[i] += init.can.streams[can_periodic_id].packets[i].time;
		start_time = end_time;
		end_time = min_time(can_period_time, &i, can_num_period);
	}
	printf("Ending periodic thread\n");
}

/* Thread for fixed lin packets */
void *lin_fixed(void *param) {
  int i = 0;
	int start_time = 0;
	int end_time = init.lin.streams[lin_fixed_id].packets[0].time;

	while (buffer_prior.size != -1) {

		usleep((end_time-start_time)*1000);
		start_time = end_time;

		pthread_mutex_lock(&mutex);
    buffer_add(&buffer_prior, LIN, init.lin.streams[lin_fixed_id].packets[i]); 
   	pthread_mutex_unlock(&mutex);

		i++;
		/*This next statement can be changed to something more precise */
		end_time = init.lin.streams[lin_fixed_id].packets[i].time;
		if(end_time == 0)
			break;
	}
	printf("No more fixed packets!\n");
}

/* Thread for periodic lin packets */
void *lin_periodic(void *param) {
	int i =0;
	int end_time = min_time(lin_period_time, &i, lin_num_period);
	int start_time = 0;
	
	while (buffer_prior.size != -1) {
		usleep((end_time - start_time)*1000);

		pthread_mutex_lock(&mutex);
    buffer_add(&buffer_prior, LIN, init.lin.streams[lin_periodic_id].packets[i]); 
   	pthread_mutex_unlock(&mutex);

		lin_period_time[i] += init.lin.streams[lin_periodic_id].packets[i].time;
		start_time = end_time;
		end_time = min_time(lin_period_time, &i, lin_num_period);
	}
	printf("Ending periodic thread\n");
}

int main(int argc, const char* argv[])
{

  struct timeval endwait;
	struct timeval current;
	struct timeval check;
  struct timeval start;
	double dif;

	/* Get data from config file and initialize */
  get_config("config.conf", &init);
	initializeData();
	buffer_init(&buffer_prior);

	prepare_can();
	prepare_lin();
	
	printf("Data sending starts....\n");

	gettimeofday(&start,NULL);
	endwait = addTime(start,12000);
	gettimeofday(&current,NULL);

	while (getMiliTimeDiff(current, endwait) > 0) {

		if (buffer_prior.size > 0) {

			check = addTime(start,buffer_prior.head->data.time);
			if (getMiliTimeDiff(check,current) >= 0) {

				pthread_mutex_lock(&mutex);
				if (buffer_prior.head->type == CAN)	{
					printf("CAN Sent data: %d %s %s\n",buffer_prior.head->data.time,buffer_prior.head->data.PID,buffer_prior.head->data.data);
				} else {
					printf("LIN Sent data: %d %s %s\n",buffer_prior.head->data.time,buffer_prior.head->data.PID,buffer_prior.head->data.data);
				}
   			buffer_remove(&buffer_prior);
				pthread_mutex_unlock(&mutex);
			}
		}
   	
		gettimeofday(&check,NULL);
		dif = getMiliTimeDiff(current,check);
		if (dif*1000 <= 1000) {
			usleep(dif*1000);
		}
		gettimeofday(&current,NULL);
	}

	gettimeofday(&endwait,NULL);
	dif = getMiliTimeDiff(start, endwait);
	printf("TIME PASSED %lf\n",dif);

	pthread_mutex_lock(&mutex);
	buffer_deinit(&buffer_prior);
	pthread_mutex_unlock(&mutex);
  return 0;
}

/*
#include <stdio.h>

#include "parser.h"

int main() {
    init_data_t init;
    get_config("config.conf", &init);
    
    
    return 0;
}
*/
/*
------------------
  POASON NUMBERS
------------------
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <string.h>

int poisson(double lambda){
    srand(time(NULL));
    int k=0;
    double L=exp(-lambda), p=1;
    do 
    {
        ++k;
        p *= rand()/(double)INT_MAX;
    } 
    while (p > L);
    return --k;
}

int main(int argc, char**argv){
  while (argc > 1) {
    int in = atoi(argv[--argc]);
    printf("lambda=%d:  %d\n",in,poisson(in));
  }
}
*/
