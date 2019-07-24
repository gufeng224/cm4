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
/// @file   halCPUINT.h
/// @brief  MStar CPU Interrupt HAL DDI
/// @author MStar Semiconductor Inc.
/// @attention
/// <b><em></em></b>
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MHAL_CPUINT_H
#define _MHAL_CPUINT_H

//=============================================================================
// Includs
//=============================================================================
#include "mosWrapper.h"

//=============================================================================
// Defines & Macros
//=============================================================================

typedef enum
{
//   E_CPUINT_PM = 0,
   E_CPUINT_HK0 = 1,
   E_CPUINT_HK1 = 2,
   E_CPUINT_CP  = 0,
   E_CPUINT_MAX = 4,
}CPUINT_ID_e;


//=============================================================================
// Type and Structure Declaration
//=============================================================================


//=============================================================================
// Function
//=============================================================================
void MHal_CPUINT_Fire (CPUINT_ID_e eDstCPUID, CPUINT_ID_e eSrcCPUID);
void MHal_CPUINT_Clear (CPUINT_ID_e eDstCPUID, CPUINT_ID_e eSrcCPUID);

#endif


