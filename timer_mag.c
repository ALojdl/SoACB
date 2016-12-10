/****************************************************************************
*
* Univerzitet u Novom Sadu, Fakultet tehnickih nauka
* Katedra za Racunarsku Tehniku i Racunarske Komunikacije
*
* -----------------------------------------------------
* Ispitni projekat iz predmeta:
*
* RACUNARSKE MREZE, MAGISTRALE I PROTOKOLI U AUTOMOBILU
* -----------------------------------------------------
* Naslov zadatka: Emulacija sistema komunikacije automobilskih  magistrala
* -----------------------------------------------------*
* \file timer_mag.c
* \brief
* Opis Modula : Realizacija funkcija za rukovanje vremenskim strukturama
* Kreirano : Decembar 2016
*
* @Author Andrej Lojdl, Nives Kaprocki
* \notes
*
*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <limits.h>
#include "timer_mag.h"

double getMiliTimeDiff(struct timeval t_start, struct timeval t_end)
{
	double dif = 0;
	double dif2 = 0;
	dif = (t_end.tv_sec - t_start.tv_sec) * 1000;

	if (t_end.tv_usec < t_start.tv_usec) {
		dif-=1000;
		t_end.tv_usec += 1000000;

	}
	dif2 = (t_end.tv_usec - t_start.tv_usec)/1000.0;
	dif += dif2;
	return dif;
}

struct timeval addTime(struct timeval t_time, int mili_time)
{
	int delta = 0;
	struct timeval sum;
	
	delta = mili_time/1000;
	sum.tv_sec = t_time.tv_sec + delta;
	mili_time = mili_time%1000;

	if (mili_time*1000 + t_time.tv_usec > 999999) {
		sum.tv_sec++;
		sum.tv_usec = t_time.tv_usec - 1000000;
		
	}
	else {

		sum.tv_usec = t_time.tv_usec;
	}
	sum.tv_usec += mili_time*1000;
	return sum;
}

int poason(double lambda) {
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
