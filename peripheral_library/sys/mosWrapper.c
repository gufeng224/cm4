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
/// @file   mos_wrapper.c
/// @brief  MStar System wrapper source file
/// @author MStar Semiconductor Inc.
/// @attention
/// <b><em></em></b>
///////////////////////////////////////////////////////////////////////////////////////////////////

#define _MOS_WRAPPER_C

/*=============================================================*/
// Include files
/*=============================================================*/
#include "mosWrapper.h"

#include "drvINTC.h"

/*=============================================================*/
// Macro definition
/*=============================================================*/

/*=============================================================*/
// Data type definition
/*=============================================================*/

/*=============================================================*/
// Variable definition
/*=============================================================*/
#ifdef CONFIG_SYS_DBG_LEVEL
U32 gu32MosDbgLevel = CONFIG_SYS_DBG_LEVEL;
#else
U32 gu32MosDbgLevel = MOS_DBG_LEVEL_ALL;
#endif
/*=============================================================*/
// Local function definition
/*=============================================================*/

/*=============================================================*/
// Global function definition
/*=============================================================*/

extern unsigned long __Vectors[];


void MOS_IRQn_AttachIsr(IRQn_ISR_t tIntIsr, IRQn_Type eIntNum, U32 u32Priority)
{
  if (eIntNum == NONE_IRQn)
    return;

  if (eIntNum <= MAX_IRQn)
  {
    __Vectors[eIntNum + 16] = (unsigned long)tIntIsr;

    NVIC_SetPriority(eIntNum, u32Priority);
    NVIC_ClearPendingIRQ(eIntNum);
    //NVIC_EnableIRQ(eIntNum);
  }
  else if ((eIntNum - MAX_IRQn) <= 64)
  {
  	MDrv_INTC_IRQAttach(tIntIsr, eIntNum - MAX_IRQn - 1);
  }
  else
  {
  	MDrv_INTC_FIQAttach(tIntIsr, eIntNum - MAX_IRQn - 1 - 64);
  }
}

void MOS_IRQn_Mask(IRQn_Type eIntNum)
{
  if (eIntNum == NONE_IRQn)
    return;

  if (eIntNum <= MAX_IRQn)
  {
  	NVIC_DisableIRQ(eIntNum);
  }
  else if ((eIntNum - MAX_IRQn) <= 64)
  {
  	MDrv_INTC_IRQSetMask(eIntNum - MAX_IRQn - 1, TRUE);
  }
  else
  {
  	MDrv_INTC_FIQSetMask(eIntNum - MAX_IRQn - 1 - 64, TRUE);
  }
}

void MOS_IRQn_Unmask(IRQn_Type eIntNum)
{
  if (eIntNum == NONE_IRQn)
    return;

  if (eIntNum <= MAX_IRQn)
  {
  	NVIC_EnableIRQ(eIntNum);
  }
  else if ((eIntNum - MAX_IRQn) <= 64)
  {
  	MDrv_INTC_IRQSetMask(eIntNum - MAX_IRQn - 1, FALSE);
  }
  else
  {
  	MDrv_INTC_FIQSetMask(eIntNum - MAX_IRQn - 1 - 64, FALSE);
  }
}

void MOS_mDelay(U32 u32MiniSeconds)
{
  MHal_Timer_Delay(TIMER_DELAY_1ms*u32MiniSeconds);
}

void MOS_uDelay(U32 u32MicroSeconds)
{
  MHal_Timer_Delay(TIMER_DELAY_1us*u32MicroSeconds);
}

void MOS_Debug_EnableMessage(BOOL bEn)
{
  if (bEn)
  {
#ifdef CONFIG_SYS_DBG_LEVEL
    gu32MosDbgLevel = CONFIG_SYS_DBG_LEVEL;
#else
    gu32MosDbgLevel = MOS_DBG_LEVEL_ALL;
#endif
  }
  else
   gu32MosDbgLevel = 0;
}

