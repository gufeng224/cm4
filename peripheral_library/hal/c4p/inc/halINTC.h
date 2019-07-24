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
/// @file   halINTC.h
/// @brief  MStar Interrupt controller HAL Driver DDI
/// @author MStar Semiconductor Inc.
/// @attention
/// <b><em></em></b>
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _HAL_INTC_H
#define _HAL_INTC_H

#include "boot.h"
#include "platform.h"
#include "std_c.h"

U16 MHal_INTC_IRQStatus(U32 u32IntcNumber);
U16 MHal_INTC_FIQStatus(U32 u32IntcNumber);
void MHal_INTC_FIQClear(U32 u32IntcNumber);
void MHal_INTC_IRQMask(U32 u32IntcNumber);
void MHal_INTC_IRQUnMask(U32 u32IntcNumber);
void MHal_INTC_FIQMask(U32 u32IntcNumber);
void MHal_INTC_FIQUnMask(U32 u32IntcNumber);


#endif //#ifndef _DRVAUDIO_H
