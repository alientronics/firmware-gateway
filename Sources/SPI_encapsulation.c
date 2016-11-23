/*
 * SPI_encapsulation.cpp
 *
 *  Created on: 16/11/2016
 *      Author: joaob
 */

#include "Cpu.h"
#include "Events.h"
#include "Init_Config.h"
#include "PDD_Includes.h"

#include "config.h"
#include "SPI_encapsulation.h"

//==============================================================
// CODE:
//--------------------------------------------------------------

// Error code from "LDD_TError", possible codes:
// ERR_OK 		- OK
// ERR_SPEED 	- This device does not work in the active clock configuration
// ERR_DISABLED	- Component is disabled
// ERR_BUSY 	- The previous transmit request is pending

LDD_TError spi_transmit(uint8_t dataBuffer){
	LDD_TError Error; // Error Variable of the SPI

	//Set the SPI flag to zero
	DataSentFlag_SPI = FALSE;

	// Start transmission/reception
	Error = SPI1_SendBlock(DeviceData_SPI_Struc, &dataBuffer, 1);

	// Check if the SPI Error return:
	if(Error == ERR_OK){

	}
	else{
		SPI1_CancelBlockTransmission();
		return Error;
	}

}

LDD_TError spi_transmit_multiples(uint8_t *dataBuffer, uint16_t bufferSize){
	LDD_TError Error; // Error Variable of the SPI

	//Set the SPI flag to zero
	DataSentFlag_SPI = FALSE;

	// Start transmission/reception
	Error = SPI1_SendBlock(DeviceData_SPI_Struc, dataBuffer, bufferSize);

	// Check if the SPI is busy
	if(Error==ERR_BUSY){
		SPI1_CancelBlockTransmission();
		return SPI_BUSY;
	}

	return SPI_OK;
}

LDD_TError spi_receive(uint8_t dataBuffer){
	LDD_TError Error; // Error Variable of the SPI

	//Set the SPI flags to zero
	DataReceivedFlag_SPI = FALSE;
	DataSentFlag_SPI = FALSE;

	// Request data block reception
	Error = SPI1_ReceiveBlock(DeviceData_SPI_Struc, &dataBuffer, 1);

	// Check if the SPI is busy
	if(Error==ERR_BUSY){
		SPI1_CancelBlockReception();
		return SPI_BUSY;
	}

	// Start transmission/reception
	Error = SPI1_SendBlock(DeviceData_SPI_Struc, SPI_dummyData, 1);

	// Check if the SPI is busy
	if(Error==ERR_BUSY){
		SPI1_CancelBlockTransmission();
		return SPI_BUSY;
	}

	// If not busy, wait for the flag or timeout of 500us
	t1_counter = 5;
	while((!DataReceivedFlag_SPI) || (t1_counter));

	/*
	if(!t1_counter){
		SPI1_CancelBlockReception();
		SPI1_CancelBlockTransmission();
		return SPI_TIMEOUT;
	}
	*/

	return SPI_OK;
}

LDD_TError spi_receive_multiples(uint8_t *dataBuffer, uint16_t bufferSize){
	LDD_TError Error; // Error Variable of the SPI

	//Set the SPI flags to zero
	DataReceivedFlag_SPI = FALSE;
	DataSentFlag_SPI = FALSE;

	// Request data block reception
	Error = SPI1_ReceiveBlock(DeviceData_SPI_Struc, dataBuffer, bufferSize);

	// Check if the SPI is busy
	if(Error==ERR_BUSY){
		SPI1_CancelBlockReception();
		return SPI_BUSY;
	}

	// Start transmission/reception
	Error = SPI1_SendBlock(DeviceData_SPI_Struc, SPI_dummyData, bufferSize);

	// Check if the SPI is busy
	if(Error==ERR_BUSY){
		SPI1_CancelBlockTransmission();
		return SPI_BUSY;
	}

	// If not busy, wait for the flag or timeout of 500us
	//t1_counter = 5;
	//while((!DataReceivedFlag_SPI) || (t1_counter));
	while(!DataReceivedFlag_SPI);

	/*
	if(!t1_counter){
		SPI1_CancelBlockReception();
		SPI1_CancelBlockTransmission();
		return SPI_TIMEOUT;
	}
	*/

	return SPI_OK;
}

//==============================================================





