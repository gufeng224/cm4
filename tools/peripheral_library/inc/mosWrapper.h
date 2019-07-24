////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2018 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// ("MStar Confidential Information") by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
/// @file   mosWrapper.c
/// @brief  MStar System wrapper header file
/// @author MStar Semiconductor Inc.
/// @attention
/// <b><em></em></b>
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MOS_WRAPPER_H
#define _MOS_WRAPPER_H

/* Kernel includes. */
#include <string.h>
#include <math.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "semphr.h"
#include "event_groups.h"
#include "stream_buffer.h"

#include "std_c.h"
#include "boot.h"
#include "platform.h"
#include "irq.h"
#include "halUART.h"
#include "halTimer.h"

//-------------------------------------------------------------------------------------------------
// Defines
//-------------------------------------------------------------------------------------------------
/// Define debug level:
/// Default Debug Level: No any Debug Message Print out
#define MOS_DBG_LEVEL_NONE       0x00
#define MOS_DBG_LEVEL_ERROR      0x01
#define MOS_DBG_LEVEL_VOC        0x02
#define MOS_DBG_LEVEL_CMD        0x04
#define MOS_DBG_LEVEL_CRITICAL   0x08

#define MOS_DBG_LEVEL_VP         0x0100
#define MOS_DBG_LEVEL_VQ         0x0200
#define MOS_DBG_LEVEL_VD         0x0400

#define MOS_DBG_LEVEL_AUD        0x00010000
#define MOS_DBG_LEVEL_MBX        0x00020000
#define MOS_DBG_LEVEL_INTC       0x00040000
#define MOS_DBG_LEVEL_CPUINT     0x00080000
#define MOS_DBG_LEVEL_BDMA       0x00100000
#define MOS_DBG_LEVEL_PM         0x00200000
#define MOS_DBG_LEVEL_RTC        0x00400000
#define MOS_DBG_LEVEL_ALL        0xFFFFFFFF


//-------------------------------------------------------------------------------------------------
// Macros
//-------------------------------------------------------------------------------------------------
extern U32 gu32MosDbgLevel;
#define MOS_DBG_PRINT(dbgLevel, fmt, args...)  { \
                                                    if(gu32MosDbgLevel&dbgLevel) \
                                                    { \
                                                        UartSendTrace(fmt, ## args); \
                                                    } \
                                                }

#define MOS_DBG_ERROR(fmt, args...)             { \
                                                    if(gu32MosDbgLevel&MOS_DBG_LEVEL_ERROR) \
                                                    { \
                                                        UartSendTrace(fmt, ## args); \
                                                    } \
                                                }

#define MOS_DBG_SetLevel(level) { \
                                         gu32MosDbgLevel |= level;\
                                       }


#define MOS_DBG_ClearLevel(level) { \
                                           gu32MosDbgLevel &= ~level;\
                                         }



#define STR2(x) #x
#define STR(x) STR2(x)

#define INCBIN(name, file) \
    __asm__(".section .rodata\n" \
            ".global incbin_" STR(name) "_start\n" \
            ".type incbin_" STR(name) "_start, %object\n" \
            ".balign 16\n" \
            "incbin_" STR(name) "_start:\n" \
            ".incbin \"" file "\"\n" \
            \
            ".global incbin_" STR(name) "_end\n" \
            ".type incbin_" STR(name) "_end, %object\n" \
            ".balign 1\n" \
            "incbin_" STR(name) "_end:\n" \
            ".byte 0\n" \
    ); \
    extern const __attribute__((aligned(16))) void* incbin_ ## name ## _start; \
    extern const void* incbin_ ## name ## _end; \

//-------------------------------------------------------------------------------------------------
// Type and Structure Declaration
//-------------------------------------------------------------------------------------------------

typedef void (*IRQn_ISR_t)(void);

//-------------------------------------------------------------------------------------------------
// Extern Functions
//-------------------------------------------------------------------------------------------------

void MOS_IRQn_AttachIsr(IRQn_ISR_t tIntIsr, IRQn_Type eIntNum, U32 u32Priority);
void MOS_IRQn_Mask(IRQn_Type eIntNum);
void MOS_IRQn_Unmask(IRQn_Type eIntNum);
void MOS_mDelay(U32 u32MiniSeconds);
void MOS_uDelay(U32 u32MicroSeconds);

#endif

