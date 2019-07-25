/* Standard includes. */
#include <string.h>
#include <stdio.h>

/* Kernel includes. */
#include "mosWrapper.h"

/* driver includes. */
#include "drvAudio.h"
#include "drvPM.h"

#include "apiVOC.h"

#include "voc_task.h"
#include "FreeRTOS.h"
#include "task.h"
#define VOC_MSG(fmt, args...) MOS_DBG_PRINT(MOS_DBG_LEVEL_VOC, "[VOC] " fmt, ##args)
#define VOC_ERROR(fmt, args...) MOS_DBG_ERROR("[VOC ERR] " fmt, ##args);

/*============================================================================*/
/*============================================================================*/
/* Voice Task Structure                                                       */
/*============================================================================*/
/*============================================================================*/

//mapping to VOC_FUNC_e
#define ebBIT_RESET    BIT0
#define ebBIT_VQ       BIT1
#define ebBIT_VD       BIT2
#define ebBIT_VP       BIT3
#define ebBIT_VS       BIT4
#define ebBIT_CUS0     BIT5
#define ebBIT_TEST     BIT6
#define ebBIT_VP_TEST  BIT7

#define ebBIT_VOC (ebBIT_RESET | ebBIT_VQ | ebBIT_VD | ebBIT_VS | ebBIT_TEST | ebBIT_VP)

#define VOC_TASK_STACK_SIZE 512
#define VOC_TASK_PRIORITY  ( tskIDLE_PRIORITY + 2)
static StackType_t _tVocTaskStack[ VOC_TASK_STACK_SIZE ];
static StaticTask_t _tVocTaskBuffer ;

#define VOC_TASK_PCM_BUF_MS (16) //16msec
#define VOC_TASK_PCM_BUF_COUNT (AUDIO_DMA_WR_SAMPLE_RATE * AUDIO_DMIC_NO * VOC_TASK_PCM_BUF_MS)/1000

#if defined(CONFIG_VOC_SW_SRC)
static S16 _ps16VocTaskPcmBuf[VOC_TASK_PCM_BUF_COUNT*2];
#define VOC_TASK_READ_PCM_FRAME (AUDIO_DMA_WR_SAMPLE_RATE*VOC_TASK_PCM_BUF_MS*2)/1000
#elif defined(CONFIG_VOC_SW_SRC48K)
static S16 _ps16VocTaskPcmBuf[VOC_TASK_PCM_BUF_COUNT*3];
#define VOC_TASK_READ_PCM_FRAME (AUDIO_DMA_WR_SAMPLE_RATE*VOC_TASK_PCM_BUF_MS*3)/1000
#else
static S16 _ps16VocTaskPcmBuf[VOC_TASK_PCM_BUF_COUNT];
#define VOC_TASK_READ_PCM_FRAME (AUDIO_DMA_WR_SAMPLE_RATE*VOC_TASK_PCM_BUF_MS)/1000
#endif

static SemaphoreHandle_t _tVocSyncSem = NULL;
static EventGroupHandle_t _tVocProcEvent = NULL;
static VOC_PROC_CUS_FUNC_t _pdfVocProcCusFunc = 0;
static VOC_VQMode_e _eVocVQMode = E_VQ_MODE_NORMAL;

/*============================================================================*/
/*============================================================================*/
/* VD Task Structure                                                          */
/*============================================================================*/
/*============================================================================*/

#define ebBIT_VD_RESET    BIT0
#define ebBIT_VD_MOVE     BIT1
#define ebBIT_VD_SKIP     BIT2

#define ebBIT_VD_EVENT (ebBIT_VD_RESET | ebBIT_VD_MOVE | ebBIT_VD_SKIP)

#define VD_TASK_STACK_SIZE 128
#define VD_TASK_PRIORITY  ( tskIDLE_PRIORITY + 3)
static StackType_t _tVdTaskStack[ VD_TASK_STACK_SIZE ];
static StaticTask_t _tVdTaskBuffer ;

#define VD_TASK_PCM_BUF_MS VOC_TASK_PCM_BUF_MS
#define VD_TASK_PCM_BUF_COUNT (AUDIO_DMA_WR_SAMPLE_RATE * AUDIO_DMIC_NO * VD_TASK_PCM_BUF_MS)/1000
#define VD_TASK_READ_PCM_FRAME_BYTES (AUDIO_DMA_WR_SAMPLE_RATE*VD_TASK_PCM_BUF_MS*2)/1000
static S16 _ps16VdTaskPcmBuf[VD_TASK_PCM_BUF_COUNT];

static EventGroupHandle_t _tVdTaskEvent = NULL;
/*============================================================================*/
/*============================================================================*/
/* Voice Streaming Structure                                                  */
/*============================================================================*/
/*============================================================================*/
#if defined(CONFIG_VOC_DKWS) && defined(CONFIG_VOC_SEAMLESS)
#define VOC_STREAM_BUF_MS VD_TASK_PCM_BUF_MS*102 //(default 1600+36ms)
#elif defined(CONFIG_VOC_DKWS)
#define VOC_STREAM_BUF_MS VD_TASK_PCM_BUF_MS*87  //(default 1392ms)
#else
#define VOC_STREAM_BUF_MS VD_TASK_PCM_BUF_MS*15 //(default 240ms)
#endif

#define VOC_STREAM_BUF_COUNT (AUDIO_DMA_WR_SAMPLE_RATE * 4 * VOC_STREAM_BUF_MS)/1000
static S16 _ps16VocStreamBuf[VOC_STREAM_BUF_COUNT+1];
static StreamBufferHandle_t _tVocStreamBuffer = NULL;
static StaticStreamBuffer_t _tVocStreamBufferStruct;

/*============================================================================*/
/*============================================================================*/
/* Debug, Val_20180410                                                        */
/*============================================================================*/
/*============================================================================*/
#define ENABLE_DEBUG_BY_FORCE_WAKEUP           (0)
#if ENABLE_DEBUG_BY_FORCE_WAKEUP
BOOL VocDebugForceWakeup(BOOL bRet)
{
  static int iCount = 0;
  if (bRet)
    return bRet;
  iCount++;
  if (iCount >100)
  {
    iCount =0;
    return true;
  }
  return false;
}
#endif
/*============================================================================*/
/*============================================================================*/
/* Voice Processing Task                                                      */
/*============================================================================*/
/*============================================================================*/

//static U32 voc_ms = 0;
static U32 err_try_again = 0;

static void prvVocProcTask(void *pvParameters)
{
  size_t xBytesSent;

  U32 u32ReturnCount;
  U32 u32Result;

  static U32 wakeUpNum;
  EventBits_t uxBits;

  BOOL bRet;
//TickType_t xTicksToWait = portMAX_DELAY;
  (void) pvParameters;

  VOC_MSG("VocProcTask Start\n");

  for(;;)
  {
    uxBits = xEventGroupWaitBits(_tVocProcEvent, ebBIT_VOC , pdFALSE, pdFALSE, portMAX_DELAY);
    if(uxBits & ebBIT_RESET)
    {
      MDrv_AUD_Stop();

#if defined(CONFIG_VOC_SW_VAD)
      if(MApi_VOC_VADIsEnable())
        MApi_VOC_VADStopTimer();
#endif

      xEventGroupSetBits(_tVdTaskEvent, ebBIT_VD_RESET);
      //xEventGroupWaitBits(_tVocProcEvent, ebBIT_VOC, pdTRUE, pdFALSE, 0);
      xEventGroupClearBits(_tVocProcEvent, ebBIT_VOC);
      xSemaphoreGive(_tVocSyncSem);

      continue;
    }
    else
    {
      if(MDrv_AUD_IsXrun())
      {
        if (uxBits & ebBIT_VD)
        {
          MApi_VOC_NotifyStatus(E_STATUS_XRUN);
        }
      }

      if (MDrv_AUD_Start())
      {
        MApi_VOC_SwGainReset();

#if defined(CONFIG_VOC_SWHPF)
        MApi_VOC_HpfReset();
#endif

#if defined(CONFIG_VOC_SW_SRC)
        MApi_VOC_SrcInit();
#elif defined(CONFIG_VOC_SW_SRC48K)
        MApi_VOC_SrcInit48k();
#endif
      }
    }

    if(MDrv_AUD_ReadPCM(_ps16VocTaskPcmBuf, VOC_TASK_READ_PCM_FRAME*MDrv_AUD_GetMicNo(), &u32ReturnCount, pdTRUE) == pdPASS)
    {
      //VOC_MSG("mic num:%d\n",MDrv_AUD_GetMicNo());
      //voc_ms = MHal_Timer_GetTick()/TIMER_DELAY_1ms;
#if defined(CONFIG_VOC_SW_SRC)
      bRet = MApi_VOC_SrcProcess(_ps16VocTaskPcmBuf, &u32ReturnCount);
      if(!bRet)
      {
        VOC_ERROR("MApi_VOC_SrcPort: failed\n");
      }
#elif defined(CONFIG_VOC_SW_SRC48K)
      bRet = MApi_VOC_SrcProcess48k(_ps16VocTaskPcmBuf, &u32ReturnCount);
      if(!bRet)
      {
        VOC_ERROR("MApi_VOC_SrcProcess48k: failed\n");
      }
#endif

#if defined(CONFIG_VOC_SW_VAD)
      if(MApi_VOC_VADIsEnable())
      {
        bRet = MApi_VOC_VADPort(_ps16VocTaskPcmBuf, u32ReturnCount, &u32Result);
        if (bRet)
        {
           if (u32Result)
             MApi_VOC_VADResetTimer();
           else
           {
             if (MApi_VOC_VADIsTimeout())
               continue;
           }
        }
        else
          MApi_VOC_VADStopTimer();
      }
#endif

#if defined(CONFIG_VOC_SWHPF)
      bRet=MApi_VOC_Hpf(_ps16VocTaskPcmBuf, u32ReturnCount*2);
      if(!bRet)
      {
         // xEventGroupClearBits(_tVocProcEvent, ebBIT_CUS1);
        VOC_ERROR("MApi_VOC_Hpf: HPF failed\n");
      }
#endif

      bRet=MApi_VOC_SwGain(_ps16VocTaskPcmBuf, u32ReturnCount*2);
      if(!bRet)
      {
        VOC_ERROR("MApi_VOC_SwGain: failed\n");
      }

#if defined(CONFIG_VOC_READBIN)
      bRet=MApi_VOC_ReadPcmBin(_ps16VocTaskPcmBuf, &u32ReturnCount);
      if(!bRet)
      {
        VOC_MSG("MApi_VOC_ReadPCMBIN: loop data\n");
        continue;
      }
#endif

#if defined(CONFIG_VOC_CUS0)
      if(uxBits & ebBIT_VD)
      {
        if(uxBits & ebBIT_CUS0)
        {
          bRet = MApi_VOC_CUSPort0(_ps16VocTaskPcmBuf, &u32ReturnCount);
          if(!bRet)
          {
            xEventGroupClearBits(_tVocProcEvent, ebBIT_CUS0);
            VOC_ERROR("MApi_VOC_CUSPort0: CUS0 process failed\n");
          }
        }
      }
#endif

#if defined(CONFIG_VOC_MIC_MAPPING)
      bRet = MApi_VOC_MicMap(_ps16VocTaskPcmBuf, &u32ReturnCount);
      if(!bRet)
      {
        VOC_ERROR("MApi_VOC_MicMap: failed\n");
      }
#endif

      if(uxBits & ebBIT_VP_TEST)
      {
        bRet = MApi_VOC_VPPort(_ps16VocTaskPcmBuf, &u32ReturnCount);
        if(!bRet)
        {
          xEventGroupClearBits(_tVocProcEvent, ebBIT_VP_TEST);
          VOC_ERROR("MApi_VOC_VPPort: VP process test failed\n");
        }
      }

      /*xBytesSent = xStreamBufferSend( _tVocStreamBuffer,
                            (void *)_ps16VocTaskPcmBuf, u32ReturnCount*2, 0);

      if (xBytesSent != u32ReturnCount*2)
      {
        xEventGroupClearBits(_tVocProcEvent, ebBIT_VD);
        VOC_ERROR("xStreamBufferSend: buffer full\n");
      }
      else
      {
        if (uxBits & ebBIT_VD)
        {
          xEventGroupSetBits(_tVdTaskEvent, ebBIT_VD_MOVE);
        }
        else if (uxBits & (ebBIT_VQ | ebBIT_VS))
        {
          if (xStreamBufferIsFull(_tVocStreamBuffer))
          {
            xEventGroupSetBits(_tVdTaskEvent, ebBIT_VD_SKIP);
          }
        }
      }*/

      //VOC_MSG("xStreamBufferSend() xBytesSent = %d\n", xBytesSent);

      if(uxBits & ebBIT_VP)
      {
        bRet = MApi_VOC_VPPort(_ps16VocTaskPcmBuf, &u32ReturnCount);
        if(!bRet)
        {
          xEventGroupClearBits(_tVocProcEvent, ebBIT_VP);
          VOC_ERROR("MApi_VOC_VPPort: VP process failed\n");
        }
      }

#if defined(CONFIG_VOC_TEST)
      if(uxBits & ebBIT_TEST)
      {
        if (_pdfVocProcCusFunc)
        {
          bRet = _pdfVocProcCusFunc(_ps16VocTaskPcmBuf, u32ReturnCount);
          if(bRet)
          {
            //xEventGroupWaitBits(_tVocProcEvent, ebBIT_TEST, pdTRUE, pdFALSE, 0);
            xEventGroupClearBits(_tVocProcEvent, ebBIT_TEST);
          }
        }
      }
#endif
// add by gufeng
      xBytesSent = xStreamBufferSend( _tVocStreamBuffer,
                            (void *)_ps16VocTaskPcmBuf, u32ReturnCount*2, 0);

      if (xBytesSent != u32ReturnCount*2)
      {
        xEventGroupClearBits(_tVocProcEvent, ebBIT_VD);
        //VOC_ERROR("xStreamBufferSend: buffer full\n");
      }
      else
      {
        if (uxBits & ebBIT_VD)
        {
          xEventGroupSetBits(_tVdTaskEvent, ebBIT_VD_MOVE);
        }
        else if (uxBits & (ebBIT_VQ | ebBIT_VS))
        {
          if (xStreamBufferIsFull(_tVocStreamBuffer))
          {
            xEventGroupSetBits(_tVdTaskEvent, ebBIT_VD_SKIP);
          }
        }
      }
// add by gufeng

      if(uxBits & ebBIT_VQ)
      {
        bRet = MApi_VOC_VQPort(_ps16VocTaskPcmBuf, u32ReturnCount);
       
        #if ENABLE_DEBUG_BY_FORCE_WAKEUP
          bRet = VocDebugForceWakeup(bRet);
        #endif
        
       if(bRet)
        {
       #if 0
         static char buf[1024];
         vTaskGetRunTimeStats(buf);
         printf("Run Time Stats:\nTask Name Time Percent\n%s\n", buf);
       #endif
          wakeUpNum++;
          printf("wake up num ---%d \n",wakeUpNum);
          VOC_MSG("MApi_VOC_VQPort: VQ process success t = %d\n", xTaskGetTickCount() * portTICK_PERIOD_MS);
          VocTaskVQPost();
        }
      }
#if 0
      if(uxBits & ebBIT_VD)
      {
        if(uxBits & ebBIT_CUS0)
        {
          bRet = MApi_VOC_CUSPort0(_ps16VocTaskPcmBuf, &u32ReturnCount);
          if(!bRet)
          {
            xEventGroupClearBits(_tVocProcEvent, ebBIT_CUS0);
            VOC_ERROR("MApi_VOC_CUSPort0: CUS0 process failed\n");
          }
        }

        bRet = MApi_VOC_VDPort(_ps16VocTaskPcmBuf, u32ReturnCount);
        if(bRet)
          MApi_VOC_NotifyPeriod(u32ReturnCount);
        else
        {
          xEventGroupClearBits(_tVocProcEvent, ebBIT_VD);
          VOC_ERROR("VocProcVD: VD process failed\n");
        }
      }
#endif

      //VOC_MSG("%d\n", ((MHal_Timer_GetTick()/TIMER_DELAY_1ms)- voc_ms));
    }
    else
    {
      if (uxBits & (ebBIT_VQ | ebBIT_VS))
      {
#if defined(CONFIG_VOC_SW_VAD)
        MApi_VOC_VADStopTimer();
#endif
        if (err_try_again++ >= 3)
        {
          //xEventGroupWaitBits(_tVocProcEvent, ebBIT_VOC, pdTRUE, pdFALSE, 0);
          xEventGroupClearBits(_tVocProcEvent, ebBIT_VOC);
          //VocTaskVQPost();
          err_try_again = 0;
        }
      }

      if (uxBits & ebBIT_VD)
      {
        if(MDrv_AUD_IsXrun())
        {
          MApi_VOC_NotifyStatus(E_STATUS_XRUN);
        }
      }

      VOC_ERROR("prvVocProcTask: MDrv_AUD_ReadPCM failed\n");
    }

  } //for
}

void VocTaskVQConfig(VOC_VQMode_e eMode)
{
  //if (!VocTaskIsEnableFunc(E_FUNC_VQ))
  {
    if (eMode < E_VQ_MODE_MAX)
      _eVocVQMode = eMode;
    VOC_MSG("MApi_VOC_VQConfig: Mode = %d\n", (U32)eMode);
  }
}

void VocTaskVQPost(void)
{
  if (_eVocVQMode == E_VQ_MODE_PM)
  {
    VocTaskEnableFunc(FALSE, E_FUNC_VQ);
#if defined(CONFIG_VOC_SEAMLESS)
    VocTaskEnableFunc(TRUE, E_FUNC_VS); //turn on seamless
#endif
    MDrv_PM_Resume();
  }
  else if (_eVocVQMode == E_VQ_MODE_LOOP)
  {
    //VocTaskEnableFunc(TRUE, E_FUNC_VQ);
    VOC_MSG("MApi_VOC_VQPost: Loop again\n");
  }
#if defined(CONFIG_VOC_SW_VAD)
  else if (_eVocVQMode == E_VQ_MODE_VAD)
  {
    VocTaskEnableFunc(FALSE, E_FUNC_VQ);
    MDrv_PM_Resume();
  }
  else if (_eVocVQMode == E_VQ_MODE_TEST)
  {
    MApi_VOC_VADResetTimer();
    VOC_MSG("MApi_VOC_VQPost: Loop again (VAD)\n");
  }
#endif
  else if (_eVocVQMode == E_VQ_MODE_DWW)
  {
    //VocTaskEnableFunc(TRUE, E_FUNC_VD);
    VOC_MSG("MApi_VOC_VQPost: DWW\n");
  }
  else if (_eVocVQMode == E_VQ_MODE_TEST)
  {
    VocTaskEnableFunc(TRUE, E_FUNC_VD);
    VOC_MSG("MApi_VOC_VQPost: TEST\n");
  }
}

void VocTaskRegisterCusFunc(VOC_PROC_CUS_FUNC_t pdfFunc)
{
  if (!(xEventGroupGetBits(_tVocProcEvent) & ebBIT_TEST))
  {
    _pdfVocProcCusFunc = pdfFunc;
    VOC_MSG("VocTaskRegisterCusFunc: 0x%x\n", (U32)_pdfVocProcCusFunc);
  }
  else
    VOC_ERROR("VocTaskRegisterCusFunc failed: State not match\n");
}

void VocTaskReset(void)
{
  xEventGroupSetBits(_tVocProcEvent, ebBIT_RESET);
  xSemaphoreTake(_tVocSyncSem, portMAX_DELAY);
}

void VocTaskEnableFunc(BOOL bEnable, VOC_FUNC_e eFunc)
{
  if(eFunc != E_FUNC_NONE && eFunc < E_FUNC_NUM)
  {
    if (bEnable)
    {
      if(eFunc == E_FUNC_VQ)
      {
        if (_eVocVQMode == E_VQ_MODE_PM)
          MDrv_PM_Suspend();
#if defined(CONFIG_VOC_SW_VAD)
        else if (_eVocVQMode == E_VQ_MODE_VAD)
        {
          MDrv_PM_Suspend();
          MApi_VOC_VADStartTimer();
        }
        else if (_eVocVQMode == E_VQ_MODE_TEST)
        {
          MApi_VOC_VADStartTimer();
        }
#endif
        //xEventGroupSetBits(_tVdTaskEvent, ebBIT_VD_SKIP);
        //xStreamBufferSetTriggerLevel(_tVocStreamBuffer, sizeof(_ps16VocStreamBuf));
      }
      else if (eFunc == E_FUNC_VD)
      {
        if (xEventGroupGetBits(_tVocProcEvent) & ebBIT_VS)
          xEventGroupClearBits(_tVocProcEvent, ebBIT_VS); //turn off seamless

        //xEventGroupSetBits(_tVdTaskEvent, ebBIT_VD_MOVE);
      }

      xEventGroupSetBits(_tVocProcEvent, (1 << eFunc));
    }
    else
    {
      if(eFunc == E_FUNC_VD)
      {
        xEventGroupClearBits(_tVdTaskEvent, ebBIT_VD_MOVE);
        MApi_VOC_VDReset();
      }
      else if(eFunc == E_FUNC_VQ)
      {
#if defined(CONFIG_VOC_SW_VAD)
        if(_eVocVQMode == E_VQ_MODE_VAD)
          MApi_VOC_VADStopTimer();
        else if (_eVocVQMode == E_VQ_MODE_TEST)
          MApi_VOC_VADStopTimer();
#endif
        //xStreamBufferSetTriggerLevel(_tVocStreamBuffer, 0);
      }

      xEventGroupClearBits(_tVocProcEvent, (1 << eFunc));
    }
  }
  else
    VOC_ERROR("VocTaskEnableFunc failed: parameter error\n");
}

BOOL VocTaskIsEnableFunc(VOC_FUNC_e eFunc)
{
  if (eFunc != E_FUNC_NONE && eFunc < E_FUNC_NUM)
  {
    return xEventGroupGetBits(_tVocProcEvent) & (1 << eFunc);
  }
  else
  {
    VOC_ERROR("VocTaskIsEnableFunc failed: parameter error\n");
    return FALSE;
  }
}

static void prvVdProcTask(void *pvParameters)
{
  BOOL bRet;
  size_t xReceivedBytes;
  TickType_t xTicksToWait = portMAX_DELAY;
  EventBits_t uxBits;

  (void) pvParameters;

  VOC_MSG("VdProcTask Start\n");

  for(;;)
  {

    uxBits = xEventGroupWaitBits(_tVdTaskEvent, ebBIT_VD_EVENT , pdFALSE, pdFALSE, portMAX_DELAY);
    if(uxBits & ebBIT_VD_RESET)
    {
      xStreamBufferReset(_tVocStreamBuffer);
      xEventGroupClearBits(_tVdTaskEvent, ebBIT_VD_EVENT);
      VOC_MSG("VdTask: Reset\n");
      continue;
    }

    xReceivedBytes = xStreamBufferReceive(_tVocStreamBuffer,
                                  (void *)_ps16VdTaskPcmBuf,
                                   VD_TASK_READ_PCM_FRAME_BYTES*MDrv_AUD_GetMicNo(), xTicksToWait);
    if(xReceivedBytes > 0)
    {
      //VOC_MSG("VdTask: xStreamBufferReceive() xReceivedBytes= %d\n", xReceivedBytes);
      if(uxBits & ebBIT_VD_MOVE)
      {
        bRet = MApi_VOC_VDPort(_ps16VdTaskPcmBuf, xReceivedBytes/2);
        if(bRet)
        {
          if (xStreamBufferIsEmpty(_tVocStreamBuffer))
          {
            MApi_VOC_VDNotify(xReceivedBytes/2);
            xEventGroupClearBits(_tVdTaskEvent, ebBIT_VD_MOVE);
          }
        }
        else
        {
          xEventGroupClearBits(_tVdTaskEvent, ebBIT_VD_MOVE);
          VOC_ERROR("VdTask: MApi_VOC_VDPort failed\n");
        }
      }

      if (uxBits & ebBIT_VD_SKIP)
      {
        xEventGroupClearBits(_tVdTaskEvent, ebBIT_VD_SKIP);
      }

      //if (xReceivedBytes != sizeof(_ps16VdTaskPcmBuf))
      //  VOC_ERROR("VdTask: xStreamBufferReceive warning (receive bytes not match buffer size)\r\n");

      //VOC_MSG("VdTask: buffer space availabe = %d\n", xStreamBufferSpacesAvailable(_tVocStreamBuffer));
    }
    else
    {

      VOC_ERROR("VdTask: xStreamBufferReceive failed\n");
    }
  } //for
}

void VocTaskStart(void)
{
  _tVocProcEvent = xEventGroupCreate();
  configASSERT( _tVocProcEvent );

  _tVocSyncSem = xSemaphoreCreateBinary();
  configASSERT( _tVocSyncSem );

  _tVocStreamBuffer = xStreamBufferCreateStatic(sizeof(_ps16VocStreamBuf)-1,
                                                0,
                                               (U8 *)_ps16VocStreamBuf,
                                               &_tVocStreamBufferStruct );
  configASSERT( _tVocStreamBuffer );

  //VOC_MSG("VocTaskStart: buffer space availabe = %d\n", xStreamBufferSpacesAvailable(_tVocStreamBuffer));

  _tVdTaskEvent = xEventGroupCreate();
  configASSERT( _tVdTaskEvent );

  /* Create that task that handles the console itself. */
  xTaskCreateStatic(prvVocProcTask, "VOC", VOC_TASK_STACK_SIZE, NULL, VOC_TASK_PRIORITY, _tVocTaskStack, &_tVocTaskBuffer);
  xTaskCreateStatic(prvVdProcTask, "VD", VD_TASK_STACK_SIZE, NULL, VD_TASK_PRIORITY, _tVdTaskStack, &_tVdTaskBuffer);
}

/*-----------------------------------------------------------*/

