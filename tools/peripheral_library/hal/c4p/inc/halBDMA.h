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
/// @file   halBDMA.h
/// @brief  MStar BDMA HAL DDI
/// @author MStar Semiconductor Inc.
/// @attention
/// <b><em></em></b>
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MHAL_BDMA_H
#define _MHAL_BDMA_H

//=============================================================================
// Includs
//=============================================================================
#include "mosWrapper.h"


//typedef enum _BDMA_Dev
//{
//    E_BDMA_DEV_MIU0
//    ,E_BDMA_DEV_MIU1
//    ,E_BDMA_DEV_SEARCH
//    ,E_BDMA_DEV_CRC32
//    ,E_BDMA_DEV_MEM_FILL
//    ,E_BDMA_DEV_FLASH
//    ,E_BDMA_DEV_DMDMCU
//    ,E_BDMA_DEV_VDMCU
//    ,E_BDMA_DEV_DSP
//    ,E_BDMA_DEV_TSP
//    ,E_BDMA_DEV_1KSRAM_HK51
//    ,E_BDMA_DEV_NOT_SUPPORT
//}BDMA_Dev;
//
//typedef enum _BDMA_SrcDev
//{
//    E_BDMA_SRCDEV_MIU0          = E_BDMA_DEV_MIU0
//    ,E_BDMA_SRCDEV_MIU1         = E_BDMA_DEV_MIU1
//    ,E_BDMA_SRCDEV_MEM_FILL     = E_BDMA_DEV_MEM_FILL
//    ,E_BDMA_SRCDEV_FLASH        = E_BDMA_DEV_FLASH
//    ,E_BDMA_SRCDEV_NOT_SUPPORT  = E_BDMA_DEV_NOT_SUPPORT
//}BDMA_SrcDev;
//
//typedef enum _BDMA_DstDev
//{
//    E_BDMA_DSTDEV_MIU0          = E_BDMA_DEV_MIU0
//    ,E_BDMA_DSTDEV_MIU1         = E_BDMA_DEV_MIU1
//    ,E_BDMA_DSTDEV_SEARCH       = E_BDMA_DEV_SEARCH
//    ,E_BDMA_DSTDEV_CRC32        = E_BDMA_DEV_CRC32
//    ,E_BDMA_DSTDEV_DMDMCU       = E_BDMA_DEV_DMDMCU         //Demod
//    ,E_BDMA_DSTDEV_VDMCU        = E_BDMA_DEV_VDMCU          //VD
//    ,E_BDMA_DSTDEV_DSP          = E_BDMA_DEV_DSP
//    ,E_BDMA_DSTDEV_TSP          = E_BDMA_DEV_TSP
//    ,E_BDMA_DSTDEV_HK51_1KSRAM  = E_BDMA_DEV_1KSRAM_HK51
//    ,E_BDMA_DSTDEV_NOT_SUPPORT  = E_BDMA_DEV_NOT_SUPPORT
//}BDMA_DstDev;

typedef enum _BDMA_Ch
{
    E_BDMA_CH0,
    E_BDMA_CH1
}BDMA_Ch;

typedef struct _BDMA_ChStatus
{
    BOOL bIsBusy;
    BOOL bIsInt;
    BOOL bIsFound;
}BDMA_ChStatus;


BOOL MHal_BDMA_WaitDone(BDMA_Ch channel,U32 u32Timeoutmsec);
BOOL MHal_BDMA_PollingDone(BDMA_Ch channel);
BOOL MHal_BDMA_FlashToMem(BDMA_Ch channel, U32 u32FlashAddr, U32 u32DramAddr, U32 u32Len);
BOOL MHal_BDMA_CalculateCRC32(BDMA_Ch channel, U32 u32DramAddr, U32 u32Len);
U32 MHal_BDMA_GetCRC32(BDMA_Ch channel);
BOOL MHal_BDMA_MemToMem(BDMA_Ch channel, U32 u32Src, U32 u32Dst, U32 u32Len);
BOOL MHal_BDMA_IMItoIMI(BDMA_Ch channel, U32 u32Src, U32 u32Dst, U32 u32Len);
BOOL MHal_BDMA_IMItoMIU(BDMA_Ch channel, U32 u32Src, U32 u32Dst, U32 u32Len);
BOOL MHal_BDMA_MIUtoIMI(BDMA_Ch channel, U32 u32Src, U32 u32Dst, U32 u32Len);
BOOL MHal_BDMA_CalculateCRC32FromMIU(BDMA_Ch channel, U32 u32DramAddr, U32 u32Len);
BOOL MHal_BDMA_IsBusy(BDMA_Ch channel);
BOOL MHal_BDMA_ClearStatus(BDMA_Ch channel);
void MHal_BDMA_Init(void);
void MHal_BDMA_Resume(void);
#endif
