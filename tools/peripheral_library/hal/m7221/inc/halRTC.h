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

#ifndef _MHAL_RTC_H
#define _MHAL_RTC_H

//=============================================================================
// Includs
//=============================================================================
#include "boot.h"
#include "platform.h"
#include "std_c.h"

//=============================================================================
// Defines & Macros
//=============================================================================

//=============================================================================
// Type and Structure Declaration
//=============================================================================

//=============================================================================
// Function
//=============================================================================
void MHal_RTC_GetTime(U32 *pSecCnt, U16 *pSubCnt);
U64 MHal_RTC_GetTimeUs(void);
U32 MHal_RTC_GetTimeSec(void);
#endif


