/**********************************************************************
* Filename    : rotaryEncoder.c
* Description : make a rotaryEncoder work.
* Author      : Robot
* E-mail      : support@sunfounder.com
* website     : www.sunfounder.com
* Date        : 2014/08/27
**********************************************************************/
#include <stdio.h>
#include "rotaryEncoder.h"

void rotaryDeal(int *counter)
{
	Last_RoB_Status = digitalRead(RoBPin);

	while(!digitalRead(RoAPin)){
		Current_RoB_Status = digitalRead(RoBPin);
		flag = 1;
	}

	if(flag == 1){
		flag = 0;
		if((Last_RoB_Status == 0)&&(Current_RoB_Status == 1)){
			*counter = *counter + 1;
		}
		if((Last_RoB_Status == 1)&&(Current_RoB_Status == 0)){
			*counter = *counter - 1;
		}

	}
}

void rotaryClear(int *counter)
{
	if(digitalRead(RoSPin) == 0)
	{
		*counter = 0;
		delay(300);
	}
}

void *rotary(void *param)
{
	if(wiringPiSetup() < 0){
		fprintf(stderr, "Unable to setup wiringPi:%s\n",strerror(errno));
		return NULL;
	}
	
	int *counter = (int *) param;
	pinMode(RoAPin, INPUT);
	pinMode(RoBPin, INPUT);
	pinMode(RoSPin, INPUT);

	pullUpDnControl(RoSPin, PUD_UP);

	while(1){
		rotaryDeal(counter);
		rotaryClear(counter);
	}

	return NULL;
}
