#ifndef _BUFFER_MAG_H
#define _BUFFER_MAG_H

#define BUFFER_CAPACITY 100

typedef enum bus_type {
    CAN,
    LIN
} bus_type_t;

typedef struct node{
	packets_t data;
	bus_type_t type;
	struct node *next;
} node_t;

typedef struct buffer_mag{
	int size;
	struct node *head;
} buffer_mag_t;


void buffer_init(buffer_mag_t *buffer);

void buffer_add(buffer_mag_t *buffer, bus_type_t type, packets_t packet);

void buffer_remove(buffer_mag_t *buffer);

void buffer_deinit(buffer_mag_t *buffer);

#endif

