/*
 * packet_reception.h
 *
 *  Created on: 10/09/2016
 *      Author: joaob
 */
//=============================================================
// packet_reception.h - PACKET RECEPTION FUNCTIONS.  2016-09-10
//=============================================================

#ifndef SOURCES_PACKET_RECEPTION_H_
#define SOURCES_PACKET_RECEPTION_H_

//-------------------------------------------------------------
// PUBLIC VARIABLES IN packet_reception.c:
typedef struct {
	uint32_t TireID;
	uint16_t pressure;
	int32_t acceZ;
	int32_t acceX;
	float voltage;
	int32_t temp;
	uint16_t frameID;
} dataPayload;

//-------------------------------------------------------------
// PUBLIC FUNCTIONS IN packet_reception.c:
void decodeFrame (uint8_t *data, dataPayload *message);

//-------------------------------------------------------------

#endif /* SOURCES_PACKET_RECEPTION_H_ */
