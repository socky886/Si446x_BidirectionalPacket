/*! @file radio.c
 * @brief This file contains functions to interface with the radio chip.
 *
 * @b COPYRIGHT
 * @n Silicon Laboratories Confidential
 * @n Copyright 2012 Silicon Laboratories, Inc.
 * @n http://www.silabs.com
 */

#include "..\bsp.h"

/*****************************************************************************
 *  Local Macros & Definitions
 *****************************************************************************/

/*****************************************************************************
 *  Global Variables
 *****************************************************************************/
const SEGMENT_VARIABLE(Radio_Configuration_Data_Array[], U8, SEG_CODE) = \
              RADIO_CONFIGURATION_DATA_ARRAY;

const SEGMENT_VARIABLE(RadioConfiguration, tRadioConfiguration, SEG_CODE) = \
                        RADIO_CONFIGURATION_DATA;

const SEGMENT_VARIABLE_SEGMENT_POINTER(pRadioConfiguration, tRadioConfiguration, SEG_CODE, SEG_CODE) = \
                        &RadioConfiguration;


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
#if !(defined SILABS_PLATFORM_WMB912)
    LED4 = !LED4;
#else
    vCio_ToggleLed(eHmi_Led4_c);
#endif
    for (wDelay = 0x7FFF; wDelay--; ) ;
    /* Power Up the radio chip */
    vRadio_PowerUp();
  }

  // Read ITs, clear pending ones
  si446x_get_int_status(0u, 0u, 0u);
}

/*!
 *  Check if Packet sent IT flag is pending.
 *
 *  @return   TRUE / FALSE
 *
 *  @note
 *
 */
BIT gRadio_CheckTransmitted(void)
{
  if (RF_NIRQ == FALSE)
  {
    /* Read ITs, clear pending ones */
    si446x_get_int_status(0u, 0u, 0u);

    /* check the reason for the IT */
    if (Si446xCmd.GET_INT_STATUS.PH_PEND & SI446X_CMD_GET_INT_STATUS_REP_PH_PEND_PACKET_SENT_PEND_BIT)
    {
      return TRUE;
    }
  }

  return FALSE;
}

/*!
 *  Set Radio to TX mode, fixed packet length.
 *
 *  @param channel Freq. Channel, Packet to be sent
 *
 *  @note
 *
 */
void  vRadio_StartTx(U8 channel, U8 *pioFixRadioPacket)
{
  /* Reset TX FIFO */
  si446x_fifo_info(SI446X_CMD_FIFO_INFO_ARG_FIFO_TX_BIT);

  // Read ITs, clear pending ones
  si446x_get_int_status(0u, 0u, 0u);

  /* Fill the TX fifo with datas */
  si446x_write_tx_fifo(RadioConfiguration.Radio_PacketLength, pioFixRadioPacket);

  /* Start sending packet, channel 0, START immediately, Packet length according to PH, go READY when done */
  si446x_start_tx(channel, 0x30,  0x00);
}
