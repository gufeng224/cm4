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
/// @file   halCPU.h
/// @brief  MStar CPU HAL DDI
/// @author MStar Semiconductor Inc.
/// @attention
/// <b><em></em></b>
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MHAL_CPU_H
#define _MHAL_CPU_H

//=============================================================================
// Includs
//=============================================================================
#include "boot.h"
#include "platform.h"
#include "std_c.h"

#include "drvCPU.h" //for CPU_Freq_e
//=============================================================================
// Defines & Macros
//=============================================================================


//=============================================================================
// Type and Structure Declaration
//=============================================================================


//=============================================================================
// Function
//=============================================================================
void MHal_CPU_Init(void);
void MHal_CPU_SetFreq (CPU_FREQ_e eFreq);

#endif


