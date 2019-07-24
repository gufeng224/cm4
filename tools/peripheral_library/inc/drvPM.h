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
/// @file   drvPM.h
/// @brief  MStar Power Management Driver DDI
/// @author MStar Semiconductor Inc.
/// @attention
/// <b><em></em></b>
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _DRV_PM_H
#define _DRV_PM_H

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
typedef enum
{
  E_PM_GPIO0 = 0,
  E_PM_GPIO1,
  E_PM_GPIO2,
  E_PM_GPIO3,
  E_PM_GPIO4,
  E_PM_GPIO5,
  E_PM_GPIO6,
  E_PM_GPIO7,
  E_PM_GPIO8,
  E_PM_GPIO9,
  E_PM_GPIO10,
  E_PM_GPIO11,
  E_PM_GPIO12,
  E_PM_GPIO_MAX,
} PM_GPIO_e;

//-------------------------------------------------------------------------------------------------
// Extern Functions
//-------------------------------------------------------------------------------------------------
void MDrv_PM_Suspend(void);
void MDrv_PM_Resume(void);
void MDrv_PM_HighPower(void);
void MDrv_PM_LowPower(void);

BOOL MDrv_PM_GPIO_OEN(PM_GPIO_e eGPIO, BOOL bOEn, BOOL bInitHigh);

#endif

