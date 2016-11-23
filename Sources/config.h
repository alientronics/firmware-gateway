/*
 * config.h
 *
 *  Created on: 05/11/2016
 *      Author: joaob
 */
//=============================================================
// config.h - CONFIG FUNCTIONS.  2016-11-05
//=============================================================

#ifndef SOURCES_CONFIG_H_
#define SOURCES_CONFIG_H_

//-------------------------------------------------------------
// PUBLIC VARIABLES IN config.c:

// SPI1 Interrupt Variables:
extern volatile bool DataReceivedFlag_SPI;
extern volatile bool DataSentFlag_SPI;

// TIMER 1 Interrupt Variable:
extern volatile uint16_t   t1_counter;

// RFMOD DATA:
#define RFMRXBLEN  32	//24
uint8_t au8RRxD[RFMRXBLEN];
uint8_t u8RRxIdx;
uint8_t u8RssiValue;

// UART0 DATA:
#define UTXBLEN    96
uint8_t UTxBuf[UTXBLEN];
uint8_t u8UTxIdx;
uint16_t u16UTxDataLen;
uint32_t u32UTxCntData;

// ENUMERATIONS:
enum SPI_return {SPI_TIMEOUT=0, SPI_OK=1, SPI_BUSY=2};

//-------------------------------------------------------------
// PUBLIC FUNCTIONS IN config.c:
void uConfig(void);
void delay_ms(uint16_t value);
void delay_faster(uint16_t value);
//-------------------------------------------------------------

#endif /* SOURCES_CONFIG_H_ */
