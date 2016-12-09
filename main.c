#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#include "parser.h"
#include "buffer_mag.h"
#include "timer_mag.h"
#include "telnet-client.h"
#include "rotaryEncoder.h"

static volatile int globalCounter = 0 ;
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
void *rotary(void *param);


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
#ifdef DEBUG
		printf("[%s]Can fixed id:%d\n", __file__, can_fixed_id);
#endif
	}

	if (can_periodic_id != -1) {
		while (init.can.streams[can_periodic_id].packets[can_num_period].time !=0) {
			can_period_time[can_num_period] = init.can.streams[can_periodic_id].packets[can_num_period].time;
			can_num_period++;
		}
		pthread_create(&pt,NULL,can_periodic,NULL);
#ifndef DEBUG
		printf("[%s]Can periodic id:%d\n", __FILE__, can_periodic_id);
#endif
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
#ifdef DEBUG
		printf("[%s]Lin fixed id:%d\n", __FILE__, lin_fixed_id);
#endif
	}

	if (lin_periodic_id != -1) {
		while (init.lin.streams[lin_periodic_id].packets[lin_num_period].time !=0) {
			lin_period_time[lin_num_period] = init.lin.streams[lin_periodic_id].packets[lin_num_period].time;
			lin_num_period++;
		}
		pthread_create(&pt,NULL,lin_periodic,NULL);
#ifdef DEBUG
		printf("Lin periodic id:%d\n",lin_periodic_id);
#endif
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
		/* This next statement can be changed to something more precise */
		end_time = init.can.streams[can_fixed_id].packets[i].time;
		if(end_time == 0)
			break;
	}
#ifdef DEBUG
	printf("No more fixed packets!\n");
#endif
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
#ifdef DEBUG
	printf("Ending periodic thread\n");
#endif
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
#ifdef DEBUG
	printf("No more fixed packets!\n");
#endif
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
#ifdef DEBUG
	printf("Ending periodic thread\n");
#endif
}

void separate(char *buffer, char *string)
{
		int length = strlen(string);
		if (length == 8)
			sprintf(buffer, "%c%c %c%c %c%c %c%c", string[0], string[1],
				string[2], string[3], string[4], string[5], string[6], string[7]);
		else if (length == 6)
			sprintf(buffer, "%c%c %c%c %c%c", string[0], string[1],
				string[2], string[3], string[4], string[5]);
		else if (length == 4)
			sprintf(buffer, "%c%c %c%c", string[0], string[1],
				string[2], string[3]);
		else
			sprintf(buffer,"%s", string);
			
		return;
}
int main(int argc, const char* argv[])
{
    struct timeval endwait;
	struct timeval current;
	struct timeval check;
    struct timeval start;
	double dif;
	char tmp[MAX_CHARACTERS * 2], buf[MAX_CHARACTERS * 2];
	telnet_config_t config;

	/* Get data from config file and initialize */
    get_config("config.conf", &init);
    telnet_construct(&config);
	initializeData();
	buffer_init(&buffer_prior);
	pthread_create(&pt,NULL,rotary,NULL);

	prepare_can();
	prepare_lin();

	/* Andrej testing area */
	func(&config, "AT\n");
	sprintf(tmp, "CAN USER OPEN CH2 %s\n", mapping[init.can.baudrate]);
	func(&config, tmp);
	sprintf(tmp, "CAN USER ALIGN %s\n", mapping[init.can.alignment + 7]);
	func(&config, tmp);
	func(&config, "LIN OPEN SLAVE2X\n");

	gettimeofday(&start,NULL);
	endwait = addTime(start,init.time * 1000);
	gettimeofday(&current,NULL);

	while (getMiliTimeDiff(current, endwait) > 0) {

		if (buffer_prior.size > 0) {

			check = addTime(start,buffer_prior.head->data.time);
			if (getMiliTimeDiff(check,current) >= 0) {
				pthread_mutex_lock(&mutex);
				if (buffer_prior.head->type == CAN)	{
					sprintf(tmp, "CAN USER TX CH2 %s %s\n", buffer_prior.head->data.PID, buffer_prior.head->data.data);
					func(&config, tmp);
					printf("\n%d\n", get());
				} else {
					separate(buf, buffer_prior.head->data.data);
					sprintf(tmp, "LIN SR %s %s\n", buffer_prior.head->data.PID, buf);
					func(&config, tmp);
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
#ifdef DEBUG
	printf("TIME PASSED %lf\n",dif);
#endif

	/* Andrej test area */
	func(&config, "LIN CLOSE\n");
	func(&config, "CAN USER CLOSE CH2\n");

	/* Deinitialize variables */
	telnet_deconstruct(&config);
	pthread_mutex_lock(&mutex);
	buffer_deinit(&buffer_prior);
	pthread_mutex_unlock(&mutex);
    return 0;
}
