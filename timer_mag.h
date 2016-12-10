/****************************************************************************
*
* Univerzitet u Novom Sadu, Fakultet tehnickih nauka
* Katedra za Računarsku Tehniku i Računarske Komunikacije
*
* -----------------------------------------------------
* Ispitni projekat iz predmeta:
*
* RACUNARSKE MREZE, MAGISTRALE I PROTOKOLI U AUTOMOBILU
* -----------------------------------------------------
* Naslov zadatka: Emulacija sistema komunikacije automobilskih magistrala
* -----------------------------------------------------
*
* \file timer_mag.h
* \brief
* Opis Modula : Zaglavlje modula koje rukuje vremenskim strukturama 
* Kreirano : Decembar 2016
*
* @Authors Andrej Lojdl, Nives Kaprocki
* \notes
*
*****************************************************************************/

#ifndef _TIMER_MAG_H
#define _TIMER_MAG_H

#define SEC_TO_MILI 1000    /* factor to convert seconds to ms */
#define MILI_TO_MICRO 1000  /* factor to convert ms to us */

/**
 * @brief  subtracts two timeval structures to calculate the difference in ms
 *
 * @param  [in]   struct timeval t_start - starting point in time
 *				  struct timeval t_end - ending point in time	
 * @param  [out]  
 * @return        difference between to timeval structures in ms
 */
double getMiliTimeDiff(struct timeval t_start, struct timeval t_end);

/**
 * @brief  adds a timeval structures and time in ms
 *
 * @param  [in]   struct timeval t_time - a point in time
 *				  int mili_time - a period in ms	
 * @param  [out]  
 * @return  	  an updated timeval structure
 */
struct timeval addTime(struct timeval t_time, int mili_time);

/**
 * @brief  calculates next wait interval with poisson distribution
 *
 * @param  [in]   double lambda - event rate	
 * @param  [out]  
 * @return 		  time until the next poason packet can be sent	
 */
int poason(double lambda);

/**
 * @brief  returns time of periodic packet which is next to be sent to the bus
 *
 * @param  [in]   int* p_time - list of points in time for packers presented in the same order as in the config file
 * 				  int* num - index of the packet with the smallest time to be sent 
  * 			  int count - index of the packet with the smallest time to be sent 
 * @param  [out]  
 * @return 		  value of the minimal time
 */
int min_time(int* p_time, int* num, int count);


#endif
