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

#define _DRV_MBX_C
/* Standard includes. */

/* Kernel includes. */
#include "mosWrapper.h"

/* driver includes. */
#include "drvMBX.h"
#include "halMBX.h"
#include "halCPUINT.h"

#define MBX_INT_PRIORITY		(configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY)

#define MBX_MSG(fmt, args...) MOS_DBG_PRINT(MOS_DBG_LEVEL_MBX, "[MBX] " fmt, ##args)
#define MBX_ERROR(fmt, args...) MOS_DBG_ERROR("[MBX ERR] " fmt, ##args);

//=============================================================================
// Local Variables
//=============================================================================
typedef struct
{
  MBX_ROLE_ID eMbxHostRole;
  //MBX_ROLE_ID eMbxCpu2Role[E_MBX_CPU_MAX];
  CPUINT_ID_e eMbxRole2Cpu[E_MBX_ROLE_MAX];
  U32 u32TimeoutMillSecs;

  S32 s32RefCnt;
  BOOL bMsgRecvEnable;
  BOOL bInInterruptHandler;
  U32 u32IntLockerMBX;
  S32 s32SendMutexMBX;
  S32 s32RecvMutexMBX;
  U8 u8DbgLevel;

  SemaphoreHandle_t tMbxIntSem;
  SemaphoreHandle_t tMbxMutex;
  IRQn_Type eMbxIntNum;
} MBX_Mgr;


static MBX_Mgr _mgrMBX =
{
  .eMbxHostRole = E_MBX_ROLE_MAX,
  .s32RefCnt = 0,
  .bMsgRecvEnable = FALSE,
  .bInInterruptHandler = FALSE, ////suppose two interrupt has the same priority, won't break each other.
  .s32SendMutexMBX = -1,
  .s32RecvMutexMBX = -1,
  .u8DbgLevel = MBX_DBG_LEVEL_NONE,
  .tMbxIntSem = NULL,
  .tMbxMutex = NULL
};

/*-----------------------------------------------------------*/


void NVIC_Handler_MBX(void)
{
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;

  MHal_CPUINT_Clear(E_CPUINT_CP , E_CPUINT_HK0);

  MBX_MSG("NVIC_Handler_MBX ms = %d\r\n", xTaskGetTickCount()*portTICK_PERIOD_MS);

  xSemaphoreGiveFromISR(_mgrMBX.tMbxIntSem, &xHigherPriorityTaskWoken);

  /* Call the IntQ test function for this channel. */
  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

}

void MDrv_MBX_Init(void)
{

  MHAL_MBX_Init(E_MBX_ROLE_CP);

  _mgrMBX.eMbxHostRole = E_MBX_ROLE_CP;
  _mgrMBX.eMbxRole2Cpu[E_MBX_ROLE_CP] = E_CPUINT_CP;
  _mgrMBX.eMbxRole2Cpu[E_MBX_ROLE_HK] = E_CPUINT_HK0;

  _mgrMBX.eMbxIntNum = HK0ToCP_IRQn;

  // Create the semaphore to guard a shared resource.
  _mgrMBX.tMbxIntSem = xSemaphoreCreateBinary();
  configASSERT(_mgrMBX.tMbxIntSem);
  _mgrMBX.tMbxMutex = xSemaphoreCreateMutex();
  configASSERT(_mgrMBX.tMbxMutex);

  MOS_IRQn_AttachIsr(NVIC_Handler_MBX, _mgrMBX.eMbxIntNum, MBX_INT_PRIORITY);
  //MOS_IRQn_AttachIsr(INTC_Handler_HK0ToCP_IRQn, HK0ToCP_IRQn, 0);
  //MOS_IRQn_AttachIsr(INTC_Handler_HK1ToCP_IRQn, HK1ToCP_IRQn, 0);
  //MOS_IRQn_Unmask(HK0ToCP_IRQn);
  //MOS_IRQn_Unmask(HK1ToCP_IRQn);
}


MBX_Result MDrv_MBX_SendMsg(MBX_Msg *pMsg)
{
  MBX_Result mbxResult = E_MBX_SUCCESS;
  MBXHAL_Fire_Status mbxHalFireStatus;
  U32 u32WaitCnt = 0;
  //parameter check:
  if(NULL == pMsg)
  {
    MBX_ERROR("MDrv_MBX_SendMsg: E_MBX_ERR_INVALID_PARAM!\r\n");
    return E_MBX_ERR_INVALID_PARAM;
  }

  do
  {
    mbxResult = MHAL_MBX_GetFireStatus(_mgrMBX.eMbxHostRole, pMsg->eRoleID, &mbxHalFireStatus);
    if(E_MBX_SUCCESS != mbxResult)
    {
      MBX_ERROR("MDrv_MBX_SendMsg: MHAL_MBX_GetFireStatus failed = %d\r\n ", mbxResult);
      return E_MBX_ERR_INVALID_PARAM;
    }

    if(E_MBXHAL_FIRE_SUCCESS == mbxHalFireStatus)
      break;

    //check If Timeout:
    u32WaitCnt++;
    if(u32WaitCnt >= 0x10000)
      break;
  }
  while(1);

  //mbxResult = _MDrv_MBX_SendMsg(pMsg, _mgrMBX.eMbxHostRole);
  MHAL_MBX_ClearAll(pMsg, _mgrMBX.eMbxHostRole);
  mbxResult = MHAL_MBX_Fire(pMsg, _mgrMBX.eMbxHostRole);
  if(E_MBX_SUCCESS == mbxResult)
  {
    MHal_CPUINT_Fire(_mgrMBX.eMbxRole2Cpu[pMsg->eRoleID], _mgrMBX.eMbxRole2Cpu[_mgrMBX.eMbxHostRole]);
  }
  else
  {
    MBX_ERROR("MDrv_MBX_SendMsg: MHAL_MBX_Fire Failed! Result=%d\r\n", mbxResult);
  }

  return mbxResult;
}

MBX_Result MDrv_MBX_SendMsgExt(MBX_Msg *pMsg, MBX_ROLE_ID eSrcRole)
{
  MBX_Result mbxResult = E_MBX_SUCCESS;
  MBXHAL_Fire_Status mbxHalFireStatus;
  U32 u32WaitCnt = 0;
  //parameter check:
  if(NULL == pMsg)
  {
    MBX_ERROR("MDrv_MBX_SendMsgExt: E_MBX_ERR_INVALID_PARAM!\r\n");
    return E_MBX_ERR_INVALID_PARAM;
  }

  do
  {
    mbxResult = MHAL_MBX_GetFireStatus(eSrcRole, pMsg->eRoleID, &mbxHalFireStatus);
    if(E_MBX_SUCCESS != mbxResult)
    {
      MBX_ERROR("MDrv_MBX_SendMsgExt: MHAL_MBX_GetFireStatus failed = %d\r\n ", mbxResult);
      return E_MBX_ERR_INVALID_PARAM;
    }

    if(E_MBXHAL_FIRE_SUCCESS == mbxHalFireStatus)
      break;

    //check If Timeout:
    u32WaitCnt++;
    if(u32WaitCnt >= 0x10000)
      break;
  }
  while(1);

  //mbxResult = _MDrv_MBX_SendMsg(pMsg, _mgrMBX.eMbxHostRole);
  MHAL_MBX_ClearAll(pMsg, eSrcRole);
  mbxResult = MHAL_MBX_Fire(pMsg, eSrcRole);
  if(E_MBX_SUCCESS == mbxResult)
  {
    MHal_CPUINT_Fire(_mgrMBX.eMbxRole2Cpu[pMsg->eRoleID], _mgrMBX.eMbxRole2Cpu[eSrcRole]);
  }
  else
  {
    MBX_ERROR("MDrv_MBX_SendMsg: MDrv_MBX_SendMsgExt Failed! Result=%d\r\n", mbxResult);
  }

  return mbxResult;
}


MBX_Result MDrv_MBX_RecvMsg(MBX_Msg *pMsg, BOOL bBlock)
{
  TickType_t xTicksToWait;
  MBX_Result mbxResult = E_MBX_SUCCESS;

  //parameter check:
  if((pMsg == NULL))
  {
    MBX_ERROR("MDrv_MBX_RecvMsg: E_MBX_ERR_INVALID_PARAM!\r\n");
    return E_MBX_ERR_INVALID_PARAM;
  }

  if(bBlock)
  {
    xTicksToWait = portMAX_DELAY;
    MOS_IRQn_Unmask(_mgrMBX.eMbxIntNum);
  }
  else
  {
    xTicksToWait = 0;
    MOS_IRQn_Mask(_mgrMBX.eMbxIntNum);
  }

  if(xSemaphoreTake(_mgrMBX.tMbxIntSem, xTicksToWait) == pdPASS)
    mbxResult = MHAL_MBX_Recv(pMsg,  _mgrMBX.eMbxHostRole);

  if((E_MBX_SUCCESS != mbxResult) && (E_MBX_ERR_NO_MORE_MSG != mbxResult))
  {
    MBX_ERROR("MDrv_MBX_RecvMsg: Failed! Result=%d\r\n", mbxResult);
  }

  return mbxResult;
}


MBX_Result MDrv_MBX_RecvMsgEnd(MBX_Msg *pMsg)
{
  MBX_Result mbxResult = E_MBX_SUCCESS;

  mbxResult = MHAL_MBX_RecvEnd(pMsg->eRoleID, _mgrMBX.eMbxHostRole, E_MBXHAL_RECV_SUCCESS);

  return mbxResult;
}

MBX_Result MDrv_MBX_RecvMsgEndExt(MBX_Msg *pMsg, BOOL bSuccess)
{
  MBX_Result mbxResult;

  mbxResult = MHAL_MBX_RecvEndExt(pMsg->eRoleID, _mgrMBX.eMbxHostRole, bSuccess);

  return mbxResult;
}

MBX_Result MDrv_MBX_RecvMsgISR(MBX_Msg *pMsg)
{
  MBX_Result mbxResult = E_MBX_SUCCESS;

  //parameter check:
  if((pMsg == NULL))
  {
    MBX_ERROR("MDrv_MBX_RecvMsg: E_MBX_ERR_INVALID_PARAM!\r\n");
    return E_MBX_ERR_INVALID_PARAM;
  }

  mbxResult = MHAL_MBX_Recv(pMsg,  _mgrMBX.eMbxHostRole);

  if((E_MBX_SUCCESS != mbxResult) && (E_MBX_ERR_NO_MORE_MSG != mbxResult))
  {
    MBX_ERROR("MDrv_MBX_SendMsgISR: MDrv_MBX_RecvMsg Failed! Result=%d\r\n", mbxResult);
  }

  return mbxResult;
}

