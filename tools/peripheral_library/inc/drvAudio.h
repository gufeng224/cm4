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
/// @brief  MStar Audio Driver Driver DDI
/// @author MStar Semiconductor Inc.
/// @attention
/// <b><em></em></b>
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _DRV_AUD_H
#define _DRV_AUD_H

#include "boot.h"
#include "platform.h"
#include "std_c.h"

//-------------------------------------------------------------------------------------------------
// Defines
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
// Macros
//-------------------------------------------------------------------------------------------------
#define AUDIO_DMA_WR_SAMPLE_RATE         (16000)

#ifndef CONFIG_VOC_MIC
	#define AUDIO_DMIC_NO 4
#else
	#define AUDIO_DMIC_NO	CONFIG_VOC_MIC
#endif

#if defined(CONFIG_VOC_DKWS)
#define AUDIO_DMA_BUFFER_PERIOD_MS       (64) //0.5s
#else
#define AUDIO_DMA_BUFFER_PERIOD_MS       (64*3) //0.5s
#endif

#define AUDIO_DMA_BUF_SIZE (AUDIO_DMA_WR_SAMPLE_RATE * 2 * AUDIO_DMIC_NO * AUDIO_DMA_BUFFER_PERIOD_MS/1000)

//-------------------------------------------------------------------------------------------------
// Type and Structure Declaration
//-------------------------------------------------------------------------------------------------
typedef enum
{
  AUD_MIC_NO2,
  AUD_MIC_NO4,
  AUD_MIC_NO6,
  AUD_MIC_NO8,
  AUD_MIC_MAX
} AUD_MIC_e;

typedef enum
{
  AUD_WIDTH_16,
  AUD_WIDTH_24,
  AUD_WIDTH_32,
  AUD_WIDTH_MAX
}AUD_Width_e;

typedef enum
{
  AUD_RATE_SLAVE,
  AUD_RATE_8K,
  AUD_RATE_16K,
  AUD_RATE_32K,
  AUD_RATE_48K,
  AUD_RATE_96K,
  AUD_RATE_MAX
}AUD_Rate_e;

//-------------------------------------------------------------------------------------------------
// Extern Functions
//-------------------------------------------------------------------------------------------------
void MDrv_AUD_Init(void);
BOOL MDrv_AUD_Start(void);
void MDrv_AUD_Stop(void);
BOOL MDrv_AUD_SetMicNo(AUD_MIC_e eMicNo);
void MDrv_AUD_SetBitWidth(AUD_Width_e eWidth);
void MDrv_AUD_SetSampleRate(AUD_Rate_e eRate);
U8 MDrv_AUD_GetMicNo(void);
U8 MDrv_AUD_GetBitWidth(void);
U32 MDrv_AUD_GetSampleRate(void);
U32 MDrv_AUD_GetBufferLevel(void);
BOOL MDrv_AUD_IsXrun(void);

void MDrv_AUD_DMICSetGain(U8 nStep);
void MDrv_AUD_SigGenEnable(Bool bEn);
BOOL MDrv_AUD_ReadPCM(S16 *psSampleBuffer, U32 u32SampleCount, U32 *pu32ReturnCount, BOOL bBlock );
U64 MDrv_AUD_GetStartTime(void);

BOOL MDrv_AUD_VadIsEnable(void);
void MDrv_AUD_VadEnable(Bool bEn);
void MDrv_AUD_VadSetThr(U8 u8Thr);
void MDrv_AUD_VadClear(void);
void MDrv_AUD_VadSetIsr(IRQn_ISR_t pdfIsr);

U8 MDrv_AUD_DMAGetBitWidth(void);
void MDrv_AUD_I2SEnable(Bool bEn);
void MDrv_AUD_AECEnable(Bool bEn);
BOOL MDrv_AUD_AECIsEnable(void);

#if defined(CONFIG_AUD_AMIC)
void MDrv_AUD_AMicOn(void);
void MDrv_AUD_AMicOff(void);
void MDrv_AUD_AMicAdcGain(U8 u8Level);
void MDrv_AUD_AMicMicAmpGain(U8 u8Level);
#endif


#endif

