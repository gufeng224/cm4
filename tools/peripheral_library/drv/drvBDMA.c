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
/// @file   drvBDMA.c
/// @brief  MStar BDMA DRV Driver DDI
/// @author MStar Semiconductor Inc.
/// @attention
/// <b><em></em></b>
///////////////////////////////////////////////////////////////////////////////////////////////////

#define _DRV_BDMA_C

/* Kernel includes. */
#include "mosWrapper.h"

/* driver includes. */
#include "drvBDMA.h"
#include "halBDMA.h"

#define BDMA_MSG(fmt, args...) MOS_DBG_PRINT(MOS_DBG_LEVEL_BDMA, "[BDMA]" fmt, ##args)
#define BDMA_ERROR(fmt, args...) MOS_DBG_ERROR(fmt, ##args);


BOOL MDrv_BDMA_Copy(U32 u32Src, U32 u32Dst, U32 u32Len, BDMA_Mode_e eMode)
{
    BDMA_Ch ch;
    switch(eMode)
    {
      case E_BDMA_CPtoHK:
        ch = E_BDMA_CH0;
        MHal_BDMA_IMItoMIU(ch, u32Src, u32Dst, u32Len);
        break;
      case E_BDMA_HKtoCP:
        ch = E_BDMA_CH1;
        MHal_BDMA_MIUtoIMI(ch, u32Src, u32Dst, u32Len);
        break;
      case E_BDMA_CPtoCP:
        ch = E_BDMA_CH0;
        MHal_BDMA_IMItoIMI(ch, u32Src, u32Dst, u32Len);
        break;
      case E_BDMA_HKtoHK:
      	ch = E_BDMA_CH1;
        //MHal_BDMA_MIUtoMIU(E_BDMA_CH0, u32Src, u32Dst, u32Len);
        break;
      default:
        BDMA_ERROR("MDrv_BDMA_Copy: mode %d failed\n", eMode);
        return FALSE;
    }
    MHal_BDMA_PollingDone(ch);

    return TRUE;

}

