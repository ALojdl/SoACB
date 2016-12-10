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
* \file parser.c
* \brief
* Opis Modula : Realizacija funkcija za parsiranje konfiguracione datoteke
* Kreirano : Decembar 2016
*
* @Author Andrej Lojdl, Nives Kaprocki
* \notes
*
*****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "parser.h"

void get_streams_config(conf_struct_t *file, stream_t *streams)
{
    int number;
    int j, i = 0;
    char *ptr;
    
    /* Get mode and prepare for message reading */
    fgets(file->tmp, MAX_CHARACTERS, file->conf);
#ifdef DEBUG_PARSER
	puts(file->tmp);
#endif
    
    while (file->tmp[0] == '<')
    {
        j = 0;
        file->pch = strtok(file->tmp," \n");
        sprintf(file->data, "%s", file->pch);
        
        if (strcmp(file->data, "<FIXED>") == 0)
            streams[i].mode = FIXED;
        else if (strcmp(file->data, "<PERIODIC>") == 0)
            streams[i].mode = PERIODIC;
        else if (strcmp(file->data, "<POASON>") == 0)
            streams[i].mode = POASON;
        else
            printf("%s\tERROR -> No valid stream type in setup!\n", __func__);
            
        /* Read messages until another mode is stumbled upon */
        fgets(file->tmp, MAX_CHARACTERS, file->conf);
#ifdef DEBUG_PARSER
        puts(file->tmp);
#endif
        while (strcmp(file->tmp, "<>") < 0)
        {
			/* If there is more then 32 messages ignore them */
			if (j == NUM_PACKETS - 2)
			{
				while (strcmp(file->tmp, "<>") > 0)
				{
						fgets(file->tmp, MAX_CHARACTERS, file->conf);
				}
			}
				
            /* Time or period */
            file->pch = strtok(file->tmp, " \n");
            if (file->pch == NULL)
            {
                printf("%s\tERROR -> No valid message description!\n", __func__);
                return;
            }
            number = atoi(file->pch);
            streams[i].packets[j].time = number;
            
            /* Identification section */
            file->pch = strtok(NULL, " \n");
            if (file->pch == NULL)
            {
                printf("%s\tERROR -> No valid message description!\n", __func__);
                return;
            }
            strcpy(streams[i].packets[j].PID, file->pch);
            
            /* Data section */
            file->pch = strtok(NULL, " \n");
            if (file->pch == NULL)
            {
                printf("%s\tERROR -> No valid message description!\n", __func__);
                return;
            }
            strcpy(streams[i].packets[j].data, file->pch);
            
            j++;
            fgets(file->tmp, MAX_CHARACTERS, file->conf);
#ifdef DEBUG_PARSER
			puts(file->tmp);
#endif
        }
				i++;
    }
        
    return;
}

void get_can_config(conf_struct_t *file, init_data_can_t *init_can)
{    
    /* Get baudrate for CAN bus */
    fgets(file->tmp, MAX_CHARACTERS, file->conf);
#ifdef DEBUG_PARSER
	puts(file->tmp);
#endif
    file->pch = strtok(file->tmp," ");
    sprintf(file->data, "_%s", file->pch);
    
    if (strcmp(file->data, "_500K") == 0)
        init_can->baudrate = _500K;
    else if (strcmp(file->data, "_250K") == 0)
        init_can->baudrate = _250K;
    else if (strcmp(file->data, "_125K") == 0)
        init_can->baudrate = _125K;
    else if (strcmp(file->data, "_100K") == 0)
        init_can->baudrate = _100K;
    else if (strcmp(file->data, "_95K2") == 0)
        init_can->baudrate = _95K2;
    else if (strcmp(file->data, "_83K3") == 0)
        init_can->baudrate = _83K3;
    else if (strcmp(file->data, "_50K") == 0)
        init_can->baudrate = _50K;
    else 
        printf("%s\tERROR -> No valid baudrate in CAN setup!\n", __func__);
    
    /* Get alignment for CAN bus */
    file->pch = strtok(NULL," \n");
    sprintf(file->data, "%s", file->pch);
    
    if (strcmp(file->data, "LEFT") == 0)
        init_can->alignment = LEFT;
    else if (strcmp(file->data, "RIGHT") == 0)
        init_can->alignment = RIGHT;
    else
        printf("%s\tERROR -> No valid align in CAN setup!\n", __func__);
    
    get_streams_config(file, init_can->streams);
    return;
}

void get_lin_config(conf_struct_t *file, init_data_lin_t *init_lin)
{    
    /* Get speed for LIN bus */
    fgets(file->tmp, MAX_CHARACTERS, file->conf);
#ifdef DEBUG_PARSER
	puts(file->tmp);
#endif
    file->pch = strtok(file->tmp," \n");
    
    init_lin->speed = atoi(file->pch);
    
    if (init_lin->speed > 20000 || init_lin->speed < 1000)
        printf("%s\tERROR -> No valid speed in LIN setup!\n", __func__);
        
    /* Get magic number */
    file->pch = strtok(NULL, " \n");
    strcpy(init_lin->magic, file->pch);

    get_streams_config(file, init_lin->streams);
    return;
}

void get_config(const char *path, init_data_t *init)
{
    conf_struct_t file;
    int end = 0;
    
    /* Open configuration file, return error if there is problem */
    file.conf = fopen(path, "r");
    if(file.conf == NULL)
    {
        printf("%s\tERROR -> Error openning file!\n", __func__);
        fflush(stdout);
        return;
    }
    
    /* Read lines and delegate configuration process to subfunctions */
    fgets(file.tmp, MAX_CHARACTERS, file.conf);
    while (!end) 
    {
#ifdef DEBUG_PARSER
        puts(file.tmp);
#endif
        
        file.tmp[5] = '\0';
        if (strcmp(file.tmp, "[CAN]") == 0)
            get_can_config(&file, &(init->can));
        else if (strcmp(file.tmp, "[LIN]") == 0)
            get_lin_config(&file, &(init->lin));
        else if (strcmp(file.tmp, "[END]") == 0)
        {
            fgets(file.tmp, MAX_CHARACTERS, file.conf);
            init->time = atoi(file.tmp);
            end = 1;
        }
        else
        {
            printf("%s\tERROR->Undefined configuration parameter!\n", __func__);
            end = 1;
        }
    }
    
    /* Close file and end function */
    fclose(file.conf);    
    return;
}
