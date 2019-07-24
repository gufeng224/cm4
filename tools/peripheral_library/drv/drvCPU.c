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
/// @file   drvCPU.c
/// @brief  MStar CPU DRV Driver DDI
/// @author MStar Semiconductor Inc.
/// @attention
/// <b><em></em></b>
///////////////////////////////////////////////////////////////////////////////////////////////////

#define _DRV_CPU_C

/* driver includes. */
#include "mosWrapper.h"
#include "drvCPU.h"
#include "halCPU.h"

static CPU_FREQ_e _eCPUFreq = E_CPU_FREQ_NONE;

/*-----------------------------------------------------------*/

void MDrv_CPU_SetFreq(CPU_FREQ_e eFreq)
{
  if (_eCPUFreq == E_CPU_FREQ_NONE)
  {
    MHal_CPU_Init();
    MOS_mDelay(1);
  }


  if(_eCPUFreq != eFreq)
  {
    MHal_CPU_SetFreq(eFreq);
    _eCPUFreq = eFreq;
  }
}


