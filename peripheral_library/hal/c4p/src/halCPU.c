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

#define REG_CM4_CLK        GET_REG16_ADDR(REG_ADDR_BASE_CLKGEN, 0x5C)
#define REG_CM4_FROPLL     GET_REG16_ADDR(REG_ADDR_BASE_MISC_FROPLL, 0x33)
#define REG_CM4_FROPLL_PD     GET_REG16_ADDR(REG_ADDR_BASE_MISC_FROPLL, 0x31)
#define REG_CM4_FROPLL_MUX     GET_REG16_ADDR(REG_ADDR_BASE_MISC_FROPLL, 0x39)

void MHal_CPU_Init(void)
{
  //CLRREG16(REG_CM4_FROPLL_MUX, 0x1);
  SETREG16(REG_CM4_FROPLL_MUX, 0x1);
  OUTREG16(REG_CM4_FROPLL, 0x0026);// clock 26/24M*0x22/2 ->456M
  //OUTREG16(REG_CM4_FROPLL, 0x002A);//504M
  //OUTREG16(REG_CM4_FROPLL, 0x0018);//288M
}

/*
clk_cm4_core clock setting
[0]: disable clock
[1]: invert clock
[3:2]: Select clock source
     0xx: xtal div2 (gf_mux)
     100:  clk_fro
     101:  clk_fropll
     110:  1'b0 //442M mpll
     111:  xtal div2
*/

void MHal_CPU_SetFreq(CPU_FREQ_e eFreq)
{
  U16 u16Val;
  switch(eFreq)
  {
    case E_CPU_FREQ_LOW:
      u16Val = INREG16(REG_CM4_CLK);

      CLRREG16(REG_CM4_CLK, (0x1<<4));
      OUTREG16(REG_CM4_CLK, (u16Val & 0xFFF0) | 0x0);//clk_fro 24MHz
      SETREG16(REG_CM4_CLK, (0x1<<4));

      SETREG16(REG_CM4_FROPLL_PD, (0x1<<8)); //power down FRO PLL

      break;
    case E_CPU_FREQ_HIGH:

      CLRREG16(REG_CM4_FROPLL_PD, (0x1<<8)); //power on FRO PLL
      MOS_uDelay(500);

      CLRREG16(REG_CM4_CLK, (0x1<<4));
      u16Val = INREG16(REG_CM4_CLK);
      OUTREG16(REG_CM4_CLK, (u16Val & 0xFFF0) | 0x4);
      SETREG16(REG_CM4_CLK, (0x1<<4));

        break;
    default:
        break;
  }
}

