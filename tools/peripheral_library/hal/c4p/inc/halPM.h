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
/// @brief  MStar Power Management HAL DDI
/// @author MStar Semiconductor Inc.
/// @attention
/// <b><em></em></b>
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MHAL_PM_H
#define _MHAL_PM_H

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
void Mhal_PM_Suspend(void);
void Mhal_PM_Resume(void);
void Mhal_PM_AutoReset(void);
void Mhal_PM_SelfReset(void);
void Mhal_PM_GPIO_OEN(U8 nGPIO, U8 nOEN);
void Mhal_PM_GPIO_PULL(U8 nGPIO, U8 nHi);
BOOL Mhal_PM_GPIO_IN(U8 nGPIO);
#endif


