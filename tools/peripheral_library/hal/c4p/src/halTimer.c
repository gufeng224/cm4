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
/// @file   halTimer.c
/// @brief  MStar Timer HAL Driver DDI
/// @author MStar Semiconductor Inc.
/// @attention
/// <b><em></em></b>
///////////////////////////////////////////////////////////////////////////////////////////////////

#define _HAL_TIMER_C

#include "halTimer.h"

/* Timer register address definition */
/* Timer 0 */
#define TIMER_BASE_ADDR     REG_ADDR_BASE_TIMER1
#define TIMER_CTRL_ADDR     GET_REG16_ADDR(TIMER_BASE_ADDR, 0x00)
#define TIMER_HIT_ADDR      GET_REG16_ADDR(TIMER_BASE_ADDR, 0x01)
#define TIMER_MAX_L_ADDR    GET_REG16_ADDR(TIMER_BASE_ADDR, 0x02)
#define TIMER_MAX_H_ADDR    GET_REG16_ADDR(TIMER_BASE_ADDR, 0x03)
#define TIMER_CAPCNT_L_ADDR GET_REG16_ADDR(TIMER_BASE_ADDR, 0x04)
#define TIMER_CAPCNT_H_ADDR GET_REG16_ADDR(TIMER_BASE_ADDR, 0x05)

#define TIMER1_BASE_ADDR     REG_ADDR_BASE_TIMER1
#define TIMER1_CTRL_ADDR     GET_REG16_ADDR(TIMER1_BASE_ADDR, 0x00)
#define TIMER1_HIT_ADDR      GET_REG16_ADDR(TIMER1_BASE_ADDR, 0x01)
#define TIMER1_MAX_L_ADDR    GET_REG16_ADDR(TIMER1_BASE_ADDR, 0x02)
#define TIMER1_MAX_H_ADDR    GET_REG16_ADDR(TIMER1_BASE_ADDR, 0x03)
#define TIMER1_CAPCNT_L_ADDR GET_REG16_ADDR(TIMER1_BASE_ADDR, 0x04)
#define TIMER1_CAPCNT_H_ADDR GET_REG16_ADDR(TIMER1_BASE_ADDR, 0x05)

#if 0
void timer_reset(void)
{
    REG(TIMER_CTRL_ADDR)=0;
    REG(TIMER_CTRL_ADDR)=0x1;
}

U32 timer_get_count(void)
{

    // 1 tick = 1/OSC1 sec
    U32 val=0;
    val = (REG(TIMER_CAPCNT_L_ADDR)& 0xFFFF);
    val +=  (REG(TIMER_CAPCNT_H_ADDR)<<16);
    return val;
}

U32 timer_get_max(void)
{
    // 1 tick = 1/OSC1 sec
    U32 val=0;

    val = (REG(TIMER_MAX_L_ADDR)& 0xFFFF);
    val +=  (REG(TIMER_MAX_H_ADDR)<<16);

    return val;
}


//void timer_stop(void)
//{
//    REG(TIMER_CTRL_ADDR)=0;
//}

void loop_delay_timer(unsigned long count)
{
    U32 st0=0;
    U32 tick_count=0;
    U32 st1=0;

    st0=timer_get_count();


    //uart_write_U32_hex(count);
    if(0==count)
    {
    return;
    }

    while(1)
    {

    st1=timer_get_count();

    if(st1<st0)
    {

    tick_count+=((timer_get_max()-st0) + st1);
    }
    else
    {
    tick_count+=(st1-st0);
    }
    st0=st1;

    if(tick_count>=count)
    {
    break;
    }

    }
}
#endif

void MHal_Timer_Reset(void)
{
    REG(TIMER_CTRL_ADDR)=0;
    REG(TIMER_CTRL_ADDR)=0x1;
}

U32 MHal_Timer_GetTick(void)
{
    // 1 tick = 1/OSC1 sec
    U32 val=0;
    val = (REG(TIMER_CAPCNT_L_ADDR)& 0xFFFF);
    val +=  (REG(TIMER_CAPCNT_H_ADDR)<<16);
    return val;
}

U32 MHal_Timer_GetMax(void)
{
    // 1 tick = 1/OSC1 sec
    U32 val=0;
    val = (REG(TIMER_MAX_L_ADDR)& 0xFFFF);
    val +=  (REG(TIMER_MAX_H_ADDR)<<16);
    return val;
}


void MHal_Timer_Delay(U32 count)
{
    U32 st0=0;
    U32 tick_count=0;
    U32 st1=0;

    st0=MHal_Timer_GetTick();


    //uart_write_U32_hex(count);
    if(0==count)
    {
    return;
    }

    while(1)
    {

    st1=MHal_Timer_GetTick();

    if(st1<st0)
    {

    tick_count+=((MHal_Timer_GetMax()-st0) + st1);
    }
    else
    {
    tick_count+=(st1-st0);
    }
    st0=st1;

    if(tick_count>=count)
    {
    break;
    }

    }
}


#if 0

#define HAL_OSTICK2_STOP() \
    do \
    { \
        g_ptKeInfinteTimer2->reg_timer_en = 0x00; \
    } while(0)

#define HAL_OSTICK2_RUN(tick) \
    do \
    { \
        g_ptKeInfinteTimer2->reg_timer_int_en = (tick == HAL_GLOBAL_TIMER_MAX_COUNT) ? 0x00 : 0x01; \
        g_ptKeInfinteTimer2->reg_timer_max_low = ((tick)&0xFFFF); \
        g_ptKeInfinteTimer2->reg_timer_max_high = ((tick>>16)&0xFFFF); \
        g_ptKeInfinteTimer2->reg_timer_en = 0x01; \
     } while(0)


U32 MHal_TIM_GetTimeoutLen(void)
{
    // 1 tick = 1/OSC1 sec
    U32 val=0;

    val = (REG(TIMER_MAX_L_ADDR)& 0xFFFF);
    val +=  (REG(TIMER_MAX_H_ADDR)<<16);

    return val;
}

#define HAL_OSTICK2_GET_TIMEOUTLENGTH() ((g_ptKeInfinteTimer2->reg_timer_max_low + (g_ptKeInfinteTimer2->reg_timer_max_high<<16)))
#define HAL_OSTICK2_GET_COUNT() ((g_ptKeInfinteTimer2->reg_timer_cap_low) + (g_ptKeInfinteTimer2->reg_timer_cap_high<<16))

#define HAL_OSTICK2_INT_FLAG()  (g_ptKeInfinteTimer2->reg_timer_hit)

#define HAL_OSTICK2_INT_CLEAR() \
    do \
    { \
        g_ptKeInfinteTimer2->reg_timer_hit = 1; \
    } while(0)

#define HAL_OSTICK2_INT_ENABLE() \
    do \
    { \
        g_ptKeInfinteTimer2->reg_timer_int_en = 0x01; \
    } while(0)

#define HAL_OSTICK2_INT_DISABLE() \
    do \
    { \
        g_ptKeInfinteTimer2->reg_timer_int_en = 0x0; \
    } while(0)

U64 __u64OsTickCount;
U64 DrvTimerGetOsTick(void)
{
    U64 rtk_systemTicks;
    U64 counter, length;
    U64 tick, old_tick;

    length = DrvTimerGlobalTimerGetTimeoutLength();
    tick = __u64OsTickCount;

    do
    {
        old_tick = tick;
        counter = DrvTimerGlobalTimerGetTick();
        tick = __u64OsTickCount;
    }
    while(tick != old_tick);

    rtk_systemTicks = tick * (length + 1);
    rtk_systemTicks += counter;

    return rtk_systemTicks;
}
#endif
