#ifndef _TIMER_MAG_H
#define _TIMER_MAG_H

#define SEC_TO_MILI 1000
#define MILI_TO_MICRO 1000

double getMiliTimeDiff(struct timeval t_start, struct timeval t_end);

struct timeval addTime(struct timeval t_time, int mili_time);

int poason(double lambda);

#endif
