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
/// @file   halPM.c
/// @brief  MStar Power Management HAL Driver DDI
/// @author MStar Semiconductor Inc.
/// @attention
/// <b><em></em></b>
///////////////////////////////////////////////////////////////////////////////////////////////////

#define _HAL_PM_C

#include "halPM.h"

#define REG_WK_IRQ_MASK        GET_REG16_ADDR(REG_ADDR_BASE_PM_SLEEP, 0x08)
#define REG_WK_INT             GET_REG16_ADDR(REG_ADDR_BASE_PM_SLEEP, 0x73)

#if 0
static void _WriteReg(U32 nAddr, U16 nValue)
{
  volatile U16 *pnReg = (volatile U16*)(RIU_BASE_ADDR + (nAddr) * 2);
  *pnReg = nValue;
}
#endif

void Mhal_PM_Suspend (void)
{
#if 0
   // dc_off.txt
  _WriteReg(0x0325F4, 0x0199);
  _WriteReg(0x0325F4, 0x0189);
  _WriteReg(0x0325F4, 0x0198);
  _WriteReg(0x0325F4, 0x0198);
  _WriteReg(0x0325F4, 0x0398);
#endif
  return;
}


void Mhal_PM_Resume (void)
{
#if 0
  _WriteReg(0x0325F4, 0x0198);
  _WriteReg(0x0325F4, 0x0188);
  _WriteReg(0x0325F4, 0x0189);
  _WriteReg(0x0325F4, 0x0199);
  _WriteReg(0x0325F4, 0x0399);
#endif
  //0x0E_08[6] = 1'b0 (16 bit address mode)
  //0x0E_73[15] = 1'b1(16 bit address mode)
  CLRREG16(REG_WK_IRQ_MASK, (1<<6));
  SETREG16(REG_WK_INT, (1<<15));
  return;
}

void Mhal_PM_SelfReset(void)
{
  return;
}

void Mhal_PM_AutoReset(void)
{
  return;
}

