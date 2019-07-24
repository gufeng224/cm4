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
/// @file   drvINTC.h
/// @brief  MStar Interrupt Driver DDI
/// @author MStar Semiconductor Inc.
/// @attention
/// <b><em></em></b>
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _DRV_INTC_H
#define _DRV_INTC_H

#include "boot.h"
#include "platform.h"
#include "std_c.h"

//-------------------------------------------------------------------------------------------------
// Defines
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
// Macros
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
// Type and Structure Declaration
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
// Extern Functions
//-------------------------------------------------------------------------------------------------
void MDrv_INTC_Init(void);
void MDrv_INTC_IRQAttach(void *pdfIntcIsr, U32 u32IntcNumber);
void MDrv_INTC_IRQDetach(U32 u32IntcNumber);
void MDrv_INTC_FIQAttach(void *pdfIntcIsr, U32 u32IntcNumber);
void MDrv_INTC_FIQdetach(U32 u32IntcNumber);
void MDrv_INTC_IRQSetMask(U32 u32IntcNumber, BOOL bSetMask);
void MDrv_INTC_FIQSetMask(U32 u32IntcNumber, BOOL bSetMask);

#endif

