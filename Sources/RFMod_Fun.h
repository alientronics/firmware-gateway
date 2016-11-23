/*
 * RFM.h
 *
 *  Created on: Mar 23, 2016
 *      Author: b12072
 */
//=============================================================
// RFMod_Fun.h - RF MODULE FUNCTIONS.                2016-03-24
//=============================================================

#ifndef SOURCES_RFMOD_FUN_H_
#define SOURCES_RFMOD_FUN_H_

// MKW01 REGISTERS:
#define RegFifo            0x00
#define RegOpMode          0x01
#define RegDataModul       0x02
#define RegBitrateMsb      0x03
#define RegBitrateLsb      0x04
#define RegFdevMsb         0x05
#define RegFdevLsb         0x06
#define RegFrfMsb          0x07
#define RegFrfMid          0x08
#define RegFrfLsb          0x09
#define RegOsc1            0x0A
#define RegAfcCtrl         0x0B
#define RegLowBat          0x0C
#define RegListen1         0x0D
#define RegListen2         0x0E
#define RegListen3         0x0F
#define RegVersion         0x10
#define RegPaLevel         0x11
#define RegPaRamp          0x12
#define RegOcp             0x13
#define RegLna             0x18
#define RegRxBw            0x19
#define RegAfcBw           0x1A
#define RegOokPeak         0x1B
#define RegOokAvg          0x1C
#define RegOokFix          0x1D
#define RegAfcFei          0x1E
#define RegAfcMsb          0x1F
#define RegAfcLsb          0x20
#define RegFeiMsb          0x21
#define RegFeiLsb          0x22
#define RegRssiConfig      0x23
#define RegRssiValue       0x24
#define RegDioMapping1     0x25
#define RegDioMapping2     0x26
#define RegIrqFlags1       0x27
#define RegIrqFlags2       0x28
#define RegRssiThresh      0x29
#define RegRxTimeout1      0x2A
#define RegRxTimeout2      0x2B
#define RegPreambleMsb     0x2C
#define RegPreambleLsb     0x2D
#define RegSyncConfig      0x2E
#define RegSyncValue1      0x2F
#define RegSyncValue2      0x30
#define RegSyncValue3      0x31
#define RegSyncValue4      0x32
#define RegSyncValue5      0x33
#define RegSyncValue6      0x34
#define RegSyncValue7      0x35
#define RegSyncValue8      0x36
#define RegPacketConfig1   0x37
#define RegPayloadLength   0x38
#define RegNodeAdrs        0x39
#define RegBroadcastAdrs   0x3A
#define RegAutoModes       0x3B
#define RegFifoThresh      0x3C
#define RegPacketConfig2   0x3D
#define RegTemp1           0x4E
#define RegTemp2           0x4F
#define RegTestLna         0x58


//-------------------------------------------------------------
// PUBLIC VARIABLES IN RFM.c:

extern uint8_t RFMod_Flag;


//-------------------------------------------------------------
// PUBLIC FUNCTIONS IN RFMod.c:
void RFMod_RdReg(uint8_t *pu8Data, uint8_t u8Addr, uint8_t u8Len);
void RFMod_WrReg(uint8_t *pu8Data, uint8_t u8Addr, uint8_t u8Len);
void RFMod_WrReg1b(uint8_t u8Data, uint8_t u8Reg);
void RFMod_Reset(void);
void RFMod_Init(void);

//-------------------------------------------------------------


#endif /* SOURCES_RFMOD_FUN_H_ */
