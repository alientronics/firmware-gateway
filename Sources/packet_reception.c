/*
 * packet_reception.c
 *
 *  Created on: 10/09/2016
 *      Author: joaob
 */

#include "Cpu.h"
#include "Events.h"
#include "Init_Config.h"
#include "PDD_Includes.h"

#include "packet_reception.h"

//==============================================================
// CODE:
//--------------------------------------------------------------

//==============================================================
// decodeFrame() - Efetua leitura do buffer de dados recebidos
// através da RF, posteriomente alocando os dados formatados em
// uma estrutura do tipo "dataPayload".
// INPUT PARAMS:
//  uint8_t *data - Ponteiro para o buffer de entrada;
//  dataPayload *message - Ponteiro para a estrutara de alocação;
//--------------------------------------------------------------
void decodeFrame (uint8_t *data, dataPayload *message){
	message->TireID = (data[2]<<24) + (data[3]<<16) + (data[4]<<8) + (data[5]);
	// Processo de conversão de pressão também é feito segundo o datasheet
	message->pressure = ((((data[6]<<8) + (data[7]))*800)/1023)+100;
	// O valor 255 não parece coerente, contudo esta teoricamente correto segundo o datasheet
	message->acceZ = (data[8]<<8) + (data[9])-255;
	// Já para o eixo X o valor 255 parece correto
	message->acceX = (data[10]<<8) + (data[11])-255;
	// O valor de divisão de 60 ou 59 são os que transparecem maior coerência
	message->voltage = (data[12]/60.f);
	// -55 é especificação do datasheet
	message->temp = (data[13]-55);
	message->frameID = (data[14]<<8) + (data[15]);
}


