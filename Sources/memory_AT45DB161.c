/*
 * memory_AT45DB161.c
 *
 *  Created on: 14/11/2016
 *      Author: joaob
 */
//=============================================================
// memory_AT45DB161.c - MEMORY_AT45DB161.c FUNCTIONS.  2016-11-14
//=============================================================
/**************************************************************************************
	SPI Serial Flash Memory Library (AT45DB161):
		Memory routines through of SPI communication;
		Informations:
			- Buffer and Page Size Configuration to "Binary Page Size (512 Bytes)"!
			- Memory Array: 4096 pages with 512/528 bytes by page;
			                  __________________
			                 |					|
			              --1|SI			  SO|8--
			              --2|SCK			 GND|7--
			              --3|RESET			 VCC|6--
			              --4|CS			  WP|5--
			                 |__________________|
			                 	  AT45DB161
			                 	  	  	  	  credits for José Vinícius Melo(15/08);
**************************************************************************************/

#include "Cpu.h"
#include "Events.h"
#include "Init_Config.h"
#include "PDD_Includes.h"

#include "config.h"
#include "SPI_encapsulation.h"
#include "memory_AT45DB161.h"

//==============================================================
// CODE:
//--------------------------------------------------------------

//-----------------------------------------------------------------------------------//
// Waits when device is busy:
void wait_busy(void);
// Returns 4 bytes (Command + 3 Address Bytes) for byte address in a page:
void cmd_and_byte(uint8_t command, uint16_t byteAddress, uint8_t *byte);
// Returns 4 bytes (Command + 3 Address Bytes) for page address:
void cmd_and_page(uint8_t command, uint16_t pageAddress, uint8_t *byte);
// Returns 4 bytes (Command + 3 Address Bytes) for byte and page address:
void cmd_and_address(uint8_t command, uint16_t pageAddress, uint16_t byteAddress, uint8_t *byte);
//-----------------------------------------------------------------------------------//
//                                   Internal Routines                               //
//-----------------------------------------------------------------------------------//
void wait_busy(void){
	uint8_t dynamicData=0;
	mem_cs_0;
	spi_transmit(status_register);
	while((dynamicData & 0x80) == 0) spi_receive(dynamicData);
	mem_cs_1;
}
//-----------------------------------------------------------------------------------//
void cmd_and_byte(uint8_t command, uint16_t byteAddress, uint8_t *byte){
	// Command and Address:
	byte[0] = command;
	byte[1] = 0x00;
	byte[2] = (byteAddress >> 8);
	byte[3] = byteAddress;
}
//-----------------------------------------------------------------------------------//
void cmd_and_page(uint8_t command, uint16_t pageAddress, uint8_t *byte){
	// Command and Address:
	byte[0] = command;
	byte[1] = (pageAddress >> 7);
	byte[2] = (pageAddress << 1);
	byte[3] = 0x00;
}
//-----------------------------------------------------------------------------------//
void cmd_and_address(uint8_t command, uint16_t pageAddress, uint16_t byteAddress, uint8_t *byte){
	// Command and Address:
	byte[0] = command;
	byte[1] = (pageAddress >> 7);
	byte[2] = (pageAddress << 1) | (byteAddress >> 8);
	byte[3] = byteAddress;
}
//-----------------------------------------------------------------------------------//
//                                   External Routines                               //
//-----------------------------------------------------------------------------------//
void mem_init(void){
	// Setting pins:
	mem_wp_1;
	mem_rs_0;
	delay_ms(10);
	mem_rs_1;
	// Buffer and page size configuration ("Power of 2" binary page size - 512bytes):
	mem_cs_0;
		spi_transmit(0x3D);
		spi_transmit(0x2A);
		spi_transmit(0x80);
		spi_transmit(config_pag_size_512);
	mem_cs_1;
	wait_busy();
}
//-----------------------------------------------------------------------------------//
void mem_status_register(uint8_t *data, uint8_t dataSize){
	/*	Status Register Read.
		Returns 2 Bytes Status Register:
			Byte 1:
				bit7 	- 	Ready/Busy Status;
				bit6 	- 	Compare Result;
				bit5:2 	- 	Density Code;
				bit1 	- 	Sector Protection Status;
				bit0 	- 	Page Size Configuration;
			Byte 2:
				bit7 	- 	Ready/Busy Status;
				bit6 	- 	Reserved;
				bit5 	- 	Erase/Program Error;
				bit4 	- 	Reserved;
				bit3 	- 	Sector Lockdown Enabled;
				bit2 	- 	Program Suspend Status (Buffer 2);
				bit1 	- 	Program Suspend Status (Buffer 1);
				bit0 	- 	Erase Suspend;
	*/
	mem_cs_0;
		spi_transmit(status_register);
		spi_receive_multiples(&data[0], dataSize);
	mem_cs_1;
}
//-----------------------------------------------------------------------------------//
void mem_information(uint8_t *data, uint8_t dataSize){
	/*	Manufacturer and Device ID Read.
		Returns until 5 bytes:
			1 - Manufacturer ID;
			2 - Device ID (Byte 1);
			3 - Device ID (Byte 2);
			4 - Extended Device Information (EDI) String Length;
			5 - EDI Byte 1;
	*/
	mem_cs_0;
		spi_transmit(manufacturer_and_device_id);
		spi_receive_multiples(&data[0], dataSize);
	mem_cs_1;
}
//-----------------------------------------------------------------------------------//
void mem_buffer_write(uint8_t buffer, uint16_t byteAddress, uint8_t *data, uint16_t dataSize){
	uint8_t command, byte[4], byteSize = 4;
	// Command and Address:
	if(buffer == buffer1) command = b1_write;	// Write in Buffer 1;
	else command = b2_write;					// Write in Buffer 2;
	cmd_and_byte(command, byteAddress, &byte[0]);
	// Transmit:
	mem_cs_0;
		spi_transmit_multiples(&byte[0], byteSize);	// CMD + Address Bytes;
		spi_transmit_multiples(&data[0], dataSize);	// Data;
	mem_cs_1;
}
//-----------------------------------------------------------------------------------//
void mem_buffer_to_memory_page(uint8_t buffer, uint16_t pageAddress){
	uint8_t command, byte[4], byteSize = 4;
	// Command and Address:
	if(buffer == buffer1) command = b1_to_mem_pag_prog_with_erase;
	else command = b2_to_mem_pag_prog_with_erase;
	cmd_and_page(command, pageAddress, &byte[0]);
	// Transmit:
	mem_cs_0;
		spi_transmit_multiples(&byte[0], byteSize); // CMD + Address Bytes;
	mem_cs_1;
	wait_busy();
}
//-----------------------------------------------------------------------------------//
void mem_page_write(uint8_t buffer, uint16_t pageAddress, uint16_t byteAddress, uint8_t *data, uint16_t dataSize){
	/*	This command combines the Buffer Write and Buffer to Main
		Memory Page Program with Build-in Erase.
		The contents of the appropriate buffer are programmed into
		the main memory page.
	*/
	uint8_t command, byte[4], byteSize = 4;
	// Command and Address:
	if(buffer == buffer1) command = mem_pag_prog_through_b1_with_erase;
	else command = mem_pag_prog_through_b2_with_erase;
	cmd_and_address(command, pageAddress, byteAddress, &byte[0]);
	// Transmit and Receive:
	mem_cs_0;
		spi_transmit_multiples(&byte[0], byteSize); // CMD + Address Bytes;
		spi_transmit_multiples(&data[0], dataSize); // Data;
	mem_cs_1;
	wait_busy();
}
//-----------------------------------------------------------------------------------//
void mem_byte_write(uint16_t pageAddress, uint16_t byteAddress, uint8_t *data, uint16_t dataSize){
	/*	This command allow any number of bytes (1 to 512) to be
		programmed in the main memory.
		When the end of the buffer is reached, then the device will wrap
		around back to the beginning of the buffer.
	*/
	uint8_t byte[4], byteSize = 4;
	// Command and Address:
	cmd_and_address(mem_byte_prog_through_b1_without_erase, pageAddress, byteAddress, &byte[0]);
	// Transmit and Receive:
	mem_cs_0;
		spi_transmit_multiples(&byte[0], byteSize); // CMD + Address Bytes;
		spi_transmit_multiples(&data[0], dataSize); // Data;
	mem_cs_1;
	wait_busy();
}
//-----------------------------------------------------------------------------------//
void mem_buffer_read(uint8_t buffer, uint16_t byteAddress, uint8_t *data, uint16_t dataSize){
	uint8_t command, byte[4], byteSize = 4;
	// Command and Address:
	if(buffer == buffer1) command = b1_read;	// Read in Buffer 1;
	else command = b2_read;						// Read in Buffer 2;
	cmd_and_byte(command, byteAddress, &byte[0]);
	// Transmit:
	mem_cs_0;
		spi_transmit_multiples(&byte[0], byteSize); // CMD + Address Bytes;
		spi_receive_multiples(&data[0], dataSize);	// Data;
	mem_cs_1;
}
//-----------------------------------------------------------------------------------//
void mem_page_to_buffer(uint8_t buffer, uint16_t pageAddress){
	uint8_t command, byte[4], byteSize = 4;
	// Command and Address:
	if(buffer == buffer1) command = mem_pag_to_b1_transfer;
	else command = mem_pag_to_b2_transfer;
	cmd_and_page(command, pageAddress, &byte[0]);
	// Transmit:
	mem_cs_0;
		spi_transmit_multiples(&byte[0], byteSize); // CMD + Address Bytes;
	mem_cs_1;
	wait_busy();
}
//------------------------------------------------------------------------------------
void mem_array_read(uint16_t pageAddress, uint16_t byteAddress, uint8_t *data, uint16_t dataSize){
	/*	When the end of a page in the memory is reached, the device will continue
		reading at the beginning of the NEXT page.
	*/
	uint8_t byte[4], byteSize = 4;
	// Command and Address:
	cmd_and_address(continuous_array_read, pageAddress, byteAddress, &byte[0]);
	// Transmit and Receive:
	mem_cs_0;
		spi_transmit_multiples(&byte[0], byteSize); // CMD + Address Bytes;
		spi_receive_multiples(&data[0], dataSize);	// Data;
	mem_cs_1;
}
//------------------------------------------------------------------------------------
void mem_page_read(uint16_t pageAddress, uint16_t byteAddress, uint8_t *data, uint16_t dataSize){
	/*	When the end of a page in the memory is reached, the device will continue
		reading back at the beginning of the SAME page.
	*/
	uint8_t byte[4], byteSize = 4;
	// Command and Address:
	cmd_and_address(main_mem_page_read, pageAddress, byteAddress, &byte[0]);
	// Transmit and Receive:
	mem_cs_0;
		spi_transmit_multiples(&byte[0], byteSize); // CMD + Address Bytes;
		spi_transmit_multiples(&byte[0], byteSize); // 4 Dummy Bytes;
		spi_receive_multiples(&data[0], dataSize);	// Data;
	mem_cs_1;
}
//-----------------------------------------------------------------------------------//
void mem_chip_erase(void){
	mem_cs_0;
		spi_transmit(0xC7);
		spi_transmit(0x94);
		spi_transmit(0x80);
		spi_transmit(0x9A);
	mem_cs_1;
	wait_busy();
}
//-----------------------------------------------------------------------------------//
void mem_page_erase(uint16_t pageAddress){
	uint8_t byte[4], byteSize = 4;
	// Command and Address:
	cmd_and_page(pag_erase, pageAddress, &byte[0]);	// CMD + Address Bytes;
	mem_cs_0;
		spi_transmit_multiples(&byte[0], byteSize);
	mem_cs_1;
	wait_busy();
}
//-----------------------------------------------------------------------------------//
uint8_t mem_buffer_compare(uint8_t buffer, uint16_t pageAddress){
	/*	After the compare operation, bit6 of the Status Register
		will be updated with the result of the compare.
		Compare Result (bit6):
						0 - Main memory page data matches buffer data;
						1 - Main memory page data does not match buffer data;
	*/
	uint8_t command, byte[4], byteSize = 4;
	uint8_t statusRegister, compareResult;
	// Command and Address:
	if(buffer == buffer1) command = mem_pag_to_b1_compare;
	else command = mem_pag_to_b2_compare;
	cmd_and_page(command, pageAddress, &byte[0]);
	// Transmit:
	mem_cs_0;
		spi_transmit_multiples(&byte[0], byteSize); // CMD + Address Bytes;
	mem_cs_1;
	wait_busy();
	// Verification (returns result of the bit6):
	mem_status_register(&statusRegister, 1);
	if (statusRegister & 0x40) compareResult = 1;	//
	else compareResult = 0;
	return compareResult;
}
//-----------------------------------------------------------------------------------//
void mem_enable_software_protection(void){
	/*	If the device is power cycled, then the software controlled
		protection will be diseable.
	*/
	mem_cs_0;
		spi_transmit(0x3D);
		spi_transmit(0x2A);
		spi_transmit(0x7F);
		spi_transmit(enable_protector);
	mem_cs_1;
}
//-----------------------------------------------------------------------------------//
void mem_disable_software_protection(void){
	mem_cs_0;
		spi_transmit(0x3D);
		spi_transmit(0x2A);
		spi_transmit(0x7F);
		spi_transmit(disable_protector);
	mem_cs_1;
}
//-----------------------------------------------------------------------------------//





