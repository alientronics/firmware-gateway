/* ###################################################################
**     Filename    : main.c
**     Project     : MKW01_RX1
**     Processor   : MKW01Z128CHN4
**     Version     : Driver 01.01
**     Compiler    : GNU C Compiler
**     Date/Time   : 2016-03-17, 12:57, # CodeGen: 0
**     Abstract    :
**         Main module.
**         This module contains user's application code.
**     Settings    :
**     Contents    :
**         No public methods
**
** ###################################################################*/
/*!
** @file main.c
** @version 01.01
** @brief
**         Main module.
**         This module contains user's application code.
*/         
/*!
**  @addtogroup main_module main module documentation
**  @{
*/         
/* MODULE main */


/* Including needed modules to compile this module/procedure */
#include "Cpu.h"
#include "Events.h"
#include "TI1.h"
#include "TU1.h"
#include "UART0.h"
#include "RFM_SPIM.h"
#include "DMA1.h"
#include "RFM_NSS_PTD0.h"
#include "RFM_RESET_PTE30.h"
#include "RFM_DIO0_PTE2.h"
#include "RFM_DIO1_PTE3.h"
#include "mReset.h"
#include "mWP.h"
#include "mCS.h"
#include "SPI1.h"
/* Including shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
#include "PDD_Includes.h"
#include "Init_Config.h"

#include <stdio.h>
#include <string.h>

#include "config.h"
#include "RFMod_Fun.h"
#include "packet_reception.h"
#include "SPI_encapsulation.h"
#include "memory_AT45DB161.h"
/* User includes (#include below this line is not maintained by Processor Expert) */

//==============================================================
// LOCAL FUNCTIONS:
//--------------------------------------------------------------
void RF_application(void);

//==============================================================
// GLOBAL DATA:
//--------------------------------------------------------------
volatile bool DataReceivedFlag_SPI = FALSE; // SPI1 Received Interrupt Variable
volatile bool DataSentFlag_SPI = FALSE; 	// SPI1 Sent Interrupt Variable
volatile uint16_t t1_counter=0; 			// TIMER1 Interrupt Counter
dataPayload message;

/*lint -save  -e970 Disable MISRA rule (6.3) checking. */
int main(void)
/*lint -restore Enable MISRA rule (6.3) checking. */
{
	/* Write your local variable definition here */
	/*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
	PE_low_level_init();
	/*** End of Processor Expert internal initialization.                    ***/
	/* Write your code here */
	uConfig();

	uint8_t databuffer[2] = {0x00, 0x00};
	uint8_t bufferTeste[20];
	uint8_t countTeste=0;

	while(1){
	  t1_counter = 10;
	  while (t1_counter);

	  /*
	  DataReceivedFlag_SPI = FALSE;
	  SPI1_ReceiveBlock(SPI1_DeviceData, &databuffer[3], 3); // Request data block reception
	  SPI1_SendBlock(SPI1_DeviceData, &databuffer[0], 3);    // Start transmission/reception
	  while (!DataReceivedFlag_SPI);
	  */

	  mem_cs_0;
	  spi_transmit(status_register);
	  spi_receive_multiples(databuffer, 2);
	  mem_cs_1;

	  if(countTeste>200){
		  sprintf(bufferTeste, "%X %X\n", databuffer[0], databuffer[1]);
		  UART0_SendBlock(UART0_DeviceData, bufferTeste, strlen(bufferTeste));
		  countTeste = 0;
	  }
	  else countTeste++;
	}

	// MAIN APPLICATION LOOP:
	while (1){
		RF_application();
  }
  /*** Don't write any code pass this line, or it will be deleted during code generation. ***/
  /*** RTOS startup code. Macro PEX_RTOS_START is defined by the RTOS component. DON'T MODIFY THIS CODE!!! ***/
  #ifdef PEX_RTOS_START
    PEX_RTOS_START();                  /* Startup of the selected RTOS. Macro is defined by the RTOS component. */
  #endif
  /*** End of RTOS startup code.  ***/
  /*** Processor Expert end of main routine. DON'T MODIFY THIS CODE!!! ***/
  for(;;){}
  /*** Processor Expert end of main routine. DON'T WRITE CODE BELOW!!! ***/
} /*** End of main routine. DO NOT MODIFY THIS TEXT!!! ***/

void RF_application(void){
	bool check_RF = 0;

	// CHECK IF NEW PACKET RCVD:
	check_RF = RFM_DIO0_PTE2_GetVal(RFM_DIO0_PTE2_DeviceData);

	if (check_RF == TRUE){
		// READ PACKET FROM FIFO
		// ENTER STDBY MODE:
		RFMod_WrReg1b(0x04, RegOpMode); // Mode=001-STDBY
		// WAIT FOR STDBY ModeReady:
		au8RRxD[0] = 0x00;
		while ((au8RRxD[0] & 0x80) == 0){
			RFMod_RdReg(au8RRxD, RegIrqFlags1, 1); // RFM IS IN STDBY MODE.

			// READ PACKET FROM FIFO:
			u8RRxIdx = 0;
			t1_counter = 100;
		}
		while ((check_RF == TRUE) || (t1_counter > 0)){
			RFMod_RdReg(&au8RRxD[u8RRxIdx], RegFifo, 1);
			if (u8RRxIdx < (RFMRXBLEN-1)){
				++u8RRxIdx;
				check_RF = RFM_DIO0_PTE2_GetVal(RFM_DIO0_PTE2_DeviceData);
			}
			if (check_RF == TRUE) t1_counter = 100;
		}

		decodeFrame(au8RRxD, &message);
		sprintf(UTxBuf, "\rTire_ID: %X, press: %d, acceZ: %d, acceX: %d, volt: %u, temp: %d, frame_ID: %u",
				message.TireID, message.pressure, message.acceZ, message.acceX,
				(uint32_t)(message.voltage*100), message.temp, message.frameID);
		UART0_SendBlock(UART0_DeviceData, UTxBuf, strlen(UTxBuf));
		// ENTER RX MODE:
		RFMod_WrReg1b(0x10, RegOpMode); // Mode=100-RX
		// WAIT FOR RX ModeReady:
		au8RRxD[0] = 0x00;
		while ((au8RRxD[0] & 0x80) == 0)
			RFMod_RdReg(au8RRxD, RegIrqFlags1, 1);
		// RFM IS IN RX MODE.
	}
}

/* END main */
/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.5 [05.21]
**     for the Freescale Kinetis series of microcontrollers.
**
** ###################################################################
*/
