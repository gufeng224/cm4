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
/// @file   halINTC.c
/// @brief  MStar Interrupt controller HAL Driver DDI
/// @author MStar Semiconductor Inc.
/// @attention
/// <b><em></em></b>
///////////////////////////////////////////////////////////////////////////////////////////////////

#define _HAL_INTC_C

#include "halINTC.h"

/* Timer register address definition */
/* Timer 0 */
#define INTC_BASE_ADDR        REG_ADDR_BASE_INTC
#define INTC_FIQ_FORCE_15_0   GET_REG16_ADDR(INTC_BASE_ADDR, 0x00)
#define INTC_FIQ_FORCE_31_16  GET_REG16_ADDR(INTC_BASE_ADDR, 0x01)
#define INTC_FIQ_FORCE_47_32  GET_REG16_ADDR(INTC_BASE_ADDR, 0x02)
#define INTC_FIQ_FORCE_63_48  GET_REG16_ADDR(INTC_BASE_ADDR, 0x03)
#define INTC_FIQ_MASK_15_0    GET_REG16_ADDR(INTC_BASE_ADDR, 0x04)
#define INTC_FIQ_MASK_31_16   GET_REG16_ADDR(INTC_BASE_ADDR, 0x05)
#define INTC_FIQ_MASK_47_32   GET_REG16_ADDR(INTC_BASE_ADDR, 0x06)
#define INTC_FIQ_MASK_63_48   GET_REG16_ADDR(INTC_BASE_ADDR, 0x07)

#define INTC_FIQ_STATUS_15_0    GET_REG16_ADDR(INTC_BASE_ADDR, 0x0c)
#define INTC_FIQ_STATUS_31_16   GET_REG16_ADDR(INTC_BASE_ADDR, 0x0d)
#define INTC_FIQ_STATUS_47_32   GET_REG16_ADDR(INTC_BASE_ADDR, 0x0e)
#define INTC_FIQ_STATUS_63_48   GET_REG16_ADDR(INTC_BASE_ADDR, 0x0f)


#define INTC_IRQ_FORCE_15_0   GET_REG16_ADDR(INTC_BASE_ADDR, 0x10)
#define INTC_IRQ_FORCE_31_16  GET_REG16_ADDR(INTC_BASE_ADDR, 0x11)
#define INTC_IRQ_FORCE_47_32  GET_REG16_ADDR(INTC_BASE_ADDR, 0x12)
#define INTC_IRQ_FORCE_63_48  GET_REG16_ADDR(INTC_BASE_ADDR, 0x13)
#define INTC_IRQ_MASK_15_0    GET_REG16_ADDR(INTC_BASE_ADDR, 0x14)
#define INTC_IRQ_MASK_31_16   GET_REG16_ADDR(INTC_BASE_ADDR, 0x15)
#define INTC_IRQ_MASK_47_32   GET_REG16_ADDR(INTC_BASE_ADDR, 0x16)
#define INTC_IRQ_MASK_63_48   GET_REG16_ADDR(INTC_BASE_ADDR, 0x17)

#define INTC_IRQ_STATUS_15_0    GET_REG16_ADDR(INTC_BASE_ADDR, 0x1c)
#define INTC_IRQ_STATUS_31_16   GET_REG16_ADDR(INTC_BASE_ADDR, 0x1d)
#define INTC_IRQ_STATUS_47_32   GET_REG16_ADDR(INTC_BASE_ADDR, 0x1e)
#define INTC_IRQ_STATUS_63_48   GET_REG16_ADDR(INTC_BASE_ADDR, 0x1f)



U16 MHal_INTC_IRQStatus(U32 u32IntcNumber)
{
  if( u32IntcNumber < 64 )
  {
    if((u32IntcNumber/16) == 0)
      return (INREG16(INTC_IRQ_STATUS_15_0) & (1 << (u32IntcNumber%16)));
	  else if((u32IntcNumber/16) == 1)
      return (INREG16(INTC_IRQ_STATUS_31_16) & (1 << (u32IntcNumber%16)));
	  else if((u32IntcNumber/16) == 2)
      return (INREG16(INTC_IRQ_STATUS_47_32) & (1 << (u32IntcNumber%16)));
	  else
		  return (INREG16(INTC_IRQ_STATUS_63_48) & (1 << (u32IntcNumber%16)));
  }
  return 0;
}

U16 MHal_INTC_FIQStatus(U32 u32IntcNumber)
{
  if( u32IntcNumber < 64 )
  {
    if((u32IntcNumber/16) == 0)
      return (INREG16(INTC_FIQ_STATUS_15_0) & (1 << (u32IntcNumber%16)));
	  else if((u32IntcNumber/16) == 1)
      return (INREG16(INTC_FIQ_STATUS_31_16) & (1 << (u32IntcNumber%16)));
	  else if((u32IntcNumber/16) == 2)
      return (INREG16(INTC_FIQ_STATUS_47_32) & (1 << (u32IntcNumber%16)));
	  else
		  return (INREG16(INTC_FIQ_STATUS_63_48) & (1 << (u32IntcNumber%16)));
  }
  return 0;
}


void MHal_INTC_FIQClear(U32 u32IntcNumber)
{
  if( u32IntcNumber < 64 )
  {
    if((u32IntcNumber/16) == 0)
      INREG16(INTC_FIQ_STATUS_15_0) = (1 << (u32IntcNumber%16));
    else if((u32IntcNumber/16) == 1)
      INREG16(INTC_FIQ_STATUS_31_16) = (1 << (u32IntcNumber%16));
    else if((u32IntcNumber/16) == 2)
      INREG16(INTC_FIQ_STATUS_47_32) = (1 << (u32IntcNumber%16));
    else
      INREG16(INTC_FIQ_STATUS_63_48) = (1 << (u32IntcNumber%16));
  }
}


void MHal_INTC_IRQMask(U32 u32IntcNumber)
{
  if( u32IntcNumber < 64 )
  {
    if((u32IntcNumber/16) == 0)
      INREG16(INTC_IRQ_MASK_15_0)  |= (1 << (u32IntcNumber%16));
	  else if((u32IntcNumber/16) == 1)
      INREG16(INTC_IRQ_MASK_31_16) |= (1 << (u32IntcNumber%16));
	  else if((u32IntcNumber/16) == 2)
      INREG16(INTC_IRQ_MASK_47_32) |= (1 << (u32IntcNumber%16));
	  else
		  INREG16(INTC_IRQ_MASK_63_48) |= (1 << (u32IntcNumber%16));
  }
}

void MHal_INTC_IRQUnMask(U32 u32IntcNumber)
{
  if( u32IntcNumber < 64 )
  {
    if((u32IntcNumber/16) == 0)
      INREG16(INTC_IRQ_MASK_15_0)  &= ~(1 << (u32IntcNumber%16));
	  else if((u32IntcNumber/16) == 1)
      INREG16(INTC_IRQ_MASK_31_16) &= ~(1 << (u32IntcNumber%16));
	  else if((u32IntcNumber/16) == 2)
      INREG16(INTC_IRQ_MASK_47_32) &= ~(1 << (u32IntcNumber%16));
	  else
		  INREG16(INTC_IRQ_MASK_63_48) &= ~(1 << (u32IntcNumber%16));
  }
}


void MHal_INTC_FIQMask(U32 u32IntcNumber)
{
  if( u32IntcNumber < 64 )
  {
    if((u32IntcNumber/16) == 0)
      INREG16(INTC_FIQ_MASK_15_0)  |= (1 << (u32IntcNumber%16));
	  else if((u32IntcNumber/16) == 1)
      INREG16(INTC_FIQ_MASK_31_16) |= (1 << (u32IntcNumber%16));
	  else if((u32IntcNumber/16) == 2)
      INREG16(INTC_FIQ_MASK_47_32) |= (1 << (u32IntcNumber%16));
	  else
		  INREG16(INTC_FIQ_MASK_63_48) |= (1 << (u32IntcNumber%16));
  }
}

void MHal_INTC_FIQUnMask(U32 u32IntcNumber)
{
  if( u32IntcNumber < 64 )
  {
    if((u32IntcNumber/16) == 0)
      INREG16(INTC_FIQ_MASK_15_0)  &= ~(1 << (u32IntcNumber%16));
	  else if((u32IntcNumber/16) == 1)
      INREG16(INTC_FIQ_MASK_31_16) &= ~(1 << (u32IntcNumber%16));
	  else if((u32IntcNumber/16) == 2)
      INREG16(INTC_FIQ_MASK_47_32) &= ~(1 << (u32IntcNumber%16));
	  else
		  INREG16(INTC_FIQ_MASK_63_48) &= ~(1 << (u32IntcNumber%16));
  }
}


