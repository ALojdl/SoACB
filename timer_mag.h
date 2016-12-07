#ifndef _TIMER_MAG_H
#define _TIMER_MAG_H


double getMiliTimeDiff(struct timeval t_start, struct timeval t_end);

struct timeval addTime(struct timeval t_time, int mili_time);

#endif
