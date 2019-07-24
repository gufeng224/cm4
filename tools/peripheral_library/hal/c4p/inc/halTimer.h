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
/// @file   halPM.h
/// @brief  MStar RTC HAL DDI
/// @author MStar Semiconductor Inc.
/// @attention
/// <b><em></em></b>
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MHAL_TIMER_H
#define _MHAL_TIMER_H

//=============================================================================
// Includs
//=============================================================================
#include "boot.h"
#include "platform.h"
#include "std_c.h"

// U32 timer_get_OSC(void);
void MHal_Timer_Reset(void);
U32 MHal_Timer_GetTick(void);
void MHal_Timer_Delay(unsigned long count);

#define TIMER_CLOCK           TIMER_OSC           // 1 sec = TIMER_CLOCK ticks

#define TIMER_DELAY_1s        (TIMER_OSC / 1)
#define TIMER_DELAY_500ms     (TIMER_OSC / 2)
#define TIMER_DELAY_250ms     (TIMER_OSC / 4)
#define TIMER_DELAY_100ms     (TIMER_OSC / 10)
#define TIMER_DELAY_10ms      (TIMER_OSC / 100)
#define TIMER_DELAY_5ms       (TIMER_OSC / 200)
#define TIMER_DELAY_2500us    (TIMER_OSC / 400)
#define TIMER_DELAY_2ms       (TIMER_OSC / 500)
#define TIMER_DELAY_1ms       (TIMER_OSC / 1000)
#define TIMER_DELAY_500us     (TIMER_OSC / 2000)
#define TIMER_DELAY_250us     (TIMER_OSC / 4000)
#define TIMER_DELAY_100us     (TIMER_OSC / 10000)
#define TIMER_DELAY_10us      (TIMER_OSC / 100000)
#define TIMER_DELAY_5us       (TIMER_OSC / 200000)
#define TIMER_DELAY_2us       (TIMER_OSC / 500000)
#define TIMER_DELAY_1us       (TIMER_OSC / 1000000)

#endif
