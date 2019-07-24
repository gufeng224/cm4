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

#define VOC_MSG(fmt, args...) MOS_DBG_PRINT(MOS_DBG_LEVEL_VOC, "[VOC] " fmt, ##args)
#define VOC_ERROR(fmt, args...) MOS_DBG_ERROR("[VOC ERR] " fmt, ##args);

#if defined(CONFIG_VOC_READBIN)
INCBIN(voice, "../../3rd/Test/mics4_750ms.pcm");

static char *_pIncBinStart = (char *)&incbin_voice_start;
static char *_pIncBinEnd = (char *)&incbin_voice_end;
static char *_pIncBinPtr = (char *)&incbin_voice_start;
#endif

static SemaphoreHandle_t _tVdMutex;
static U32 _u32ACPUBufferAddr = 0;
static U32 _u32ACPUBufferSize = 0;
static S32 _u32ACPUBufferPtr = 0;
static U32 _u32PeriodCount = 0;

static U16 _u16MaxToNotify = 0;
static U16 _u16CountToNotify = 0;

void MApi_VOC_VDInit(void)
{
  _tVdMutex = xSemaphoreCreateMutex();
  configASSERT( _tVdMutex );
}

void MApi_VOC_VDConfig(U32 u32BufferAddr, U32 u32BufferSize)
{
  //if (!(xEventGroupGetBits(_tVocProcEvent) & ebBIT_VD))
  {
    _u32ACPUBufferAddr = u32BufferAddr;
    _u32ACPUBufferSize = u32BufferSize;
    _u32ACPUBufferPtr = _u32ACPUBufferAddr;
    _u32PeriodCount = 0;

    _u16MaxToNotify = 0;
    _u16CountToNotify = 0;

    VOC_MSG("MApi_VOC_VDConfig: 0x%x(0x%x)\n", _u32ACPUBufferAddr, _u32ACPUBufferSize);
  }
  //else
  //  VOC_ERROR("MApi_VOC_VDConfig failed: State not match\n");
}

void MApi_VOC_VDConfigExt(U32 u32BufferAddr, U32 u32BufferSize, U16 u16MaxToNotify)
{
    _u32ACPUBufferAddr = u32BufferAddr;
    _u32ACPUBufferSize = u32BufferSize;
    _u32ACPUBufferPtr = _u32ACPUBufferAddr;
    _u32PeriodCount = 0;

    _u16MaxToNotify = u16MaxToNotify;
    _u16CountToNotify = 0;

    VOC_MSG("MApi_VOC_VDConfigExt: 0x%x(0x%x) - %d\n", _u32ACPUBufferAddr, _u32ACPUBufferSize, _u16MaxToNotify);
}

void MApi_VOC_VDReset(void)
{
  if( xSemaphoreTake( _tVdMutex, portMAX_DELAY ) == pdPASS )
  {
    _u32ACPUBufferPtr = _u32ACPUBufferAddr;
    _u32PeriodCount = 0;
    _u16CountToNotify = 0;

    xSemaphoreGive(_tVdMutex);
  }
  VOC_MSG("MApi_VOC_VDReset\n");
}

BOOL MApi_VOC_VDPort(S16 *ps16Buffer, U32 u32Count)
{
  U32 u32Buffer = (U32)ps16Buffer;
  U32 u32Size = u32Count*2;

  if( xSemaphoreTake( _tVdMutex, portMAX_DELAY ) == pdPASS )
  {
    if (u32Size > _u32ACPUBufferSize || !_u32ACPUBufferAddr)
    {
      VOC_ERROR("MApi_VOC_VDPort: copy DMA data size 0x%x to ACPU 0x%x(0x%x)\n", u32Size, _u32ACPUBufferAddr, _u32ACPUBufferSize);
      xSemaphoreGive(_tVdMutex);
      return pdFAIL;
    }

    if(_u32ACPUBufferPtr + u32Size >= _u32ACPUBufferAddr + _u32ACPUBufferSize)
    {
      //memcpy(ptr, buffer, _gMcuDmaBuf3 + sizeof(_gMcuDmaBuf3) - ptr);
      MDrv_BDMA_Copy(u32Buffer, _u32ACPUBufferPtr, (_u32ACPUBufferAddr + _u32ACPUBufferSize - _u32ACPUBufferPtr), E_BDMA_CPtoHK);
     // VOC_MSG("MApi_VOC_VDPort: moving audio data size 0x%x to ACPU 0x%x, period = %d\n", u32Size, _u32ACPUBufferPtr, _u32PeriodCount);
      //_chip_flush_miu_pipe();
      u32Size -= (_u32ACPUBufferAddr + _u32ACPUBufferSize - _u32ACPUBufferPtr);
      u32Buffer +=  (_u32ACPUBufferAddr + _u32ACPUBufferSize - _u32ACPUBufferPtr);
      _u32ACPUBufferPtr = _u32ACPUBufferAddr;
    }

    if (u32Size > 0)
    {
      //memcpy(ptr, buffer, size);
      MDrv_BDMA_Copy(u32Buffer, _u32ACPUBufferPtr, u32Size, E_BDMA_CPtoHK);
      //VOC_MSG("MApi_VOC_VDPort: moving audio data size 0x%x to ACPU 0x%x, period = %d\n", u32Size, _u32ACPUBufferPtr, _u32PeriodCount);
      _u32ACPUBufferPtr += u32Size;
    }

    _u32PeriodCount++;
    _u16CountToNotify++;
    xSemaphoreGive(_tVdMutex);
  }

  return TRUE;
}

void MApi_VOC_VDNotify(U32 u32Count)
{
  MBX_Msg tMsg;

  if (_u16CountToNotify < _u16MaxToNotify)
    return;

  tMsg.eRoleID = E_MBX_ROLE_HK;
  //tMsg.eRoleID = E_MBX_ROLE_CP;
  tMsg.eMsgType = E_MBX_MSG_TYPE_NORMAL;
  tMsg.u8MsgClass = E_MBX_CLASS_VOC;
  tMsg.u8Index = E_MBX_MSG_PERIOD_NOTIFY;

  //MBX_MSG_PUT_U32(tMsg, _u32PeriodCount);
  //MBX_MSG_PUT_2U32(tMsg, _u32PeriodCount, u32Count*2);

  MBX_MSG_PUT_2U32(tMsg, _u32PeriodCount, (u32Count*2));

  MDrv_MBX_SendMsg(&tMsg);
  //OUTREG16(0x40006600, 0x7878);
  //MDrv_MBX_SendMsgExt(&tMsg);

  _u16CountToNotify = 0;
}


void MApi_VOC_NotifyStartTime(void)
{
  MBX_Msg tMsg;
  U64 u64StartTime;

  U32 u32Low, u32High;

  tMsg.eRoleID = E_MBX_ROLE_HK;
  //tMsg.eRoleID = E_MBX_ROLE_CP;
  tMsg.eMsgType = E_MBX_MSG_TYPE_NORMAL;
  tMsg.u8MsgClass = E_MBX_CLASS_VOC;
  tMsg.u8Index = E_MBX_MSG_STARTTIME_NOTIFY;

  u64StartTime = MDrv_AUD_GetStartTime();
  u32Low = (U32)u64StartTime;
  u32High = (U32)(u64StartTime>>32);
  MBX_MSG_PUT_2U32(tMsg, u32Low, u32High);

  MDrv_MBX_SendMsg(&tMsg);
}

void MApi_VOC_NotifyPeriod(U32 u32Count)
{
  MBX_Msg tMsg;

  tMsg.eRoleID = E_MBX_ROLE_HK;
  //tMsg.eRoleID = E_MBX_ROLE_CP;
  tMsg.eMsgType = E_MBX_MSG_TYPE_NORMAL;
  tMsg.u8MsgClass = E_MBX_CLASS_VOC;
  tMsg.u8Index = E_MBX_MSG_PERIOD_NOTIFY;

  //MBX_MSG_PUT_U32(tMsg, _u32PeriodCount);
  //MBX_MSG_PUT_2U32(tMsg, _u32PeriodCount, u32Count*2);

  MBX_MSG_PUT_2U32(tMsg, _u32PeriodCount, (u32Count*2));

  MDrv_MBX_SendMsg(&tMsg);
  //OUTREG16(0x40006600, 0x7878);
  //MDrv_MBX_SendMsgExt(&tMsg);
}

void MApi_VOC_NotifyStatus(U8 u8Status)
{
  MBX_Msg tMsg;

  tMsg.eRoleID = E_MBX_ROLE_HK;
  //tMsg.eRoleID = E_MBX_ROLE_CP;
  tMsg.eMsgType = E_MBX_MSG_TYPE_NORMAL;
  tMsg.u8MsgClass = E_MBX_CLASS_VOC;
  tMsg.u8Index = E_MBX_MSG_STATUS_NOTIFY;
  tMsg.u8ParameterCount = 1;
  tMsg.u8Parameters[0] = u8Status;

  MDrv_MBX_SendMsg(&tMsg);
}


#if defined(CONFIG_VOC_READBIN)
BOOL MApi_VOC_ReadPcmBin(S16 *ps16Buffer, U32 *pu32Count)
{
  if (_pIncBinPtr+(*pu32Count)*2 > _pIncBinEnd)
  {
    _pIncBinPtr = _pIncBinStart;
    return FALSE;
  }

  memcpy(ps16Buffer, _pIncBinPtr, (*pu32Count)*2);
  _pIncBinPtr += (*pu32Count)*2;

  return pdTRUE;
}
#endif

/*-----------------------------------------------------------*/

