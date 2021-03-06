/*!
 * File:
 *  radio_hal.c
 *
 * Description:
 *  This file contains RADIO HAL.
 *
 * Silicon Laboratories Confidential
 * Copyright 2011 Silicon Laboratories, Inc.
 */

                /* ======================================= *
                 *              I N C L U D E              *
                 * ======================================= */

#include "bsp.h"


                /* ======================================= *
                 *          D E F I N I T I O N S          *
                 * ======================================= */

                /* ======================================= *
                 *     G L O B A L   V A R I A B L E S     *
                 * ======================================= */

                /* ======================================= *
                 *      L O C A L   F U N C T I O N S      *
                 * ======================================= */

                /* ======================================= *
                 *     P U B L I C   F U N C T I O N S     *
                 * ======================================= */

void radio_hal_AssertShutdown(void)
{
// #if (defined SILABS_PLATFORM_RFSTICK) || (defined SILABS_PLATFORM_LCDBB) || (defined SILABS_PLATFORM_WMB)
//   RF_PWRDN = 1;
// #else
//   PWRDN = 1;
// #endif
    HAL_GPIO_WritePin(SX_NRST_GPIO_Port,SX_NRST_Pin,1);
}

void radio_hal_DeassertShutdown(void)
{
// #if (defined SILABS_PLATFORM_RFSTICK) || (defined SILABS_PLATFORM_LCDBB) || (defined SILABS_PLATFORM_WMB)
//   RF_PWRDN = 0;
// #else
//   PWRDN = 0;
// #endif
  HAL_GPIO_WritePin(SX_NRST_GPIO_Port,SX_NRST_Pin,0);
}

void radio_hal_ClearNsel(void)
{
    // RF_NSEL = 0;
    HAL_GPIO_WritePin(SPI1_NSS_GPIO_Port,SPI1_NSS_Pin,0);
}

void radio_hal_SetNsel(void)
{
    // RF_NSEL = 1;
    HAL_GPIO_WritePin(SPI1_NSS_GPIO_Port,SPI1_NSS_Pin,1);
}

BIT radio_hal_NirqLevel(void)
{
    // return RF_NIRQ;
    return HAL_GPIO_ReadPin(NIRQ_GPIO_Port,NIRQ_Pin);
    
}

void radio_hal_SpiWriteByte(U8 byteToWrite)
{
// #if (defined SILABS_PLATFORM_RFSTICK) || (defined SILABS_PLATFORM_LCDBB) || (defined SILABS_PLATFORM_WMB)
//   bSpi_ReadWriteSpi1(byteToWrite);
// #else
//   SpiReadWrite(byteToWrite);
// #endif
  SpiInOut(  byteToWrite );
}

U8 radio_hal_SpiReadByte(void)
{
// #if (defined SILABS_PLATFORM_RFSTICK) || (defined SILABS_PLATFORM_LCDBB) || (defined SILABS_PLATFORM_WMB)
//   return bSpi_ReadWriteSpi1(0xFF);
// #else
//   return SpiReadWrite(0xFF);
// #endif
   U8 rs=SpiInOut(0x00);
   return rs;
}

void radio_hal_SpiWriteData(U8 byteCount, U8* pData)
{
// #if (defined SILABS_PLATFORM_RFSTICK) || (defined SILABS_PLATFORM_LCDBB) || (defined SILABS_PLATFORM_WMB)
//   vSpi_WriteDataSpi1(byteCount, pData);
// #else
//   SpiWriteData(byteCount, pData);
// #endif
   int i;
   for ( i = 0; i < byteCount; i++)
   {
      SpiInOut(  pData[i] );
   }
   
}

void radio_hal_SpiReadData(U8 byteCount, U8* pData)
{
// #if (defined SILABS_PLATFORM_RFSTICK) || (defined SILABS_PLATFORM_LCDBB) || (defined SILABS_PLATFORM_WMB)
//   vSpi_ReadDataSpi1(byteCount, pData);
// #else
//   SpiReadData(byteCount, pData);
// #endif
  int i;
  for ( i = 0; i < byteCount; i++)
  {
      pData[i]=SpiInOut(0x00);
  }
  
}

#ifdef RADIO_DRIVER_EXTENDED_SUPPORT
BIT radio_hal_Gpio0Level(void)
{
  BIT retVal = FALSE;

#ifdef SILABS_PLATFORM_DKMB
  retVal = FALSE;
#endif
#ifdef SILABS_PLATFORM_UDP
  retVal = EZRP_RX_DOUT;
#endif
#if (defined SILABS_PLATFORM_RFSTICK) || (defined SILABS_PLATFORM_LCDBB)
  retVal = RF_GPIO0;
#endif
#if (defined SILABS_PLATFORM_WMB930)
  retVal = FALSE;
#endif
#if defined (SILABS_PLATFORM_WMB912)
  #ifdef SILABS_IO_WITH_EXTENDER
    //TODO
    retVal = FALSE;
  #endif
#endif

  return retVal;
}

BIT radio_hal_Gpio1Level(void)
{
  BIT retVal = FALSE;

#ifdef SILABS_PLATFORM_DKMB
  retVal = FSK_CLK_OUT;
#endif
#ifdef SILABS_PLATFORM_UDP
  retVal = FALSE; //No Pop
#endif
#if (defined SILABS_PLATFORM_RFSTICK) || (defined SILABS_PLATFORM_LCDBB) || (defined SILABS_PLATFORM_WMB930)
  retVal = RF_GPIO1;
#endif
#if defined (SILABS_PLATFORM_WMB912)
  #ifdef SILABS_IO_WITH_EXTENDER
    //TODO
    retVal = FALSE;
  #endif
#endif

  return retVal;
}

BIT radio_hal_Gpio2Level(void)
{
  BIT retVal = FALSE;

#ifdef SILABS_PLATFORM_DKMB
  retVal = DATA_NFFS;
#endif
#ifdef SILABS_PLATFORM_UDP
  retVal = FALSE; //No Pop
#endif
#if (defined SILABS_PLATFORM_RFSTICK) || (defined SILABS_PLATFORM_LCDBB) || (defined SILABS_PLATFORM_WMB930)
  retVal = RF_GPIO2;
#endif
#if defined (SILABS_PLATFORM_WMB912)
  #ifdef SILABS_IO_WITH_EXTENDER
    //TODO
    retVal = FALSE;
  #endif
#endif

  return retVal;
}

BIT radio_hal_Gpio3Level(void)
{
  BIT retVal = FALSE;

#if (defined SILABS_PLATFORM_RFSTICK) || (defined SILABS_PLATFORM_LCDBB) || (defined SILABS_PLATFORM_WMB930)
  retVal = RF_GPIO3;
#elif defined (SILABS_PLATFORM_WMB912)
  #ifdef SILABS_IO_WITH_EXTENDER
    //TODO
    retVal = FALSE;
  #endif
#endif

  return retVal;
}

#endif
