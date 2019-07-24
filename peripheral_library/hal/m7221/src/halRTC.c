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

#define RTC_BASE_ADDR           REG_ADDR_BASE_PM_RTC
#define REG_PMRTC_CTRL          GET_REG16_ADDR(RTC_BASE_ADDR, 0x00)
#define PMRTC_CTRL_READ_EN      (1 << 4)
#define REG_PMRTC_CNT_VAL_L     GET_REG16_ADDR(RTC_BASE_ADDR, 0x0b)
#define REG_PMRTC_CNT_VAL_H     GET_REG16_ADDR(RTC_BASE_ADDR, 0x0c)

#define RTC_MSG(fmt, args...)   MOS_DBG_PRINT(MOS_DBG_LEVEL_RTC, "[RTC] " fmt, ##args)
#define RTC_ERROR(fmt, args...) MOS_DBG_ERROR("[RTC ERR] " fmt, ##args);
void MHal_RTC_GetTime(U32 *pSecCnt, U16 *pSubCnt)
{
  (void)pSecCnt;
  (void)pSubCnt;
}

U32 MHal_RTC_GetTimeSec(void)
{
    //Using PM_RTC0
    U32 secCnt;
    SETREG8(REG_PMRTC_CTRL, PMRTC_CTRL_READ_EN);
    MOS_uDelay(100);
    secCnt = INREG16(REG_PMRTC_CNT_VAL_L) | (INREG16(REG_PMRTC_CNT_VAL_H) << 16);
    //RTC_MSG("MHal_RTC_GetTimeSec: RTC = 0x%x Sec\n", secCnt);

    return secCnt;
}
