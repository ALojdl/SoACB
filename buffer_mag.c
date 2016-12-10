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
* \file buffer_mag.c
* \brief
* Opis Modula : Realizacija funkcija za rukovanje dinamickim FIFO baferom
* Kreirano : Decembar 2016
*
* @Author Andrej Lojdl, Nives Kaprocki
* \notes
*
*****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include "parser.h"
#include "buffer_mag.h"

void buffer_init(buffer_mag_t *buffer) {

	buffer->size = 0;
	buffer->head = NULL;

}

void buffer_deinit(buffer_mag_t *buffer)	{
	struct node *next;
	struct node *current = buffer->head;
	while (current != NULL) {
			next = current->next;
			free(current);
			buffer->size--;
			current = next;

	}
	buffer->head = NULL;
	buffer->size--;
}

void buffer_add(buffer_mag_t *buffer, bus_type_t type, packets_t packet) {

	struct node *current = buffer->head;
	struct node *temp = (struct node*)malloc(sizeof(struct node));
	temp->data = packet;
	temp->type = type;
	temp->next = NULL;

	if (buffer->size == 0) {
		buffer->head = temp;

	} else {

		while (current->next != NULL) {
			current = current->next;

		}

		current->next = temp;

	}
	buffer->size++;

}

void buffer_remove(buffer_mag_t *buffer) {

	struct node *current = buffer->head;

	buffer->head = buffer->head->next;
	free(current);
	buffer->size--;
}

