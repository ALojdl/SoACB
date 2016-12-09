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

volatile globalCounter = 0;

void rotaryDeal()
{
	Last_RoB_Status = digitalRead(RoBPin);

	while(!digitalRead(RoAPin)){
		Current_RoB_Status = digitalRead(RoBPin);
		flag = 1;
	}

	if(flag == 1){
		flag = 0;
		if((Last_RoB_Status == 0)&&(Current_RoB_Status == 1)){
			globalCounter++;
			printf("%d", globalCounter);
		}
		if((Last_RoB_Status == 1)&&(Current_RoB_Status == 0)){
			globalCounter--;
			printf("%d", globalCounter);
		}

	}
}

void rotaryClear()
{
	if(digitalRead(RoSPin) == 0)
	{
		globalCounter = 0;
		delay(300);
	}
}

int get()
{
	return globalCounter;
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

	printf("\nEND\n");
	return NULL;
}
