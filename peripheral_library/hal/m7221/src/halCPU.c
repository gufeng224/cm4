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
/// file    halCPU.c
/// @brief  MStar MailBox interrupt DDI
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

#define _MHAL_CPU_C

//=============================================================================
// Include Files
//=============================================================================
#include "mosWrapper.h"
#include "halCPU.h"

#define WRIU8(x, y, z) INSREG8(GET_REG8_ADDR(RIU_BASE_ADDR, x), y, z)

void MHal_CPU_Init(void) {}

void MHal_CPU_SetFreq(CPU_FREQ_e eFreq)
{
/*
DC Off
wriu -b 0x0325F8 0x10 0x00

DC ON
wriu -b 0x0325F8 0x10 0x10
*/
  switch(eFreq)
  {
    case E_CPU_FREQ_LOW:
      WRIU8(0x0325F8, 0x10, 0x00);
      //power off XINGGIGPLL
      break;
    case E_CPU_FREQ_HIGH:
      //power on XINGGIGPLL
      //MOS_uDelay(500);
       WRIU8(0x0325F8, 0x10, 0x10);
        break;
    default:
        break;
  }
}

