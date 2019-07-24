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
///
/// file    drvMBX.c
/// @brief  MStar MailBox DDI
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

#define _DRV_AUDIO_C
/* Standard includes. */
#include <string.h>

/* Kernel includes. */
#include "mosWrapper.h"

/* driver includes. */
#include "drvAudio.h"
#include "halAudio.h"
#include "halUART.h"
#include "halRTC.h"
#include "halTimer.h"

//#define CONFIG_AUD_I2S //move to makefile
/*-----------------------------------------------------------*/

#define AUDIO_UNDERRUN_MS 10 //10 ms
#define AUDIO_UNDERRUN_SIZE (AUDIO_DMA_WR_SAMPLE_RATE * 2 * AUDIO_DMIC_NO * AUDIO_UNDERRUN_MS/1000)

#if defined(CONFIG_VOC_DKWS)
#define AUDIO_OVERRUN_MS (AUDIO_DMA_BUFFER_PERIOD_MS/4) //1/4 buffer size
#else
#define AUDIO_OVERRUN_MS (AUDIO_DMA_BUFFER_PERIOD_MS/12) //1/12 buffer size
#endif

#define AUDIO_OVERRUN_SIZE  (AUDIO_DMA_WR_SAMPLE_RATE * 2 * AUDIO_DMIC_NO * AUDIO_OVERRUN_MS/1000)

//#define AUDIO_WAIT_OVERRUN_TIME pdMS_TO_TICKS( AUDIO_OVERRUN_MS*2 )
#define AUDIO_WAIT_OVERRUN_TIME portMAX_DELAY

#define AUDIO_INT_PRIORITY		( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1 )
#define AUDIO_VAD_INT_PRIORITY		( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY - 1 )

/*-----------------------------------------------------------*/

#define AUD_MSG(fmt, args...) MOS_DBG_PRINT(MOS_DBG_LEVEL_AUD, "[AUD] " fmt, ##args)
#define AUD_ERROR(fmt, args...) MOS_DBG_ERROR("[AUD] " fmt, ##args);

typedef enum
{
  E_AUD_STATE_NONE = 0,
  E_AUD_STATE_INIT,
  E_AUD_STATE_START,
  E_AUD_STATE_STOP,
  E_AUD_STATE_XRUN
} AUD_State_e;

static SemaphoreHandle_t _tAudIntSem;
static SemaphoreHandle_t _tAudMutex;

#if defined(CONFIG_AUD_VAD)
static Bool _bAudVadEable = FALSE;
static U8 _u8AudVadThr = 0x36;
static U32 _u8AudVadNum = 0;
#endif

static AUD_State_e _eAudState = 0;
static AUD_MIC_e _eAudMicNo = AUD_MIC_NO2;
static AUD_Width_e _eAudWidth = AUD_WIDTH_16;
static AUD_Rate_e _eAudRate = AUD_RATE_16K;
#if defined(CONFIG_AUD_HWGAIN)
//static U16 _u16AudDmaMode = 10; //16k
static U8  _u8AudDmicGain = 0; //16k
#endif
static Bool _bAudGenEable = FALSE;

#if defined(CONFIG_AUD_AMIC)
static Bool _bAudAmicEable = FALSE;
#endif
static U32 _u32AudStartTime = 0;
static U16 _u16AudStartSubTime = 0;
static U32 _u32AudDeltaTime = 0;

__attribute__((section(".dma_buffer_64"))) __attribute__((aligned(64))) static U8 _gMcuDmaBuf[AUDIO_DMA_BUF_SIZE];


#if defined(CONFIG_AUD_I2S)
typedef struct
{
  AudI2sFmt_e eI2sFmt;
  Bool bEnable;
} I2S_Mgr;


static I2S_Mgr _mgrI2S =
{
  .eI2sFmt = AUD_I2S_FMT_I2S,
  .bEnable = FALSE,
};
#endif

#if defined(CONFIG_AUD_AEC)
static Bool _bAudAecEnable = FALSE;
#endif
/*-----------------------------------------------------------*/

#if defined(CONFIG_AUD_VAD)
void NVIC_Handler_AUDIO_VAD(void)
{
#if 0
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  uint32_t nCmd = AUDIO_CMD_START;

  AUD_MSG("NVIC_Handler_AUDIO_VAD\n");

  if(xQueueIsQueueFullFromISR(xAudioQueue) != pdTRUE && nCmd)
  {
    UBaseType_t uxSavedInterruptStatus;
    uxSavedInterruptStatus = portSET_INTERRUPT_MASK_FROM_ISR();
    {
      if(xQueueSendFromISR(xAudioQueue, (void *) &nCmd, &xHigherPriorityTaskWoken) != pdPASS)
      {
        AUD_MSG("NVIC_Handler_AUDIO_VAD: xQueueSendFromISR sent failed");
      }
    }
    portCLEAR_INTERRUPT_MASK_FROM_ISR(uxSavedInterruptStatus);
  }

  /* Call the IntQ test function for this channel. */
  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
#endif

  AUD_MSG("NVIC_Handler_AUDIO_VAD = %d\r\n", _u8AudVadNum);
  _u8AudVadNum++;
  MHAL_AUD_VADClearInt();
}
#endif

void NVIC_Handler_AUDIO(void)
{
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  Bool bFull, bOverrun;
  //uint32_t nCmd = AUDIO_CMD_NONE;

  //AUD_MSG("NVIC_Handler_AUDIO ms = %d\n", xTaskGetTickCount()*portTICK_PERIOD_MS);
  //AUD_MSG("NVIC_Handler_AUDIO ms2 = %d\n", (timer_get_count()/TIMER_DELAY_1ms));

  if (MHAL_AUD_DMAIsRunning())
  {
    MHAL_AUD_DMAGetIntStatus(&bFull, &bOverrun);

    if(bFull)
    {
      //nCmd = AUDIO_CMD_STOP;
      MHAL_AUD_DMAEnableInt(FALSE, FALSE);
      MHAL_AUD_DMAStop();
      MHAL_AUD_DMAClearInt();
      AUD_MSG("AUDIO ISR: FULL (STOP), delta = 0x%x\r\n", _u32AudDeltaTime);
      _u32AudDeltaTime = 0;
      _eAudState = E_AUD_STATE_XRUN;
    }
    else if(bOverrun)
    {
      //nCmd = AUDIO_CMD_READ;
      MHAL_AUD_DMAEnableInt(TRUE, FALSE);
      if(MHAL_AUD_DMAIsRunning())
        xSemaphoreGiveFromISR(_tAudIntSem, &xHigherPriorityTaskWoken);

      //AUD_MSG("AUDIO ISR: OVERRUN, delta = 0x%x\n", _u32AudDeltaTime);
    }
  }

  /* Call the IntQ test function for this channel. */
  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

}

void MDrv_AUD_Init(void)
{
  U32   nOverrunTh;

  // Create the semaphore to guard a shared resource.
  _tAudIntSem = xSemaphoreCreateBinary();
  configASSERT( _tAudIntSem );
  _tAudMutex = xSemaphoreCreateMutex();
  configASSERT( _tAudMutex );

  _eAudState = E_AUD_STATE_INIT;

  MHAL_AUD_Init();

#if defined(CONFIG_AUD_I2S)
  MHAL_AUD_I2SInit();
  //MHAL_AUD_I2SSetSynthRate(_mgrI2S.eRate);
  //MHAL_AUD_I2SSetMode(_mgrI2S.eI2sMode);
  //MHAL_AUD_I2SSetBitWidth(_mgrI2S.eWidth);
  //MHAL_AUD_I2SSetMsMode(_mgrI2S.eMsMode);
  MHAL_AUD_I2SSetFmt(_mgrI2S.eI2sFmt);
#endif
  //MHAL_AUD_DMAReset();
  //MHAL_AUD_VADSetThr(0);
  MDrv_AUD_SetMicNo(_eAudMicNo);

#if defined(CONFIG_VOC_SW_SRC)
  MDrv_AUD_SetSampleRate(AUD_RATE_32K);
#elif defined(CONFIG_VOC_SW_SRC48K)
  MDrv_AUD_SetSampleRate(AUD_RATE_48K);
#else
  MDrv_AUD_SetSampleRate(_eAudRate);
#endif

  MDrv_AUD_SetBitWidth(_eAudWidth);
  //MHAL_AUD_DMASetChn(_eAudDmaChn);
  //MHAL_AUD_DMASetMode(_u16AudDmaMode);
  //MHAL_AUD_DMASetWidth(_eAudDmaWidth);
  MHAL_AUD_DMAEnableSigGen(_bAudGenEable);

  //memset(_gMcuDmaBuf, 0, DMA_BUF_SIZE);
  memset(_gMcuDmaBuf, 0xa, AUDIO_DMA_BUF_SIZE);
  MHAL_AUD_DMASetBufAddr(_gMcuDmaBuf, AUDIO_DMA_BUF_SIZE);
  AUD_MSG("MDrv_AUD_Init memory size = 0x%x, addr = 0x%x\r\n", AUDIO_DMA_BUF_SIZE, _gMcuDmaBuf);

  nOverrunTh = AUDIO_OVERRUN_SIZE;
  MHAL_AUD_DMASetBufOverThr(nOverrunTh);

  MHAL_AUD_DMAEnableInt(FALSE, FALSE);

/*
extern unsigned long __Vectors[];

  __Vectors[AUDIO_IRQn + 16] = (unsigned long)NVIC_Handler_AUDIO;

  NVIC_SetPriority(AUDIO_IRQn, AUDIO_INT_PRIORITY);
  NVIC_ClearPendingIRQ(AUDIO_IRQn);
  NVIC_EnableIRQ(AUDIO_IRQn);
*/

  MOS_IRQn_AttachIsr(NVIC_Handler_AUDIO, AUDIO_IRQn, AUDIO_INT_PRIORITY);
  MOS_IRQn_Unmask(AUDIO_IRQn);

#if defined(CONFIG_AUD_VAD)
  MHAL_AUD_VADSetThr(_u8AudVadThr);
  MHAL_AUD_VADEn(FALSE);
  MOS_IRQn_AttachIsr(NVIC_Handler_AUDIO_VAD, AUDIO_VAD_IRQn, AUDIO_VAD_INT_PRIORITY);
  //MOS_IRQn_Unmask(AUDIO_VAD_IRQn);
#endif

/*
  NVIC_SetPriority(AUDIO_VAD_IRQn, AUDIO_VAD_INT_PRIORITY);
  NVIC_ClearPendingIRQ(AUDIO_VAD_IRQn);
  NVIC_EnableIRQ(AUDIO_VAD_IRQn);

  uIntIsr.pdfNvicIsr = NVIC_Handler_AUDIO_VAD;
  MOS_IRQn_AttachIsr(uIntIsr, AUDIO_VAD_IRQn, AUDIO_VAD_INT_PRIORITY);
  MOS_IRQn_Unmask(AUDIO_VAD_IRQn);

*/
}

void MDrv_AUD_SetSampleRate(AUD_Rate_e eRate)
{
  if( xSemaphoreTake( _tAudMutex, portMAX_DELAY ) == pdPASS )
	{
    switch(eRate)
    {
#if defined(CONFIG_AUD_I2S)
      case AUD_RATE_SLAVE:
        MHAL_AUD_I2SSetSynthRate(AUD_I2S_RATE_8K);
        MHAL_AUD_I2SSetMsMode(AUD_I2S_MSMODE_SLAVE);
        _eAudRate = eRate;
        break;
      case AUD_RATE_96K:
        MHAL_AUD_I2SSetSynthRate(AUD_I2S_RATE_96K);
        MHAL_AUD_I2SSetMsMode(AUD_I2S_MSMODE_MASTER);
        _eAudRate = eRate;
        break;
#endif
      case AUD_RATE_8K:
        MHAL_AUD_DMASetModeExt(0x8000);
        MHAL_AUD_DMASetMode(7);
#if defined(CONFIG_AUD_AMIC)
       if(_bAudAmicEable)
         MHAL_AUD_AdcSetRate(AUD_ADC_RATE_8K);
#endif
#if defined(CONFIG_AUD_I2S)
        MHAL_AUD_I2SSetSynthRate(AUD_I2S_RATE_8K);
        MHAL_AUD_I2SSetMsMode(AUD_I2S_MSMODE_MASTER);
#endif
        _eAudRate = eRate;
        break;
      case AUD_RATE_16K:
        MHAL_AUD_DMASetModeExt(0x8000);
        MHAL_AUD_DMASetMode(11);
        MHAL_AUD_DMicSelPhase(0x0904);//for 16kHz 2.4M
#if defined(CONFIG_AUD_AMIC)
       if(_bAudAmicEable)
         MHAL_AUD_AdcSetRate(AUD_ADC_RATE_16K);
#endif
#if defined(CONFIG_AUD_I2S)
        MHAL_AUD_I2SSetSynthRate(AUD_I2S_RATE_16K);
        MHAL_AUD_I2SSetMsMode(AUD_I2S_MSMODE_MASTER);
#endif
        _eAudRate = eRate;
        break;
      case AUD_RATE_32K:
        MHAL_AUD_DMASetModeExt(0x8000);
        MHAL_AUD_DMASetMode(15);
        MHAL_AUD_DMicSelPhase(0x0502);//for 32kHz 4M
#if defined(CONFIG_AUD_AMIC)
       if(_bAudAmicEable)
         MHAL_AUD_AdcSetRate(AUD_ADC_RATE_32K);
#endif
#if defined(CONFIG_AUD_I2S)
        MHAL_AUD_I2SSetSynthRate(AUD_I2S_RATE_32K);
        MHAL_AUD_I2SSetMsMode(AUD_I2S_MSMODE_MASTER);
#endif
        _eAudRate = eRate;
        break;
      case AUD_RATE_48K:
//#if defined(CONFIG_VOC_SW_SRC48K)
        MHAL_AUD_DMASetModeExt(0x0317);
        MHAL_AUD_DMicSelPhase(0x0904);//for 48kHz 2.4M
//#else
//        MHAL_AUD_DMASetMode(16);
//#endif
#if defined(CONFIG_AUD_AMIC)
       if(_bAudAmicEable)
         MHAL_AUD_AdcSetRate(AUD_ADC_RATE_48K);
#endif
#if defined(CONFIG_AUD_I2S)
        MHAL_AUD_I2SSetSynthRate(AUD_I2S_RATE_48K);
        MHAL_AUD_I2SSetMsMode(AUD_I2S_MSMODE_MASTER);
#endif
        _eAudRate = eRate;
        break;
      default:
        AUD_ERROR("MDrv_AUD_SetSampleRate failed\r\n");
    }
    xSemaphoreGive(_tAudMutex);
  }
}

void MDrv_AUD_SetBitWidth(AUD_Width_e eWidth)
{
  if( xSemaphoreTake( _tAudMutex, portMAX_DELAY ) == pdPASS )
  {
    switch(eWidth)
    {
      case AUD_WIDTH_16:
        MHAL_AUD_DMASetWidth(AUD_DMA_WIDTH16);
#if defined(CONFIG_AUD_I2S)
        MHAL_AUD_I2SSetBitWidth(AUD_I2S_WIDTH16);
#endif
        _eAudWidth = eWidth;
        break;
      case AUD_WIDTH_24:
        MHAL_AUD_DMASetWidth(AUD_DMA_WIDTH24);
#if defined(CONFIG_AUD_I2S)
        MHAL_AUD_I2SSetBitWidth(AUD_I2S_WIDTH24);
#endif
        _eAudWidth = eWidth;
        break;
      case AUD_WIDTH_32:
        MHAL_AUD_DMASetWidth(AUD_DMA_WIDTH32);
#if defined(CONFIG_AUD_I2S)
        MHAL_AUD_I2SSetBitWidth(AUD_I2S_WIDTH32);
#endif
        _eAudWidth = eWidth;
        break;
      default:
        AUD_ERROR("MDrv_AUD_SetBitWidth failed\r\n");
    }
    xSemaphoreGive(_tAudMutex);
  }
}

BOOL MDrv_AUD_SetMicNo(AUD_MIC_e eMicNo)
{
  if(eMicNo == AUD_MIC_MAX)
  {
    AUD_ERROR("MDrv_AUD_SetMICNo failed\r\n");
    return TRUE;
  }

  if (AUDIO_DMIC_NO  < (eMicNo+1)*2)
    return FALSE;

  if( xSemaphoreTake( _tAudMutex, portMAX_DELAY ) == pdPASS )
	{
    switch(eMicNo)
    {
      case AUD_MIC_NO2:
        MHAL_AUD_DMASetChn(AUD_DMA_CHN2);
#if defined(CONFIG_AUD_AMIC)
       if(_bAudAmicEable)
         MHAL_AUD_AdcSetChn(AUD_ADC_CHN2);
#endif
#if defined(CONFIG_AUD_I2S)
        MHAL_AUD_I2SSetChn(AUD_I2S_CHN2);
#endif
        _eAudMicNo = eMicNo;
        break;
      case AUD_MIC_NO4:
        MHAL_AUD_DMASetChn(AUD_DMA_CHN4);
#if defined(CONFIG_AUD_AMIC)
       if(_bAudAmicEable)
         MHAL_AUD_AdcSetChn(AUD_ADC_CHN4);
#endif
#if defined(CONFIG_AUD_I2S)
        MHAL_AUD_I2SSetChn(AUD_I2S_CHN4);
#endif
        _eAudMicNo = eMicNo;
        break;
      case AUD_MIC_NO6:
        MHAL_AUD_DMASetChn(AUD_DMA_CHN6);
#if defined(CONFIG_AUD_AMIC)
       if(_bAudAmicEable)
         return FALSE;
#endif
#if defined(CONFIG_AUD_I2S)
        MHAL_AUD_I2SSetChn(AUD_I2S_CHN8);
#endif
        _eAudMicNo = eMicNo;
        break;
      case AUD_MIC_NO8:
        MHAL_AUD_DMASetChn(AUD_DMA_CHN8);
#if defined(CONFIG_AUD_AMIC)
        if(_bAudAmicEable)
          return FALSE;
#endif
#if defined(CONFIG_AUD_I2S)
        MHAL_AUD_I2SSetChn(AUD_I2S_CHN8);
#endif
        _eAudMicNo = eMicNo;
        break;
      default:
        AUD_ERROR("MDrv_AUD_SetMICNo failed\r\n");
    }
    xSemaphoreGive(_tAudMutex);
  }

  return TRUE;
}

U8 MDrv_AUD_GetMicNo(void)
{
  U8 nChn = 0;
  if( xSemaphoreTake( _tAudMutex, portMAX_DELAY ) == pdPASS )
	{
    switch(_eAudMicNo)
    {
      case AUD_MIC_NO2:
        nChn = 2;
        break;
      case AUD_MIC_NO4:
        nChn = 4;
        break;
      case AUD_MIC_NO6:
        nChn = 6;
        break;
      case AUD_MIC_NO8:
        nChn = 8;
        break;
      default:
        AUD_ERROR("MDrv_AUD_GetMicNo failed\r\n");
    }
    xSemaphoreGive(_tAudMutex);
  }
  return nChn;
}

U8 MDrv_AUD_GetBitWidth(void)
{
  U8 nWidth = 0;
  if( xSemaphoreTake( _tAudMutex, portMAX_DELAY ) == pdPASS )
	{
    switch(_eAudWidth)
    {
      case AUD_WIDTH_16:
        nWidth = 16;
        break;
      case AUD_WIDTH_24:
        nWidth = 24;
        break;
      case AUD_WIDTH_32:
        nWidth = 32;
        break;
      default:
        AUD_ERROR("MDrv_AUD_GetBitWidth failed\r\n");
    }
    xSemaphoreGive(_tAudMutex);
  }
  return nWidth;
}

U32 MDrv_AUD_GetSampleRate(void)
{
  U32 nRate = 0;
  if( xSemaphoreTake( _tAudMutex, portMAX_DELAY ) == pdPASS )
	{
    switch(_eAudRate)
    {
      case AUD_RATE_SLAVE:
        nRate = 0;
        break;
      case AUD_RATE_8K:
        nRate = 8000;
        break;
      case AUD_RATE_16K:
        nRate = 16000;
        break;
      case AUD_RATE_32K:
        nRate = 32000;
        break;
      case AUD_RATE_48K:
        nRate = 48000;
        break;
      case AUD_RATE_96K:
        nRate = 96000;
        break;
      default:
        AUD_ERROR("MDrv_AUD_GetSampleRate failed\r\n");
    }
    xSemaphoreGive(_tAudMutex);
  }
  return nRate;
}

U32 MDrv_AUD_GetBufferLevel(void)
{
  U32 nLevel = 0;
  if( xSemaphoreTake( _tAudMutex, portMAX_DELAY ) == pdPASS )
	{
    if(MHAL_AUD_DMAIsRunning())
      nLevel = MHAL_AUD_DMAGetBufLevel();

    xSemaphoreGive(_tAudMutex);
  }
  return nLevel;
}

#if defined(CONFIG_AUD_HWGAIN)
void MDrv_AUD_DMICSetGain(U8 nStep)
{
  if (nStep >= 8)
    return;

  if( xSemaphoreTake( _tAudMutex, portMAX_DELAY ) == pdPASS )
	{
    _u8AudDmicGain = nStep;

    if (E_AUD_STATE_NONE != _eAudState)
    {
      MHAL_AUD_DMicSetGain(nStep);
    }
    xSemaphoreGive(_tAudMutex);
  }
}
#endif

void MDrv_AUD_SigGenEnable(Bool bEn)
{
  if( xSemaphoreTake( _tAudMutex, portMAX_DELAY ) == pdPASS )
	{
    _bAudGenEable = bEn;

    if (E_AUD_STATE_NONE != _eAudState)
    {
      MHAL_AUD_DMAEnableSigGen(_bAudGenEable);
    }
    xSemaphoreGive(_tAudMutex);
  }
}


BOOL MDrv_AUD_Start(void)
{
  BOOL bRet = FALSE;
  if( xSemaphoreTake( _tAudMutex, portMAX_DELAY ) == pdPASS )
	{
    if(!MHAL_AUD_DMAIsRunning())
    {
      MHAL_AUD_DMAReset();

      taskENTER_CRITICAL();
      AUD_MSG("MDrv_AUD_Start: ms = %d\r\n", xTaskGetTickCount()*portTICK_PERIOD_MS);
      //AUD_MSG("MDrv_AUD_Start ms2 = %d\n", (timer_get_count()/TIMER_DELAY_1ms));
      MHAL_AUD_DMAStart();
      //MHal_RTC_GetTime(&_u32AudStartTime, &_u16AudStartSubTime);
      taskEXIT_CRITICAL();
      //MHAL_AUD_DMAEnableInt(TRUE, FALSE);
      bRet = TRUE;
    }
    _eAudState = E_AUD_STATE_START;
    xSemaphoreGive(_tAudMutex);
  }
  return bRet;
}

void MDrv_AUD_Stop(void)
{
  if( xSemaphoreTake( _tAudMutex, portMAX_DELAY ) == pdPASS )
	{
    if(MHAL_AUD_DMAIsRunning())
    {
      AUD_MSG("MDrv_AUD_Stop\r\n");
      MHAL_AUD_DMAEnableInt(FALSE, FALSE);
      MHAL_AUD_DMAStop();
    }
    _u32AudDeltaTime = 0;
    _eAudState = E_AUD_STATE_STOP;
    xSemaphoreGive(_tAudMutex);
  }
}

BOOL MDrv_AUD_IsXrun(void)
{
    return (_eAudState==E_AUD_STATE_XRUN?TRUE:FALSE);
}

BOOL MDrv_AUD_ReadPCM(S16 *psSampleBuffer, U32 u32SampleCount, U32 *pu32ReturnCount, BOOL bBlock )
{
  //UBaseType_t ret;
  U32 nReadSize;

  U32 u32ReturnSize = 0;
  U32 u32UnderrunSize = AUDIO_UNDERRUN_SIZE;

  U32 u32BufferSize = u32SampleCount * 2; //2 bytes per pcm sample
  U8 *pcBuffer = (U8 *)psSampleBuffer;

  *pu32ReturnCount = 0;

  if (0 == u32BufferSize)
    return FALSE;

  if (u32BufferSize < AUDIO_UNDERRUN_SIZE)
    u32UnderrunSize = u32BufferSize;

  do {
      if( xSemaphoreTake(_tAudMutex, portMAX_DELAY ) == pdPASS )
    	{
        if (!MHAL_AUD_DMAIsRunning())
        {
          if (uxSemaphoreGetCount(_tAudIntSem))
            xSemaphoreTake(_tAudIntSem, 0);
          xSemaphoreGive(_tAudMutex);
          AUD_ERROR("MDrv_AUD_ReadPCM: audio is not active\n");
          return FALSE;
        }
        else if (MHAL_AUD_DMAGetBufLevel() > u32UnderrunSize || !bBlock)
        {
          MHAL_AUD_DMAReadData((U8 *)pcBuffer, u32BufferSize, &nReadSize);
          pcBuffer += nReadSize;
          u32BufferSize -= nReadSize;
          u32ReturnSize +=  nReadSize;
          xSemaphoreGive(_tAudMutex);

          if ((bBlock && (0 == u32BufferSize)) || !bBlock)
          {
            *pu32ReturnCount = u32ReturnSize / 2; //2 bytes per pcm sample
            _u32AudDeltaTime += (*pu32ReturnCount/16)/((_eAudMicNo+1)*2); //FIX me
            return TRUE;
          }
        }
        else
        {
          MHAL_AUD_DMAClearInt();
          MHAL_AUD_DMAEnableInt(TRUE, TRUE);
          xSemaphoreGive(_tAudMutex);

          //AUD_MSG("MDrv_AUD_ReadPCM: uxSemaphoreGetCount = %d\n", uxSemaphoreGetCount(_tAudIntSem));
          if (xSemaphoreTake(_tAudIntSem, AUDIO_WAIT_OVERRUN_TIME) == pdTRUE)
            continue;
          else
          {
            AUD_ERROR("MDrv_AUD_ReadPCM: AUDIO_WAIT_OVERRUN_TIME failed\r\n");
            return FALSE;
          }
        }
      }

   } while(1);
}

#if defined(CONFIG_AUD_VAD)
BOOL MDrv_AUD_VadIsEnable(void)
{
  return _bAudVadEable;
}

void MDrv_AUD_VadEnable(Bool bEn)
{
  //if( xSemaphoreTake( _tAudMutex, portMAX_DELAY ) == pdPASS )
	{
    if (E_AUD_STATE_NONE != _eAudState)
    {
      //AUD_MSG("MDrv_AUD_VadEnable: Enable = %d\n", (U8)bEn);
      if (bEn)
      {
        MOS_IRQn_Unmask(AUDIO_VAD_IRQn);
      }
      else
      {
        MOS_IRQn_Mask(AUDIO_VAD_IRQn);
      }
      MHAL_AUD_VADEn(bEn);
      _bAudVadEable = bEn;
    }
    //xSemaphoreGive(_tAudMutex);
  }
}

void MDrv_AUD_VadSetThr(U8 u8Thr)
{
  //if( xSemaphoreTake( _tAudMutex, portMAX_DELAY ) == pdPASS )
	{
    if (E_AUD_STATE_NONE != _eAudState)
    {
      //AUD_ERROR("MDrv_AUD_VadSetThr: Threshold = %d\n", u8Thr);
      MHAL_AUD_VADSetThr(u8Thr);
      _u8AudVadThr = u8Thr;
    }
    //xSemaphoreGive(_tAudMutex);
  }
}

void MDrv_AUD_VadClear(void)
{
   MHAL_AUD_VADClearInt();
}

void MDrv_AUD_VadSetIsr(IRQn_ISR_t pdfIsr)
{
  if( xSemaphoreTake( _tAudMutex, portMAX_DELAY ) == pdPASS )
	{
    if (E_AUD_STATE_NONE != _eAudState)
    {
      AUD_ERROR("MDrv_AUD_VadSetIsr: isr = 0x%x\r\n", (U32)pdfIsr);
      if (!pdfIsr)
        MOS_IRQn_AttachIsr(NVIC_Handler_AUDIO_VAD, AUDIO_VAD_IRQn, AUDIO_VAD_INT_PRIORITY);
      else
        MOS_IRQn_AttachIsr(pdfIsr, AUDIO_VAD_IRQn, AUDIO_INT_PRIORITY);
    }
    xSemaphoreGive(_tAudMutex);
  }
}
#endif

#if defined(CONFIG_AUD_I2S)
void MDrv_AUD_I2SEnable(Bool bEn)
{
  if( xSemaphoreTake( _tAudMutex, portMAX_DELAY ) == pdPASS )
	{
    _mgrI2S.bEnable = bEn;

    if (E_AUD_STATE_NONE != _eAudState)
    {
      MHAL_AUD_I2SEnable(_mgrI2S.bEnable);
    }
    xSemaphoreGive(_tAudMutex);
  }
}
#endif

#if defined(CONFIG_AUD_AEC)
void MDrv_AUD_AECEnable(Bool bEn)
{
  if( xSemaphoreTake( _tAudMutex, portMAX_DELAY ) == pdPASS )
	{
    _bAudAecEnable = bEn;

    if (E_AUD_STATE_NONE != _eAudState)
    {
      MHAL_AUD_DMAEnableAEC(_bAudAecEnable);
    }
    xSemaphoreGive(_tAudMutex);
  }
}

BOOL MDrv_AUD_AECIsEnable(void)
{
    return _bAudAecEnable;
}
#endif

U64 MDrv_AUD_GetStartTime(void)
{
  AUD_MSG("MDrv_AUD_GetStartTime: RTC = 0x%x(0x%x), delta = 0x%x\r\n", _u32AudStartTime, _u16AudStartSubTime, _u32AudDeltaTime);
  return (U64)_u32AudStartTime*1000000 + (U64)_u16AudStartSubTime*1000000/0x8000 + (U64)_u32AudDeltaTime*1000;
  //return (U64)_u32AudStartTime;
}


#if defined(CONFIG_AUD_AMIC)
void MDrv_AUD_AMicOn(void)
{
  U8 u8MicNo = MDrv_AUD_GetMicNo();
  MHAL_AUD_AMIC(TRUE);

  if(u8MicNo==2)
    MHAL_AUD_AdcSetChn(AUD_ADC_CHN2);
  else
    MHAL_AUD_AdcSetChn(AUD_ADC_CHN4);

  _bAudAmicEable = TRUE;
}

void MDrv_AUD_AMicOff(void)
{
  MHAL_AUD_AMIC(FALSE);
  _bAudAmicEable = FALSE;
}

void MDrv_AUD_AMicAdcGain(U8 u8Level)
{
  MHAL_AUD_AMicAdcGain(u8Level);
}


void MDrv_AUD_AMicMicAmpGain(U8 u8Level)
{
  MHAL_AUD_AMicMicAmpGain(u8Level);
}
#endif

