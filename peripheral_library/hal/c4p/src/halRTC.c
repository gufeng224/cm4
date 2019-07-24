////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2008-2009 MStar Semiconductor, Inc.
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
///
/// file    halRTC.c
/// @brief  MStar RTC DDI
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

#define _MHAL_RTC_C

//=============================================================================
// Include Files
//=============================================================================
#include "mosWrapper.h"
#include "halRTC.h"

#define RTC_BASE_ADDR      REG_ADDR_BASE_RTC

#define REG_RTC_CTRL        (0x00)
    #define LOAD_EN_BIT     BIT1
    #define RSTZ_BIT        BIT2
    #define PG_RST          BIT3
    #define CNT_EN          BIT4

#define REG_RTC_LOAD_VAL_L  (0x01)
#define REG_RTC_LOAD_VAL_H  (0x02)

#define REG_RTC_MATCH_VAL_L (0x08)
#define REG_RTC_MATCH_VAL_H (0x09)

#define REG_RTC_CNT_VAL_L   (0x03)
#define REG_RTC_CNT_VAL_H   (0x04)
#define REG_RTC_SUB_CNT_VAL (0x05)

#define REG_RTC_RAW_CNT_VAL_L   (0x0B)
#define REG_RTC_RAW_CNT_VAL_H   (0x0C)
#define REG_RTC_RAW_SUB_CNT_VAL (0x0D)

#define REG_RTC_SNAPSHOT    (0x0E)
    #define ACT_SNAPSHOT    BIT0

#define REG_RTC_IRQ_MASK    (0x2B)
#define REG_RTC_IRQ_CLR     (0x2D)
    #define SUB_CNT_MATCH_INT   BIT1

#define REG_RTC_INT_PASSWD  (0x2F)


#define RTC_MSG(fmt, args...) MOS_DBG_PRINT(MOS_DBG_LEVEL_RTC, "[RTC] " fmt, ##args)
#define RTC_ERROR(fmt, args...) MOS_DBG_ERROR("[RTC ERR] " fmt, ##args);


void MHal_RTC_GetTime(U32 *pSecCnt, U16 *pSubCnt)
{
    *pSecCnt = *pSubCnt = 0;
    SETREG16(GET_REG16_ADDR(RTC_BASE_ADDR, REG_RTC_SNAPSHOT), ACT_SNAPSHOT);
    //while(INREG16(GET_REG16_ADDR(RTC_BASE_ADDR, REG_RTC_SNAPSHOT)) & ACT_SNAPSHOT);  //wait for HW latch done
    //while(!(INREG16(GET_REG16_ADDR(RTC_BASE_ADDR, REG_RTC_SNAPSHOT)) & ACT_SNAPSHOT));  //wait for HW latch done

    MOS_uDelay(200);

    *pSecCnt = INREG16(GET_REG16_ADDR(RTC_BASE_ADDR, REG_RTC_CNT_VAL_L)) | (INREG16(GET_REG16_ADDR(RTC_BASE_ADDR, REG_RTC_CNT_VAL_H)) << 16);
    *pSubCnt = INREG16(GET_REG16_ADDR(RTC_BASE_ADDR, REG_RTC_SUB_CNT_VAL));
}

#if 0
void MHal_RTC_GetTime2(U32 *pSecCnt, U16 *pSubCnt)
{
    *pSecCnt = *pSubCnt = 0;
    //SETREG16(GET_REG16_ADDR(RTC_BASE_ADDR, REG_RTC_SNAPSHOT), ACT_SNAPSHOT);
    //while(INREG16(GET_REG16_ADDR(RTC_BASE_ADDR, REG_RTC_SNAPSHOT)) & ACT_SNAPSHOT);  //wait for HW latch done

    //*pSecCnt = INREG16(GET_REG16_ADDR(RTC_BASE_ADDR, REG_RTC_RAW_CNT_VAL_L)) | (INREG16(GET_REG16_ADDR(RTC_BASE_ADDR, REG_RTC_RAW_CNT_VAL_H)) << 16);
    //*pSubCnt = INREG16(GET_REG16_ADDR(RTC_BASE_ADDR, REG_RTC_RAW_SUB_CNT_VAL));
}


U64 MHal_RTC_GetTimeUs(void)
{
    U32 secCnt, subCnt;

    SETREG16(GET_REG16_ADDR(RTC_BASE_ADDR, REG_RTC_SNAPSHOT), ACT_SNAPSHOT);
    while(INREG16(GET_REG16_ADDR(RTC_BASE_ADDR, REG_RTC_SNAPSHOT)) & ACT_SNAPSHOT);  //wait for HW latch done
    while(!(INREG16(GET_REG16_ADDR(RTC_BASE_ADDR, REG_RTC_SNAPSHOT)) & ACT_SNAPSHOT));  //wait for HW latch done

    MHal_Timer_Delay(TIMER_DELAY_100us);//

    secCnt = INREG16(GET_REG16_ADDR(RTC_BASE_ADDR, REG_RTC_CNT_VAL_L)) | (INREG16(GET_REG16_ADDR(RTC_BASE_ADDR, REG_RTC_CNT_VAL_H)) << 16);
    subCnt = INREG16(GET_REG16_ADDR(RTC_BASE_ADDR, REG_RTC_SUB_CNT_VAL));

    RTC_MSG("MHal_RTC_GetTimeUs: RTC = 0x%x s (0x%x ms)\n", secCnt, subCnt*1000/0x8000);
    return (U64)secCnt*1000000 + (U64)subCnt*1000000/0x8000;
}
#endif

