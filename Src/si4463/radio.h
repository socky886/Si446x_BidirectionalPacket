/*! @file radio.h
 * @brief This file is contains the public radio interface functions.
 *
 * @b COPYRIGHT
 * @n Silicon Laboratories Confidential
 * @n Copyright 2012 Silicon Laboratories, Inc.
 * @n http://www.silabs.com
 */

#ifndef RADIO_H_
#define RADIO_H_

#include "bsp.h"
/*****************************************************************************
 *  Global Macros & Definitions
 *****************************************************************************/
/*! Maximal packet length definition (FIFO size) */
#define RADIO_MAX_PACKET_LENGTH     64u

/*****************************************************************************
 *  Global Typedefs & Enums
 *****************************************************************************/
typedef struct
{
    U8   *Radio_ConfigurationArray;

    U8   Radio_ChannelNumber;
    U8   Radio_PacketLength;
    U8   Radio_State_After_Power_Up;

    U16  Radio_Delay_Cnt_After_Reset;

    U8   Radio_CustomPayload[RADIO_MAX_PACKET_LENGTH];
} tRadioConfiguration;

/*****************************************************************************
 *  Global Variable Declarations
 *****************************************************************************/
// extern  SEGMENT_VARIABLE_SEGMENT_POINTER(pRadioConfiguration, tRadioConfiguration, SEG_CODE, SEG_CODE);
// extern SEGMENT_VARIABLE(customRadioPacket[RADIO_MAX_PACKET_LENGTH], U8, SEG_XDATA);
extern tRadioConfiguration *pRadioConfiguration;
extern U8 customRadioPacket[RADIO_MAX_PACKET_LENGTH];
/*! Si446x configuration array */
// extern  SEGMENT_VARIABLE(Radio_Configuration_Data_Array[], U8, SEG_CODE);
extern U8 Radio_Configuration_Data_Array[];

/*****************************************************************************
 *  Global Function Declarations
 *****************************************************************************/
void  vRadio_Init(void);
U8    bRadio_Check_Tx_RX(void);
void  vRadio_StartRX(U8,U8);
U8    bRadio_Check_Ezconfig(U16);
void  vRadio_StartTx_Variable_Packet(U8, U8*, U8);
unsigned int crc16(unsigned char *data, unsigned char length);
uint16_t crc16_table(uint16_t crc, const void *buf, size_t size);

uint16_t CRC16(unsigned char *data_array, uint16_t length);
#endif /* RADIO_H_ */
