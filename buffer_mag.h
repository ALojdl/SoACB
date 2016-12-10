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
* \file buffer_mag.h
* \brief
* Opis Modula : Zaglavlje modula koji rukuje dinamickim FIFO baferom
* Kreirano : Decembar 2016
*
* @Authors Andrej Lojdl, Nives Kaprocki
* \notes
*
*****************************************************************************/

#ifndef _BUFFER_MAG_H
#define _BUFFER_MAG_H

/**
 * @brief Enumeration of possible bus types
 */
typedef enum bus_type {
    CAN,
    LIN
} bus_type_t;

/**
 * @brief Structure that defines a node in the dynamic buffer
 */
typedef struct node{
	packets_t data;
	bus_type_t type;
	struct node *next;
} node_t;

/**
 * @brief Structure that defines the dynamic buffer
 */
typedef struct buffer_mag{
	int size;
	struct node *head;
} buffer_mag_t;


/**
 * @brief  initialize the dynamic buffer
 *
 * @param  [in]   buffer_mag_t buffer - will be used to store packets
 * @param  [out]  
 * @return 
 */
void buffer_init(buffer_mag_t *buffer);

/**
 * @brief  add a new packet to the dynamic buffer
 *
 * @param  [in]   buffer_mag_t buffer - used to store packets
 *				  bus type_t type - specifies whether a packet will be sent to LIN or CAN bus
 *				  packets_t packet - an instance of the packet that needs to be sent
 * @param  [out]  
 * @return 
 */
void buffer_add(buffer_mag_t *buffer, bus_type_t type, packets_t packet);

/**
 * @brief  remove the last packet from the dynamic buffer
 *
 * @param  [in]   buffer_mag_t Buffer - used to store packets
 * @param  [out]  
 * @return 
 */
void buffer_remove(buffer_mag_t *buffer);

/**
 * @brief  remove the last packet from the dynamic buffer
 *
 * @param  [in]   buffer_mag_t Buffer - used to store packets
 * @param  [out]  
 * @return 
 */
void buffer_deinit(buffer_mag_t *buffer);

#endif

