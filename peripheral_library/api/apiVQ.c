/* Standard includes. */
#include <string.h>
#include <stdio.h>

/* Kernel includes. */
#include "mosWrapper.h"

/* driver includes. */
#include "drvAudio.h"
#include "drvBDMA.h"
#include "drvMBX.h"
#include "drvPM.h"
#include "drvCPU.h"

#include "voc_task.h"
#include "apiVOC.h"

#define VQ_MSG(fmt, args...) MOS_DBG_PRINT(MOS_DBG_LEVEL_VQ, "[VQ] " fmt, ##args)
#define VQ_ERROR(fmt, args...) MOS_DBG_ERROR("[VQ ERR] " fmt, ##args);

#if defined(CONFIG_VOC_SW_VAD)
#define VOC_VAD_TIMER_PERIOD	( ( ( TickType_t ) 5000 ) / portTICK_PERIOD_MS )
static TimerHandle_t _tVocVadTimer = NULL;
static Bool _bVadTimeOut = FALSE;
static SemaphoreHandle_t _tVadMutex = NULL;

#ifdef __SW_VAD_TEST__
static U32 _u32VadRtcStartTime = 0;
static U32 _u32VadRtcTotalTime = 0;
#endif

static void MApi_VOC_VADTimerCb(TimerHandle_t xTimer)
{
#ifdef __SW_VAD_TEST__
  U32 u32VadRtcTime = 0;
  U16 u16VadRtcSubTime = 0;
#endif

  configASSERT(xTimer);

  if( xSemaphoreTake( _tVadMutex, portMAX_DELAY ) == pdPASS )
  {
    _bVadTimeOut = TRUE;

#ifdef __SW_VAD_TEST__
    MHal_RTC_GetTime(&u32VadRtcTime, &u16VadRtcSubTime);
    _u32VadRtcStartTime = u32VadRtcTime*1000 + (U32)u16VadRtcSubTime*1000/0x8000;
#endif

    //TODO: enter low power status
    MDrv_PM_LowPower();
    VQ_MSG("MApi_VOC_VADTimerCb: low power status\r\n");

    xSemaphoreGive(_tVadMutex);
  }
}

void MApi_VOC_VADResetTimer(void)
{
#ifdef __SW_VAD_TEST__
  U32 u32VadRtcTime = 0;
  U16 u16VadRtcSubTime = 0;
  U32 u32VadRtcStartTime = 0;
#endif

  if( xSemaphoreTake( _tVadMutex, portMAX_DELAY ) == pdPASS )
  {
  	if(_tVocVadTimer)
  	{
      xTimerReset(_tVocVadTimer, (TickType_t)0);

      if (_bVadTimeOut)
      {
        _bVadTimeOut = FALSE;

        //TODO: enter full power status
        MDrv_PM_HighPower();

#ifdef __SW_VAD_TEST__
        MHal_RTC_GetTime(&u32VadRtcTime, &u16VadRtcSubTime);
        u32VadRtcStartTime = u32VadRtcTime*1000 + (U32)u16VadRtcSubTime*1000/0x8000;
        _u32VadRtcTotalTime += u32VadRtcStartTime - _u32VadRtcStartTime;
        VQ_MSG("MApi_VOC_VADResetTimer: full power status T = %d\r\n", (U32)_u32VadRtcTotalTime);
#endif

      }
  	}
    xSemaphoreGive(_tVadMutex);
	}
}

void MApi_VOC_VADStartTimer(void)
{
  if (!_tVocVadTimer)
  {
	  _tVocVadTimer = xTimerCreate("Vad", /* A text name, purely to help debugging. */
							VOC_VAD_TIMER_PERIOD,     /* The timer period, which is a multiple of ledFLASH_RATE_BASE. */
							pdFALSE,									/* This is an auto-reload timer, so xAutoReload is set to pdTRUE. */
							(void *)0,					      /* The ID is used to identify the timer within the timer callback function, as each timer uses the same callback. */
							MApi_VOC_VADTimerCb			  /* Each timer uses the same callback. */
						  );

    if(_tVocVadTimer)
  	{
      _tVadMutex = xSemaphoreCreateMutex();
      configASSERT( _tVadMutex );

      xTimerStart(_tVocVadTimer, (TickType_t)0);
      VQ_MSG("MApi_VOC_VADStartTime\r\n");
  	}
    else
      VQ_ERROR("MApi_VOC_VADStartTime: failed\r\n");

  }
}

void MApi_VOC_VADStopTimer(void)
{
  //1. VAD->VQ->STOP
  //2. VAD->STOP
  //3. VAD->ISR->STOP(X)

  if(_tVocVadTimer)
  {
    xTimerStop(_tVocVadTimer, (TickType_t)0);
    xTimerDelete(_tVocVadTimer, (TickType_t)0);
    vSemaphoreDelete(_tVadMutex);
    _tVocVadTimer = NULL;
    _bVadTimeOut = FALSE;
    MDrv_PM_HighPower();
    _tVadMutex = NULL;
    VQ_MSG("MApi_VOC_VADStopTimer\r\n");
  }
}

BOOL MApi_VOC_VADIsEnable(void)
{
  return _tVocVadTimer?TRUE:FALSE;
}

BOOL MApi_VOC_VADIsTimeout(void)
{
  return _bVadTimeOut;
}

__attribute__((weak)) BOOL MApi_VOC_VADPort(S16 *ps16Buffer, U32 u32Count, U32 *pu32Result)
{
  (void) ps16Buffer;
  (void) u32Count;

  *pu32Result = 1;
  return TRUE;
}

#endif

__attribute__((weak)) BOOL MApi_VOC_VQPort(S16 *ps16Buffer, U32 u32Count)
{
  return MApi_VOC_VQPort_RKImpl(ps16Buffer, u32Count);
}

/*-----------------------------------------------------------*/

