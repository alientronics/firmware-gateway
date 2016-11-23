/*
 * RFM.c
 *
 *  Created on: Mar 23, 2016
 *      Author: b12072
 */
//==============================================================
// RFMod_Fun.c - RF MODULE FUNCTIONS.                 2016-04-01
//==============================================================

#include "Cpu.h"
#include "Events.h"
#include "Init_Config.h"
#include "PDD_Includes.h"

#include <stdio.h>
#include <string.h>
#include "RFMod_Fun.h"


//==============================================================
// LOCAL FUNCTIONS:
//--------------------------------------------------------------
void vfnDelay_us(uint16_t delay_us);


//==============================================================
// DATA:
//--------------------------------------------------------------
const uint8_t RFMRegInit[][2] =
// REGISTER,  VALUE
 {{RegOpMode, 0x04},         // STDBY MODE
  {RegDataModul, 0x00},      // PACKET, FSK, SHAPING 0
  {RegBitrateMsb, 0x06},     // 0x0680-19230
  {RegBitrateLsb, 0x80},     //
  {RegFdevMsb, 0x03},        // 0x0333 => Fdev=50kHz
  {RegFdevLsb, 0x33},
  {RegFrfMsb, 0x6C},         // Frf=433.995MHz => 0x6C7FAE
  {RegFrfMid, 0x7F},         // Frf=434.000MHz => 0x6C8000
  {RegFrfLsb, 0xAE},         //
  {RegAfcCtrl, 0x00},

  {RegPaLevel, 0x9F},        // PA0 ON, Pout=11111
  {RegPaRamp, 0x09},         // 1001->40us
  {RegOcp, 0x1C},            // OCP ON, OCP=105mA

  {RegLna, 0x00},            // 50OHM, AGC ON
  {RegRxBw, 0x52},           // 0x52=01010010 => DccFreq=010,RcBwMant=10,RxBwExp=010, FSK BW=83.3kHz (0x43, 62.5kHz ALSO OK)
  {RegAfcBw, 0x52},          // 83.3kHz
  {RegAfcFei, 0x08},         // 0x08 AfcAutoclearOn=1, AfcAutoOn=0

  {RegDioMapping1, 0x60},    // 0x60 => DIO0=01-PayloadReady, DIO1=10-FifoNotEmpty
  {RegDioMapping2, 0x07},    // ClkOut=111-OFF

  {RegRssiThresh, 0xE4},     // 0xE4 => RssiThreshold=-114dBm

  {RegPreambleMsb, 0x00},    // 0x0003 => PREAMBLE SIZE 3
  {RegPreambleLsb, 0x03}, 
  /**/{RegSyncConfig, 0x98},     // 0x90 => SyncOn=1, SyncSize=2(3 SYNC BYTES), SyncTol=000

  {RegSyncValue1, 0x01},     // 1-ST SYNC BYTE
  {RegSyncValue2, 0x01},     // 2-ND SYNC BYTE
  {RegSyncValue3, 0x01},     // 3-RD SYNC BYTE
  {RegSyncValue4, 0x01},     // 4-TH SYNC BYTE
  {RegSyncValue5, 0x01},     // 5-TH SYNC BYTE
  {RegSyncValue6, 0x01},     // 6-TH SYNC BYTE
  {RegSyncValue7, 0x01},     // 7-TH SYNC BYTE
  {RegSyncValue8, 0x01},     // 8-TH SYNC BYTE

  /**/{RegPacketConfig1, 0x10},  //  0x98 PacketFormat=1-VAR LENGTH, DcFree=00-NONE,
                             	 //  CrcOn=1-CALC CRC, CrcAutoClearOff=1-DON'T CLEAR FIFO,
                             	 // AddressFiltering=00-NONE

  /**/{RegPayloadLength, 0x17},  // 0x12=18 BYTES MAX PAYLOAD LENGTH

  {RegAutoModes, 0x01},      // EnterCond=000-NONE, ExitCond=000-NONE, IntermCond=01-STDBY

  {RegFifoThresh, 0x00},     // FifoThreshold=0

  {RegPacketConfig2, 0x02},  // AutoRxRestartOn=1-RESTART RX AFTER EACH PACKET

  {RegListen1, 0x50},
  {RegListen2, 0x01},
  {RegListen3, 0x01},

  {RegNodeAdrs, 0xF0},
  {RegBroadcastAdrs, 0xFF}};

uint8_t RFMod_Flag;


//==============================================================
// CODE:
//--------------------------------------------------------------

//==============================================================
// RFMod_RdReg() - READ RF MODULE REGISTERS.
// INPUT PARAMS:
//  uint8_t *pu8Data - PTR TO RX DATA BUFFER,
//  uint8_t u8Addr - RFM STARTING REGISTER ADDR,
//  uint8_t u8Len - NUMBER OF BYTES TO READ.
//--------------------------------------------------------------
void RFMod_RdReg(uint8_t *pu8Data, uint8_t u8Addr, uint8_t u8Len)
  {
  uint8_t u1;
  uint8_t u2;

  // RFM CS LOW:
  RFM_NSS_PTD0_ClrVal(NULL);
  vfnDelay_us(5);

  // PREPARE RX TO READ 1 BYTE:
  RFM_SPIM_ReceiveBlock(RFM_SPIM_DeviceData, &u1, 1);
  // XMIT ADDR BYTE TO READ DATA FROM:
  RFMod_Flag = 0;
  u8Addr |= 0x00;  // BIT7=0 - SPI READING FROM SLAVE
  RFM_SPIM_SendBlock(RFM_SPIM_DeviceData, &u8Addr, 1);
  while (RFMod_Flag == 0);
  // ADDR BYTE XFERRED

  // XFER u8Len DATA BYTES THRU SPI:
  u1 = 0;
  for (u2=0; u2<u8Len; ++u2)
    {
    // GET SPI READY TO RCV NEXT DATA BYTE:
    RFM_SPIM_ReceiveBlock(RFM_SPIM_DeviceData, &pu8Data[u2], 1);
    RFMod_Flag = 0;
    // XMIT ANYTHING TO MAKE SPI XFER:
    RFM_SPIM_SendBlock(RFM_SPIM_DeviceData, &u1, 1);
    while (RFMod_Flag == 0);
    // SPI BYTE XFER COMPLETED
    }
  // pu8Data[] HAS u8Len DATA BYTES READ FROM SPI

  vfnDelay_us(5);
  // RFM CS HIGH:
  RFM_NSS_PTD0_SetVal(NULL);

  return;
  }
//==============================================================
// RFMod_WrReg() - WRITE TO RF MODULE REGISTERS.
// INPUT PARAMS:
//  uint8_t *pu8Data - PTR TO TX DATA BUFFER,
//  uint8_t u8Addr - RFM STARTING REGISTER ADDR,
//  uint8_t u8Len - NUMBER OF BYTES TO WRITE.
//--------------------------------------------------------------
void RFMod_WrReg(uint8_t *pu8Data, uint8_t u8Addr, uint8_t u8Len)
  {
  uint8_t u1;
  uint8_t u2;

  // RFM CS LOW:
  RFM_NSS_PTD0_ClrVal(NULL);
  vfnDelay_us(5);

  // PREPARE RX TO READ 1 BYTE:
  RFM_SPIM_ReceiveBlock(RFM_SPIM_DeviceData, &u1, 1);
  // XMIT ADDR BYTE TO READ DATA FROM:
  RFMod_Flag = 0;
  u8Addr |= 0x80;  // BIT7=0 - SPI WRITING TO SLAVE
  RFM_SPIM_SendBlock(RFM_SPIM_DeviceData, &u8Addr, 1);
  while (RFMod_Flag == 0);
  // ADDR BYTE XFERRED

  // XFER u8Len DATA BYTES THRU SPI:
  u1 = 0;
  for (u2=0; u2<u8Len; ++u2)
    {
    // GET SPI READY TO RCV NEXT DATA BYTE:
    RFM_SPIM_ReceiveBlock(RFM_SPIM_DeviceData, &u1, 1);
    RFMod_Flag = 0;
    // XMIT NEXT DATA BYTE THRU SPI :
    RFM_SPIM_SendBlock(RFM_SPIM_DeviceData, &pu8Data[u2], 1);
    while (RFMod_Flag == 0);
    // SPI BYTE XFER COMPLETED
    }
  // u8Len DATA BYTES FROM pu8Data[] XMITTED THRU SPI

  vfnDelay_us(5);
  // RFM CS HIGH:
  RFM_NSS_PTD0_SetVal(NULL);

  return;
  }
//==============================================================
// RFMod_WrReg1b() - WRITE 1 BYTE TO RF MODULE REGISTER.
// INPUT PARAMS:
//  uint8_t u8Data - DATA TO WRITE TO REG,
//  uint8_t u8Reg - REG ADDR TO WRITE TO.
//--------------------------------------------------------------
void RFMod_WrReg1b(uint8_t u8Data, uint8_t u8Reg)
  {
  RFMod_WrReg(&u8Data, u8Reg, 1);
  return;
  }
//==============================================================
// RFM_Reset() - HARDWARE RESET OF RF MODULE.
//--------------------------------------------------------------
void RFMod_Reset(void)
  {
  RFM_RESET_PTE30_SetVal(NULL);  // PTE30=1
  vfnDelay_us(200);              // 100us DELAY
  RFM_RESET_PTE30_ClrVal(NULL);  // PTE30=0
  vfnDelay_us(8000);             // 5ms DELAY
  return;
  }
//==============================================================
// RFM_Init() - INITIALIZE REGISTERS OF RF MODULE.
//--------------------------------------------------------------
void RFMod_Init(void)
  {
  uint8_t u8I1;
  uint8_t u8I2;

  u8I1 = sizeof(RFMRegInit);
  u8I1 = u8I1 / 2;  // # OF REGS TO INITIALIZE

  // INITIALIZE RFM REGISTERS:
  for (u8I2=0; u8I2<u8I1; ++u8I2)
    RFMod_WrReg(&RFMRegInit[u8I2][1], RFMRegInit[u8I2][0], 1);

  return;
  }
//==============================================================
// DELAY IN us.
// DELAYS CODE EXECUTION BY delay_us MICRO SECONDS.
// NOT ACCURATE FOR LOW delay_us:
//   delay_us    ACTUAL DELAY
//     100          175
//     500          566
//    1000         1060
//    5000         5000
//   10000         9920
//--------------------------------------------------------------
void vfnDelay_us(uint16_t delay_us)
  {
  uint32_t u32Cnt;
  uint32_t u32_1;
  volatile uint16_t u16_1;
  volatile uint16_t u16_2;
  float f1;
  float f2;


  f1 = PE_CpuClockConfigurations[0].cpu_core_clk_hz / 1000.0;
  f2 = (float)delay_us * (float)(57750L);
//  u32_1 = round(f2/f1);
  u32_1 = (uint32_t)(f2/f1);

  u16_1 = 0;
  u16_2 = 4;

  for (u32Cnt = 0; u32Cnt < u32_1; ++u32Cnt)
    {
	u16_1 = u16_1 + u16_2 + 9;
    }
  return;
  }
//==============================================================





