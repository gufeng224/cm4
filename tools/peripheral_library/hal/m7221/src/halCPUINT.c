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
/// file    halMBXINT.c
/// @brief  MStar MailBox interrupt DDI
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

#define _MHAL_CPUINT_C

//=============================================================================
// Include Files
//=============================================================================
#include "halCPUINT.h"

//=============================================================================
// Debug Macros
//=============================================================================
#define CPUINT_BASE_ADDR      REG_ADDR_BASE_CPUINT


static U16 _u16CPUINTBitMap[E_CPUINT_MAX][E_CPUINT_MAX] =
                                        { {0xFF, 0x1, 0x2, 0x4},
                                          {0x1, 0xFF, 0x2, 0x4},
                                          {0x1, 0x2, 0xFF, 0x4},
                                          {0x1, 0x2, 0x4, 0xFF}};
//=============================================================================
// Local Function
//=============================================================================

void MHal_CPUINT_Fire (CPUINT_ID_e eDstCPUID, CPUINT_ID_e eSrcCPUID)
{

    if (_u16CPUINTBitMap[eSrcCPUID][eDstCPUID] == 0xFF)
      return;

    SETREG16(GET_REG16_ADDR(CPUINT_BASE_ADDR, eSrcCPUID*2), _u16CPUINTBitMap[eSrcCPUID][eDstCPUID]);
    CLRREG16(GET_REG16_ADDR(CPUINT_BASE_ADDR, eSrcCPUID*2), _u16CPUINTBitMap[eSrcCPUID][eDstCPUID]);

}

void MHal_CPUINT_Clear (CPUINT_ID_e eDstCPUID, CPUINT_ID_e eSrcCPUID)
{
    if (_u16CPUINTBitMap[eSrcCPUID][eDstCPUID] == 0xFF)
      return;

    CLRREG16(GET_REG16_ADDR(CPUINT_BASE_ADDR, eSrcCPUID*2), _u16CPUINTBitMap[eSrcCPUID][eDstCPUID]);
}

