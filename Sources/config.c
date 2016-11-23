/*
 * config.c
 *
 *  Created on: 05/11/2016
 *      Author: joaob
 */

#include "Cpu.h"
#include "Events.h"
#include "Init_Config.h"
#include "PDD_Includes.h"

#include <stdio.h>
#include <string.h>

#include "RFMod_Fun.h"
#include "config.h"

//==============================================================
// CODE:
//--------------------------------------------------------------

//==============================================================
// uConfig() - Configuração do Microcontrolador
//--------------------------------------------------------------

void uConfig(void){
	// Mensagem de inicialização:
	uint8_t *ptr_data;
	ptr_data = "Gateway Inicialization ok!\n";
	UART0_SendBlock(UART0_DeviceData, (uint8_t *) ptr_data, 27);

	delay_ms(200);	// DELAY 200ms:

	// INITIALIZE RF MODULE:
	// HARDWARE RESET:
	RFMod_Reset();
	RFMod_RdReg(au8RRxD, 0x10, 1);  // READ RFM SILICIN VERSION

	// INITIALIZE RFM REGISTERS:
	RFMod_Init();

	delay_ms(200); 	// DELAY 200ms

	// SWITCH RFM TO RX MODE:
	RFMod_WrReg1b(0x10, RegOpMode); // Mode=100-RX
	// WAIT FOR RX ModeReady:
	au8RRxD[0] = 0x00;
	while ((au8RRxD[0] & 0x80) == 0)
		RFMod_RdReg(au8RRxD, RegIrqFlags1, 1);
	// RFM IS IN RX MODE.

	// DELAY 200ms:
	delay_ms(200);

	// START RSSI MEASUREMENT:
	RFMod_WrReg1b(0x01, RegRssiConfig); // RssiStart=1
}

//==============================================================
// delay_ms() - Função de delay de milisegundos
// INPUT PARAMS:
//  uint16_t value - Valor do delay em milisegundos;
//--------------------------------------------------------------
void delay_ms(uint16_t value){
	t1_counter = value*10;
	while (t1_counter);
}

//==============================================================
// delay_faster() - Função de delay 10x mais rápida que delay_ms
// INPUT PARAMS:
//  uint16_t value - Valor do delay em 0.1 milisegundos;
//--------------------------------------------------------------
void delay_faster(uint16_t value){
	t1_counter = value;
	while (t1_counter);
}

//==============================================================



