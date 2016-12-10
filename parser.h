#ifndef _PARSER_H
#define _PARSER_H

#define MAX_CHARACTERS 64
#define NUM_STREAMS 3
#define NUM_PACKETS 32
#define DATA_SIZE 9
#define DEBUG_PARSER

static char mapping[9][MAX_CHARACTERS] = {
	"500K", "250K", "125K", "100K", 
	"95K2", "83K3", "50K", "LEFT", "RIGHT"
};

typedef enum alignment {
    LEFT,
    RIGHT
} alignment_t;

typedef enum baudrate {
    _500K,
    _250K,
    _125K,
    _100K,
    _95K2,
    _83K3,
    _50K
} baudrate_t;

typedef enum modes {
    FIXED = 1,
    PERIODIC,
    POASON
} modes_t;

typedef struct conf_struct {
    FILE *conf;
    char tmp[MAX_CHARACTERS];
    char data[MAX_CHARACTERS];
    char *pch;
} conf_struct_t;

typedef struct packets {
    int time;
    char PID[DATA_SIZE];
    char data[DATA_SIZE];
} packets_t;

typedef struct stream {
    modes_t mode;
    packets_t packets[NUM_PACKETS];
} stream_t;

typedef struct init_data_can {
    baudrate_t baudrate;
    alignment_t alignment;
    stream_t streams[NUM_STREAMS];
} init_data_can_t;

typedef struct init_data_lin {
    int speed;
    char magic[8];
    stream_t streams[NUM_STREAMS];
} init_data_lin_t;

typedef struct init_data {
    init_data_can_t can;
    init_data_lin_t lin;
    int time;
} init_data_t;

void get_config(const char *path, init_data_t *init);

#endif
