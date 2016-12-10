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
* \file parser.h
* \brief
* Opis Modula : Zaglavlje modula koji parsira konfiguracionu datoteku
* Kreirano : Decembar 2016
*
* @Authors Andrej Lojdl, Nives Kaprocki
* \notes
*
*****************************************************************************/


#ifndef _PARSER_H
#define _PARSER_H

<<<<<<< HEAD
#define MAX_CHARACTERS 64       /* Maximal characters in buffer */
#define NUM_STREAMS 3           /* Maximal number of streams (fixed, periodic, poason) */
#define NUM_PACKETS 32          /* Maximal number of packets in a stream that can be listed inside the config file */
#define DATA_SIZE 9             /* Number od bits that can be sent to the bus - 1 */
=======
#define MAX_CHARACTERS 64
#define NUM_STREAMS 3
#define NUM_PACKETS 32
#define DATA_SIZE 9
#define DEBUG_PARSER
>>>>>>> a07728a07cd17ff199a671a790426c1da7d7a1ec

/**
 * @brief Mapping from enum values to string 
 */
static char mapping[9][MAX_CHARACTERS] = {
	"500K", "250K", "125K", "100K", 
	"95K2", "83K3", "50K", "LEFT", "RIGHT"
};

/**
 * @brief Enumeration of available CAN alignments
 */
typedef enum alignment {
    LEFT,
    RIGHT
} alignment_t;

/**
 * @brief Enumeration of available CAN baudrates
 */
typedef enum baudrate {
    _500K,
    _250K,
    _125K,
    _100K,
    _95K2,
    _83K3,
    _50K
} baudrate_t;

/**
 * @brief Enumeration of available stream modes
 */
typedef enum modes {
    FIXED = 1,
    PERIODIC,
    POASON
} modes_t;

/**
 * @brief Structure that defines a configuration file
 */
typedef struct conf_struct {
    FILE *conf;
    char tmp[MAX_CHARACTERS];
    char data[MAX_CHARACTERS];
    char *pch;
} conf_struct_t;

/**
 * @brief Structure that defines a packet with data needed for sending it to the bus
 */
typedef struct packets {
    int time;
    char PID[DATA_SIZE];
    char data[DATA_SIZE];
} packets_t;

/**
 * @brief Structure that defines a stream of packets
 */
typedef struct stream {
    modes_t mode;
    packets_t packets[NUM_PACKETS];
} stream_t;

/**
 * @brief Structure that defines can configuration and streams
 */
typedef struct init_data_can {
    baudrate_t baudrate;
    alignment_t alignment;
    stream_t streams[NUM_STREAMS];
} init_data_can_t;

/**
 * @brief Structure that defines LIN configuration and streams
 */
typedef struct init_data_lin {
    int speed;
    char magic[8];
    stream_t streams[NUM_STREAMS];
} init_data_lin_t;

/**
 * @brief Structure that defines LIN configuration and streams
 */
typedef struct init_data {
    init_data_can_t can;
    init_data_lin_t lin;
    int time;
} init_data_t;

/**
 * @brief  fills the structure with data from the configuration file
 *
 * @param  [in]   const char *path - path to the configuration file
 *				  init_data_t *init - structure which will contain all the data from the config file
 * @param  [out]  
 * @return 
 */
void get_config(const char *path, init_data_t *init);

#endif
