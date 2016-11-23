/*
 * memory_AT45DB161.h
 *
 *  Created on: 14/11/2016
 *      Author: joaob
 */
//=============================================================
// memory_AT45DB161.h - MEMORY_AT45DB161.h FUNCTIONS.  2016-11-14
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

#ifndef SOURCES_MEMORY_AT45DB161_H_
#define SOURCES_MEMORY_AT45DB161_H_

// General:
	#define buffer1 1
	#define buffer2 2
// Macros:

	#define mem_cs_0 mCS_PutVal(mCS_DeviceData, FALSE);			// Chip_Select_on (PTD4);
	#define mem_cs_1 mCS_PutVal(mCS_DeviceData, TRUE);			// Chip_Select_off (PTD4);
	#define mem_wp_0 mWP_PutVal(mWP_DeviceData, FALSE);			// Write_Protector_on (PTA4);
	#define mem_wp_1 mWP_PutVal(mWP_DeviceData, TRUE);			// Write_Protector_off (PTA4);
	#define mem_rs_0 mReset_PutVal(mReset_DeviceData, FALSE);	// Reset_on (PTE30);
	#define mem_rs_1 mReset_PutVal(mReset_DeviceData, TRUE);	// Reset_off (PTE30);
// Command List:
	// Read Commands:
		// Continuous Array Read (Low Frequency - fmax = 50MHz):
		#define continuous_array_read 0x03
		// Main Memory Page Read:
		#define main_mem_page_read 0xD2
		// Buffer 1 Read (Low Frequency - fmax = 50MHz):
		#define b1_read 0xD1
		// Buffer 2 Read (Low Frequency - fmax = 50MHz):
		#define b2_read 0xD3
	// Program and Erase Commands:
		// Buffer 1 Write:
		#define b1_write 0x84
		// Buffer 2 Write:
		#define b2_write 0x87
		// Buffer 1 to Main Memory Page Program with Built-In Erase:
		#define b1_to_mem_pag_prog_with_erase 0x83
		// Buffer 2 to Main Memory Page Program with Built-In Erase:
		#define b2_to_mem_pag_prog_with_erase 0x86
		// Buffer 1 to Main Memory Page Program without Built-In Erase:
		#define b1_to_mem_pag_prog_without_erase 0x88
		// Buffer 2 to Main Memory Page Program without Built-In Erase:
		#define b2_to_mem_pag_prog_without_erase 0x89
		// Main Memory Page Program through Buffer 1 with Built-In Erase:
		#define mem_pag_prog_through_b1_with_erase 0x82
		// Main Memory Page Program through Buffer 2 with Built-In Erase:
		#define mem_pag_prog_through_b2_with_erase 0x85
		// Main Memory Byte/Page Program through Buffer 1 without Built-In Erase:
		#define mem_byte_prog_through_b1_without_erase 0x02
		// Page Erase:
		#define pag_erase 0x81
	// Protection and Security Commands:
		// Enable Sector Protection:
		#define enable_protector 0xA9
		// Disable Sector Protection:
		#define disable_protector 0x9A
	// Additional Commands:
		// Main Memory Page to Buffer 1 Trasfer:
		#define mem_pag_to_b1_transfer 0x53
		// Main Memory Page to Buffer 2 Trasfer:
		#define mem_pag_to_b2_transfer 0x55
		// Main Memory Page to Buffer 1 Compare:
		#define mem_pag_to_b1_compare 0x60
		// Main Memory Page to Buffer 2 Compare:
		#define mem_pag_to_b2_compare 0x61
		// Status Register Read:
		#define status_register 0xD7
		// Manufacturer and Device ID Read:
		#define manufacturer_and_device_id 0x9F
		//Configure "Power of 2" (Binary) Page Size:
		#define config_pag_size_512 0xA6
		//Configure Standard DataFlash Page Size:
		#define config_pag_size_528 0xA7

//-----------------------------------------------------------------------------------//
//							Configuration and Information							 //
//-----------------------------------------------------------------------------------//
	// Memory Configuration (Binary Page Size - 512 Bytes):
	void mem_init(void);
	// Status Register Read (returns until 2 bytes):
	void mem_status_register(uint8_t *data, uint8_t dataSize);
	// Manufacturer and Device ID Read (returns until 5 bytes):
	void mem_information(uint8_t *data, uint8_t dataSize);
//-----------------------------------------------------------------------------------//
//										Write 										 //
//-----------------------------------------------------------------------------------//
	// Buffer Write:
	void mem_buffer_write(uint8_t buffer, uint16_t byteAddress, uint8_t *data, uint16_t dataSize);
	// Buffer to Main Memory Page Program with Built-In Erase:
	void mem_buffer_to_memory_page(uint8_t buffer, uint16_t pageAddress);
	// Main Memory Page Program through Buffer with Built-In Erase:
	void mem_page_write(uint8_t buffer, uint16_t pageAddress, uint16_t byteAddress, uint8_t *data, uint16_t dataSize);
	// Main Memory Byte/Page Program through Buffer 1 without Built-In Erase:
	void mem_byte_write(uint16_t pageAddress, uint16_t byteAddress, uint8_t *data, uint16_t dataSize);
//-----------------------------------------------------------------------------------//
//										Read 										 //
//-----------------------------------------------------------------------------------//
	// Buffer Read (Low Frequency - fmax = 50MHz):
	void mem_buffer_read(uint8_t buffer, uint16_t byteAddress, uint8_t *data, uint16_t dataSize);
	// Main Memory Page to Buffer Transfer:
	void mem_page_to_buffer(uint8_t buffer, uint16_t pageAddress);
	// Continuous Array Read (Low Frequency - fmax = 50MHz):
	void mem_array_read(uint16_t pageAddress, uint16_t byteAddress, uint8_t *data, uint16_t dataSize);
	// Main Memory Page Read:
	void mem_page_read(uint16_t pageAddress, uint16_t byteAddress, uint8_t *data, uint16_t dataSize);
//-----------------------------------------------------------------------------------//
//										Erase 										 //
//-----------------------------------------------------------------------------------//
	// Chip Erase:
	void mem_chip_erase(void);
	// Page Erase:
	void mem_page_erase(uint16_t pageAddress);
//-----------------------------------------------------------------------------------//
//								  Additional Funtions								 //
//-----------------------------------------------------------------------------------//
	// Main Memory Page to Buffer Compare (Returns the result of the compare):
	uint8_t mem_buffer_compare(uint8_t buffer, uint16_t pageAddress);
//-----------------------------------------------------------------------------------//
//								  Protection Funtions								 //
//-----------------------------------------------------------------------------------//
	// Enable Sector Protection (by software):
	void mem_enable_software_protection(void);
	// Diseable Sector Protection (by software):
	void mem_disable_software_protection(void);
//-----------------------------------------------------------------------------------//

#endif /* SOURCES_MEMORY_AT45DB161_H_ */
