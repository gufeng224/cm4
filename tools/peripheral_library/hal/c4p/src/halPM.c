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

#include "halPowerSaving.h"


void Mhal_PM_Suspend (void)
{
  // step 1.1: reset diamond
  SETREG16(GET_REG16_ADDR(REG_ADDR_BASE_tzpc_nonpm_2nd, 0x1), (0xff));  // 0x306 / 0x1[7:0]=8'hFF

  // step 1.2: enable diamond ISO
  SETREG16(GET_REG16_ADDR(REG_ADDR_BASE_ECBBRIDGE, 0x59), (3 << 12));  //0x10 / 0x59[13:12]=2'b11

  // step 1.3: power off diamond (set PM_GPIO02 = 1'b0)
  CLRREG16(GET_REG16_ADDR(REG_ADDR_BASE_PADTOP, 0x0), (1 << 1)); // set reg_PADTOP.reg_all_pad_in at bit [1] to 1'b0 to allow IO PAD can be configured as output
  CLRREG16(GET_REG16_ADDR(REG_ADDR_BASE_PMU, 0x7d), (1 << 2)); // clear reg_PMU.reg_gpio_g_out bit [2] to 1'b0
  CLRREG16(GET_REG16_ADDR(REG_ADDR_BASE_PMU, 0x7e), (1 << 2)); //clear reg_PMU.reg_gpio_g_oen bit [2] to 1'b0

  Mhal_Power_Saving_Suspend();

  return;
}
void Mhal_PM_Resume (void)
{
  int i;
  U16 u16Val;

  Mhal_Power_Saving_Resume();

  // step 3.1: power on diamond (set PM_GPIO02 = 1'b1)
  SETREG16(GET_REG16_ADDR(REG_ADDR_BASE_PMU, 0x7d), (1 << 2));
  // step 3.2: check VDD_CPU power good
  for(i = 0; i < 20000; i++)
  {
      u16Val = INREG16(GET_REG16_ADDR(REG_ADDR_BASE_PMU, 0x46));

      if(u16Val & (1 << 1)) { //check if bit[1] is 1
          break;
      }
  }
  if((u16Val & (1 << 1)) == 0){ // if bit[1] is 0, indicates polling time out
      //ERROR
  }

  // step 3.3: disable diamond iso
  CLRREG16(GET_REG16_ADDR(REG_ADDR_BASE_ECBBRIDGE, 0x59), (3 << 12)); //0x10 / 0x59[13:12]=2'b00
  // step 3.4: release diamond
  CLRREG16(GET_REG16_ADDR(REG_ADDR_BASE_tzpc_nonpm_2nd, 0x1), (0xff)); //0x306 / 0x1[7:0]=8'h00

  return;
}

void Mhal_PM_SelfReset(void)
{
  U16 u16Val;

  u16Val = INREG16(GET_REG16_ADDR(REG_ADDR_BASE_CHIPTOP, 0x4));

	if(u16Val != 0x2255){
    OUTREG16(GET_REG16_ADDR(REG_ADDR_BASE_CHIPTOP, 0x4), 0x2255);
		//while(*(volatile unsigned int *)XREG_ADDR(bank, CHIP_TOP_RESET_CPU1_OFFSET) != 0x2255) ;
	}
	OUTREG16(GET_REG16_ADDR(REG_ADDR_BASE_CHIPTOP, 0x4), 0x829f);

  return;
}

void Mhal_PM_AutoReset(void)
{
  U16 u16Val;

  u16Val = INREG16(GET_REG16_ADDR(REG_ADDR_BASE_COIN, 0x2));
  // set reg_core_autoup_rst to 1
	OUTREG16(GET_REG16_ADDR(REG_ADDR_BASE_COIN, 0x2), u16Val | 0x1);

  return;
}

void Mhal_PM_GPIO_OEN(U8 nGPIO, U8 nOEN)
{
  INSREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x003ffc), 0x1<<nGPIO, (nOEN? (0x0<<nGPIO): (0x1<<nGPIO)));
}

void Mhal_PM_GPIO_PULL(U8 nGPIO, U8 nHi)
{
  INSREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x003ffa), 0x1<<nGPIO, (nHi? (0x1<<nGPIO): (0x0<<nGPIO)));
}

BOOL Mhal_PM_GPIO_IN(U8 nGPIO)
{
  return EXTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x003f08), 0x1<<nGPIO, nGPIO);
}


