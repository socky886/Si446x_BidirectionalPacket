#ifndef __DSP__H__
#define __DSP__H__
#include "main.h"
#include <stdint.h>
#include <stdio.h>


#define SEG_FAR
#define SEG_DATA
#define SEG_NEAR
#define SEG_IDATA
#define SEG_XDATA
#define SEG_PDATA
#define SEG_CODE
#define SEG_BDATA



#define TRUE 1
#define FALSE 0


typedef uint8_t BIT;
typedef unsigned char bit;
typedef uint8_t U8;
typedef uint16_t U16;
typedef uint32_t U32;

#define SILABS_RADIO_SI446X
#define RADIO_DRIVER_EXTENDED_SUPPORT


#define BITS(bitArray, bitPos)  BIT bitArray ## bitPos
#define WRITE_TO_BIT_ARRAY(bitArray, byte)  bitArray ## 0 = byte & 0x01; \
                                            bitArray ## 1 = byte & 0x02; \
                                            bitArray ## 2 = byte & 0x04; \
                                            bitArray ## 3 = byte & 0x08; \
                                            bitArray ## 4 = byte & 0x10; \
                                            bitArray ## 5 = byte & 0x20; \
                                            bitArray ## 6 = byte & 0x40; \
                                            bitArray ## 7 = byte & 0x80;

#define READ_FROM_BIT_ARRAY(bitArray, byte) byte =  (bitArray ## 0) | \
                                                   ((bitArray ## 1) << 1) | \
                                                   ((bitArray ## 2) << 2) | \
                                                   ((bitArray ## 3) << 3) | \
                                                   ((bitArray ## 4) << 4) | \
                                                   ((bitArray ## 5) << 5) | \
                                                   ((bitArray ## 6) << 6) | \
                                                   ((bitArray ## 7) << 7);


#define SEGMENT_VARIABLE(name, vartype, locsegment) vartype locsegment name
#define VARIABLE_SEGMENT_POINTER(name, vartype, targsegment) vartype targsegment * name
#define SEGMENT_VARIABLE_SEGMENT_POINTER(name, vartype, targsegment, locsegment) vartype targsegment * locsegment name
#define SEGMENT_POINTER(name, vartype, locsegment) vartype * locsegment name
#define LOCATED_VARIABLE(name, vartype, locsegment, addr, init) locsegment vartype name
#define LOCATED_VARIABLE_NO_INIT(name, vartype, locsegment, addr) locsegment vartype name


#include "radio_hal.h"
#include "radio_comm.h"

#ifdef SILABS_RADIO_SI446X
#include "si446x_api_lib.h"
#include "si446x_defs.h"
#include "si446x_nirq.h"
#include "si446x_patch.h"
#endif

#ifdef WHITEN_YES
#include "radio_config_whiten.h"
#else
#include "radio_config_no_whiten.h"
#endif

#endif