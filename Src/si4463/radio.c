/*! @file radio.c
 * @brief This file contains functions to interface with the radio chip.
 *
 * @b COPYRIGHT
 * @n Silicon Laboratories Confidential
 * @n Copyright 2012 Silicon Laboratories, Inc.
 * @n http://www.silabs.com
 */
#include "radio.h"
#include <stdio.h>

//#include "bsp.h"
// #include "..\bsp.h"

/*****************************************************************************
 *  Local Macros & Definitions
 *****************************************************************************/

/*****************************************************************************
 *  Global Variables
 *****************************************************************************/
// SEGMENT_VARIABLE(Radio_Configuration_Data_Array[], U8, SEG_CODE) = \
//               RADIO_CONFIGURATION_DATA_ARRAY;

// SEGMENT_VARIABLE(RadioConfiguration, tRadioConfiguration, SEG_CODE) = \
//                         RADIO_CONFIGURATION_DATA;

// SEGMENT_VARIABLE_SEGMENT_POINTER(pRadioConfiguration, tRadioConfiguration, SEG_CODE, SEG_CODE) = \
//                         &RadioConfiguration;


// SEGMENT_VARIABLE(customRadioPacket[RADIO_MAX_PACKET_LENGTH], U8, SEG_XDATA);



U8 Radio_Configuration_Data_Array[]=RADIO_CONFIGURATION_DATA_ARRAY;
tRadioConfiguration RadioConfiguration=RADIO_CONFIGURATION_DATA;
tRadioConfiguration *pRadioConfiguration=&RadioConfiguration;
U8 customRadioPacket[RADIO_MAX_PACKET_LENGTH];

/*****************************************************************************
 *  Local Function Declarations
 *****************************************************************************/
void vRadio_PowerUp(void);

/*!
 *  Power up the Radio.
 *
 *  @note
 *
 */
void vRadio_PowerUp(void)
{
  SEGMENT_VARIABLE(wDelay,  U16, SEG_XDATA) = 0u;

  /* Hardware reset the chip */
  si446x_reset();

  /* Wait until reset timeout or Reset IT signal */
  for (; wDelay < pRadioConfiguration->Radio_Delay_Cnt_After_Reset; wDelay++);

}

/*!
 *  Radio Initialization.
 *
 *  @author Sz. Papp
 *
 *  @note
 *
 */
void vRadio_Init(void)
{
  U16 wDelay;

  /* Power Up the radio chip */
  vRadio_PowerUp();

  /* Load radio configuration */
  while (SI446X_SUCCESS != si446x_configuration_init(pRadioConfiguration->Radio_ConfigurationArray))
  {
    /* Error hook */
// #if !(defined SILABS_PLATFORM_WMB912)
//     LED4 = !LED4;
// #else
//     vCio_ToggleLed(eHmi_Led4_c);
// #endif
    for (wDelay = 0x7FFF; wDelay--; ) ;
    /* Power Up the radio chip */
    vRadio_PowerUp();
  }

  // Read ITs, clear pending ones
  si446x_get_int_status(0u, 0u, 0u);
}

/*!
 *  Check if Packet sent IT flag or Packet Received IT is pending.
 *
 *  @return   SI4455_CMD_GET_INT_STATUS_REP_PACKET_SENT_PEND_BIT / SI4455_CMD_GET_INT_STATUS_REP_PACKET_RX_PEND_BIT
 *
 *  @note
 *
 */
U8 bRadio_Check_Tx_RX(void)
{
  int i;
  int len;
  if (radio_hal_NirqLevel() == FALSE)
  {
      /* Read ITs, clear pending ones */
      si446x_get_int_status(0u, 0u, 0u);
      
	  if (Si446xCmd.GET_INT_STATUS.CHIP_PEND & SI446X_CMD_GET_CHIP_STATUS_REP_CHIP_PEND_CMD_ERROR_PEND_BIT)
      {
      	/* State change to */
      	si446x_change_state(SI446X_CMD_CHANGE_STATE_ARG_NEXT_STATE1_NEW_STATE_ENUM_SLEEP);
	
	  	/* Reset FIFO */
      	si446x_fifo_info(SI446X_CMD_FIFO_INFO_ARG_FIFO_RX_BIT);
      
	  	/* State change to */
        si446x_change_state(SI446X_CMD_CHANGE_STATE_ARG_NEXT_STATE1_NEW_STATE_ENUM_RX);
      }

      if(Si446xCmd.GET_INT_STATUS.PH_PEND & SI446X_CMD_GET_INT_STATUS_REP_PH_PEND_PACKET_SENT_PEND_BIT)
      {
        return SI446X_CMD_GET_INT_STATUS_REP_PH_PEND_PACKET_SENT_PEND_BIT;
      }

      if(Si446xCmd.GET_INT_STATUS.PH_PEND & SI446X_CMD_GET_INT_STATUS_REP_PH_PEND_PACKET_RX_PEND_BIT)
      {
        /* Packet RX */

        /* Get payload length */
        si446x_fifo_info(0x00);

        si446x_read_rx_fifo(Si446xCmd.FIFO_INFO.RX_FIFO_COUNT, &customRadioPacket[0]);
        // si446x_read_rx_fifo(10, &customRadioPacket[0]);
        len =Si446xCmd.FIFO_INFO.RX_FIFO_COUNT;
        printf("the payload length is %d\n",len);
        // len=10;
        for(i=0;i<len;i++)
        {
          printf("0x%02x, ",customRadioPacket[i]);
        }

        return SI446X_CMD_GET_INT_STATUS_REP_PH_PEND_PACKET_RX_PEND_BIT;
      }
      
	  if (Si446xCmd.GET_INT_STATUS.PH_PEND & SI446X_CMD_GET_INT_STATUS_REP_PH_STATUS_CRC_ERROR_BIT)
      {
      	/* Reset FIFO */
        printf("-----------crc error-------------\n");
      	si446x_fifo_info(SI446X_CMD_FIFO_INFO_ARG_FIFO_RX_BIT);
      }


  }

  return 0;
}

/*!
 *  Set Radio to RX mode. .
 *
 *  @param channel Freq. Channel,  packetLength : 0 Packet handler fields are used , nonzero: only Field1 is used
 *
 *  @note
 *
 */
void vRadio_StartRX(U8 channel, U8 packetLenght )
{
  // Read ITs, clear pending ones
  si446x_get_int_status(0u, 0u, 0u);

  // set the rx length
  si446x_set_property(0x12,0x02,0x11,0x00,63);
   // Reset the Rx Fifo
   si446x_fifo_info(SI446X_CMD_FIFO_INFO_ARG_FIFO_RX_BIT);

  /* Start Receiving packet, channel 0, START immediately, Packet length used or not according to packetLength */
  // si446x_start_rx(channel, 0u, packetLenght,
  //                 SI446X_CMD_START_RX_ARG_NEXT_STATE1_RXTIMEOUT_STATE_ENUM_NOCHANGE,
  //                 SI446X_CMD_START_RX_ARG_NEXT_STATE2_RXVALID_STATE_ENUM_READY,
  //                 SI446X_CMD_START_RX_ARG_NEXT_STATE3_RXINVALID_STATE_ENUM_RX );

  si446x_start_rx(channel, 0u, 0,
                  SI446X_CMD_START_RX_ARG_NEXT_STATE1_RXTIMEOUT_STATE_ENUM_NOCHANGE,
                  SI446X_CMD_START_RX_ARG_NEXT_STATE2_RXVALID_STATE_ENUM_READY,
                  SI446X_CMD_START_RX_ARG_NEXT_STATE3_RXINVALID_STATE_ENUM_RX );
  printf("start rx--------------------\n");
}

/*!
 *  Set Radio to TX mode, variable packet length.
 *
 *  @param channel Freq. Channel, Packet to be sent length of of the packet sent to TXFIFO
 *
 *  @note
 *
 */
void vRadio_StartTx_Variable_Packet(U8 channel, U8 *pioRadioPacket, U8 length)
{
  int i;
  U8 *p;
  p= (U8*) malloc (length+1);
  p[0]=length;
  strncpy(p+1,pioRadioPacket,length);

  // /* Leave RX state */
  // si446x_change_state(SI446X_CMD_CHANGE_STATE_ARG_NEXT_STATE1_NEW_STATE_ENUM_READY);

  // /* Read ITs, clear pending ones */
  // si446x_get_int_status(0u, 0u, 0u);

  // /* Reset the Tx Fifo */
  // si446x_fifo_info(SI446X_CMD_FIFO_INFO_ARG_FIFO_TX_BIT);

  // // si446x_set_property(0x12,0x02,0x11,0x00,pioRadioPacket[0]);
  // /* Fill the TX fifo with datas */
  // si446x_write_tx_fifo(length,pioRadioPacket);
  // // si446x_write_tx_fifo(pioRadioPacket[0], &pioRadioPacket[1]);

  // /* Start sending packet, channel 0, START immediately */
  // si446x_start_tx(channel, 0x80, length);
  // // si446x_start_tx(channel, 0x80, 0);
  // printf("tx start----------------------\n");

  // si446x_set_property(0x12,0x02,0x11,0x00,pioRadioPacket[0]);

  // Read ITs, clear pending ones
  si446x_get_int_status(0u, 0u, 0u);

  // write the field2 length
  si446x_set_property(0x12,0x02,0x11,0x00,length);

  /* Reset TX FIFO */
  si446x_fifo_info(SI446X_CMD_FIFO_INFO_ARG_FIFO_TX_BIT);

  

  

  /* Fill the TX fifo with datas */
  for ( i = 0; i < length+1; i++)
  {
    printf("the %d is 0x%02x \n",i,p[i]);
  }
  
  si446x_write_tx_fifo(length+1, p);
  //si446x_write_tx_fifo(length, pioRadioPacket);
  free(p);

  /* Start sending packet, channel 0, START immediately, Packet length according to PH, go READY when done */
  si446x_start_tx(channel, 0x80,  0x00);
  
}

// #define POLY 0x8005 // CRC-16-MAXIM (IBM) (or 0xA001)
// #define POLY 0xA001
// unsigned int crc16(uint8_t data_p[])
// {
//     unsigned char i,j;
//     unsigned int data;
//     unsigned int crc = 0x0000;//0xFFFF;
//     // count = 0;

//     //for (j = 0; j < (sizeof(data_p)/sizeof(uint8_t)); j++)
//     for (j = 0; j < 7; j++)
//     {
//         for (i=0, data=(uint8_t)0xff & data_p[j];
//                 i < 8;
//                 i++, data >>= 1)
//         {
//             if ((crc & 0x0001) ^ (data & 0x0001))
//             {
//                 crc = (crc >> 1) ^ POLY;
//             }
//             else  crc >>= 1;
//         }
//     }

//     crc = ~crc;
//     data = crc;
//     crc = (crc << 8) | (data >> 8 & 0xff);

//     return (crc);
// }

unsigned int crc16_jf(uint16_t reg_crc,unsigned char *data, unsigned char length)
{
  int j;
  // unsigned int reg_crc = 0xFFFF;
  //unsigned int reg_crc = 0x0000;
  //unsigned int reg_crc = 0x1111;
  while (length--)
  {

    reg_crc ^= *data++;
    for (j = 0; j < 8; j++)
    {
      if (reg_crc & 0x01)
      {
        reg_crc = (reg_crc >> 1) ^ 0xA001;
        // reg_crc = (reg_crc >> 1) ^ 0x8005;
      }
      else
      {
        reg_crc = reg_crc >> 1;
      }
    }
  }
  return reg_crc;
}

unsigned int crc161(unsigned char *data, unsigned char length)
{
  int j;
  // unsigned int reg_crc = 0xFFFF;
  unsigned int reg_crc = 0x0000;
  // unsigned int reg_crc = 0x4230;
  while (length--)
  {

    reg_crc ^= *data++;
    for (j = 0; j < 8; j++)
    {
      if (reg_crc & 0x01)
      {
        reg_crc = (reg_crc >> 1) ^ 0xA001;
        // reg_crc = (reg_crc >> 1) ^ 0x8005;
      }
      else
      {
        reg_crc = reg_crc >> 1;
      }
    }
  }
  return reg_crc;
}
/**
 * @brief crc16 for IBM
 * which can get the same result as si4463
 * the important factor is not reverse,start from the MSB
 * @param data
 * @param length 
 * @return unsigned int 
 */
unsigned int crc16(unsigned char *data, unsigned char length)
{
  int j;
  // unsigned int reg_crc = 0xFFFF;
  unsigned int reg_crc = 0x0000;
  // unsigned int reg_crc = 0x4230;
  while (length--)
  {

    reg_crc ^= (*data++<<8);
    for (j = 0; j < 8; j++)
    {
      if (reg_crc & 0x8000)
      {
        // reg_crc = (reg_crc >> 1) ^ 0xA001;
        reg_crc = (reg_crc << 1) ^ 0x8005;
      }
      else
      {
        reg_crc = reg_crc << 1;
      }
    }
  }
  reg_crc&=0xffff;
  return reg_crc;
}

uint16_t crc16_tab[256] = {
	0x0000, 0xc0c1, 0xc181, 0x0140, 0xc301, 0x03c0, 0x0280, 0xc241,
	0xc601, 0x06c0, 0x0780, 0xc741, 0x0500, 0xc5c1, 0xc481, 0x0440,
	0xcc01, 0x0cc0, 0x0d80, 0xcd41, 0x0f00, 0xcfc1, 0xce81, 0x0e40,
	0x0a00, 0xcac1, 0xcb81, 0x0b40, 0xc901, 0x09c0, 0x0880, 0xc841,
	0xd801, 0x18c0, 0x1980, 0xd941, 0x1b00, 0xdbc1, 0xda81, 0x1a40,
	0x1e00, 0xdec1, 0xdf81, 0x1f40, 0xdd01, 0x1dc0, 0x1c80, 0xdc41,
	0x1400, 0xd4c1, 0xd581, 0x1540, 0xd701, 0x17c0, 0x1680, 0xd641,
	0xd201, 0x12c0, 0x1380, 0xd341, 0x1100, 0xd1c1, 0xd081, 0x1040,
	0xf001, 0x30c0, 0x3180, 0xf141, 0x3300, 0xf3c1, 0xf281, 0x3240,
	0x3600, 0xf6c1, 0xf781, 0x3740, 0xf501, 0x35c0, 0x3480, 0xf441,
	0x3c00, 0xfcc1, 0xfd81, 0x3d40, 0xff01, 0x3fc0, 0x3e80, 0xfe41,
	0xfa01, 0x3ac0, 0x3b80, 0xfb41, 0x3900, 0xf9c1, 0xf881, 0x3840,
	0x2800, 0xe8c1, 0xe981, 0x2940, 0xeb01, 0x2bc0, 0x2a80, 0xea41,
	0xee01, 0x2ec0, 0x2f80, 0xef41, 0x2d00, 0xedc1, 0xec81, 0x2c40,
	0xe401, 0x24c0, 0x2580, 0xe541, 0x2700, 0xe7c1, 0xe681, 0x2640,
	0x2200, 0xe2c1, 0xe381, 0x2340, 0xe101, 0x21c0, 0x2080, 0xe041,
	0xa001, 0x60c0, 0x6180, 0xa141, 0x6300, 0xa3c1, 0xa281, 0x6240,
	0x6600, 0xa6c1, 0xa781, 0x6740, 0xa501, 0x65c0, 0x6480, 0xa441,
	0x6c00, 0xacc1, 0xad81, 0x6d40, 0xaf01, 0x6fc0, 0x6e80, 0xae41,
	0xaa01, 0x6ac0, 0x6b80, 0xab41, 0x6900, 0xa9c1, 0xa881, 0x6840,
	0x7800, 0xb8c1, 0xb981, 0x7940, 0xbb01, 0x7bc0, 0x7a80, 0xba41,
	0xbe01, 0x7ec0, 0x7f80, 0xbf41, 0x7d00, 0xbdc1, 0xbc81, 0x7c40,
	0xb401, 0x74c0, 0x7580, 0xb541, 0x7700, 0xb7c1, 0xb681, 0x7640,
	0x7200, 0xb2c1, 0xb381, 0x7340, 0xb101, 0x71c0, 0x7080, 0xb041,
	0x5000, 0x90c1, 0x9181, 0x5140, 0x9301, 0x53c0, 0x5280, 0x9241,
	0x9601, 0x56c0, 0x5780, 0x9741, 0x5500, 0x95c1, 0x9481, 0x5440,
	0x9c01, 0x5cc0, 0x5d80, 0x9d41, 0x5f00, 0x9fc1, 0x9e81, 0x5e40,
	0x5a00, 0x9ac1, 0x9b81, 0x5b40, 0x9901, 0x59c0, 0x5880, 0x9841,
	0x8801, 0x48c0, 0x4980, 0x8941, 0x4b00, 0x8bc1, 0x8a81, 0x4a40,
	0x4e00, 0x8ec1, 0x8f81, 0x4f40, 0x8d01, 0x4dc0, 0x4c80, 0x8c41,
	0x4400, 0x84c1, 0x8581, 0x4540, 0x8701, 0x47c0, 0x4680, 0x8641,
	0x8201, 0x42c0, 0x4380, 0x8341, 0x4100, 0x81c1, 0x8081, 0x4040
};

uint16_t crc16_table(uint16_t crc, const void *buf, size_t size)
{
  const uint8_t *p;

  p = buf;

  while (size--)
    crc = crc16_tab[(crc ^ (*p++)) & 0xFF] ^ (crc >> 8);

  return crc;
}

uint16_t GetCRC16(uint16_t CRC16, unsigned char cur_byte)
{
  unsigned char uIndex = 8;
  uint16_t Carry;
  CRC16 = CRC16 ^ cur_byte;
  do
  {
    Carry = CRC16 & 0x0001;
    CRC16 = CRC16 >> 1;
    CRC16 &= 0x7fff;
    if (Carry == 1)
    {
      CRC16 = CRC16 ^ 0xA001;
    }
  } while (--uIndex); // pass through message buffer
  return CRC16;
}

uint16_t CRC16(unsigned char *data_array, uint16_t length)
{
  uint16_t CalcCRC16;
  uint16_t cnt1, cnt2;
  // CalcCRC16 = 0xFFFF;
  CalcCRC16 = 0x0000;
  cnt1 = length;
  cnt2 = 0;
  while (cnt2 < cnt1)
  {
    CalcCRC16 = GetCRC16(CalcCRC16, data_array[cnt2++]);
  }
  return CalcCRC16;
}
