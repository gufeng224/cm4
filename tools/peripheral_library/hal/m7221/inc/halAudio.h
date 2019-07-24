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
/// @file   halMBX.h
/// @brief  MStar Mailbox HAL Driver DDI
/// @author MStar Semiconductor Inc.
/// @attention
/// <b><em></em></b>
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _HAL_AUD_H
#define _HAL_AUD_H

#include "mosWrapper.h"

#include "drvAudio.h" //for enum definition

/**
 * \brief Audio DMA
 */
typedef enum
{
  AUD_I2S_FMT_I2S,
  AUD_I2S_FMT_LEFT_JUSTIFY
}AudI2sFmt_e;

typedef enum
{
  AUD_DMA_CHN2,
  AUD_DMA_CHN4,
  AUD_DMA_CHN6,
  AUD_DMA_CHN8,
  AUD_DMA_CHN_MAX
} AudDmaChn_e;

typedef enum
{
  AUD_DMA_WIDTH16,
  AUD_DMA_WIDTH24,
  AUD_DMA_WIDTH32,
  AUD_DMA_WIDTH_MAX
}AudDmaWidth_e;

typedef enum
{
  AUD_I2S_WIDTH16,
  AUD_I2S_WIDTH24,
  AUD_I2S_WIDTH32,
  AUD_I2S_WIDTH_MAX
}AudI2SWidth_e;

typedef enum
{
  AUD_I2S_MSMODE_SLAVE,
  AUD_I2S_MSMODE_MASTER,
}AudI2sMsMode_e;

typedef enum
{
  AUD_I2S_MODE_I2S,
  AUD_I2S_MODE_TDM,
}AudI2sMode_e;

typedef enum
{
  AUD_I2S_CHN2,
  AUD_I2S_CHN4,
  AUD_I2S_CHN8,
  AUD_I2S_CHN_MAX
} AudI2sChn_e;


typedef enum
{
  AUD_I2S_RATE_SLAVE,
  AUD_I2S_RATE_8K,
  AUD_I2S_RATE_16K,
  AUD_I2S_RATE_32K,
  AUD_I2S_RATE_48K,
  AUD_I2S_RATE_96K,
  AUD_I2S_RATE_NUM,
  AUD_I2S_RATE_NULL = 0xff
}AudI2SRate_e;

void MHAL_AUD_Init(void);
void MHAL_AUD_DMAReset(void);
void MHAL_AUD_DMASetBufAddr(U8 *pnBufAddr, U32 nBufSize);
void MHAL_AUD_DMASetBufOverThr(U32 nOverrun);

void MHAL_AUD_DMAEnableInt(Bool bFull, Bool bOverrun);
U32 MHAL_AUD_DMAGetBufLevel(void);
void MHAL_AUD_DMAGetIntStatus(Bool *pbFull, Bool *pbOverrun);
void MHAL_AUD_DMAReadData(U8 *pnDestAddr, U32 nDataSize, U32 *pnDataRead);
void MHAL_AUD_DMAStart(void);
void MHAL_AUD_DMAStop(void);
Bool MHAL_AUD_DMAIsRunning(void);

void MHAL_AUD_DMAClearInt(void);
void MHAL_AUD_VADClearInt(void);

void MHAL_AUD_DMAEnableAEC(Bool bEn);
void MHAL_AUD_DMAEnableSigGen(Bool bEn);
void MHAL_AUD_DMASetMode(U16 nMode);
void MHAL_AUD_DMASetModeExt(U16 val);
void MHAL_AUD_DMASetChn(AudDmaChn_e eChn);
void MHAL_AUD_DMASetWidth(AudDmaWidth_e eWidth);
void MHAL_AUD_VADSetThr(U8 nThr);

void MHAL_AUD_DMicSetGain(U8 u8Step);
void MHAL_AUD_DMicSelPhase(U16 u16Sel);

BOOL MHAL_AUD_TDMSetChan(U16 nChannel);
BOOL MHAL_AUD_I2SSetFmt(AudI2sFmt_e eFmt);
BOOL MHAL_AUD_I2SSetBitWidth(AudI2SWidth_e eWidth);
BOOL MHAL_AUD_I2SSetMsMode(AudI2sMsMode_e eMsMode);
BOOL MHAL_AUD_I2SSetMode(AudI2sMode_e eMode);
BOOL MHAL_AUD_I2SSetSynthRate(AudI2SRate_e eRate);
BOOL MHAL_AUD_I2SEnable(BOOL bEnable);
void MHAL_AUD_I2SInit(void);
BOOL MHAL_AUD_I2SUpdate(void);
BOOL MHAL_AUD_I2SSetChn(AudI2sChn_e eChn);

#endif //#ifndef _DRVAUDIO_H
