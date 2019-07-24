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
/// @file   drvPM.c
/// @brief  MStar Power Management DRV Driver DDI
/// @author MStar Semiconductor Inc.
/// @attention
/// <b><em></em></b>
///////////////////////////////////////////////////////////////////////////////////////////////////

#define _DRV_INTC_C

/* Kernel includes. */
#include "mosWrapper.h"

/* driver includes. */
#include "drvPM.h"
#include "drvMBX.h"
#include "halPM.h"
#include "halTimer.h"
#include "halBDMA.h"
#include "halCPU.h"

#define PM_MSG(fmt, args...) MOS_DBG_PRINT(MOS_DBG_LEVEL_PM, "[PM] " fmt, ##args)
#define PM_ERROR(fmt, args...) MOS_DBG_ERROR("[PM] " fmt, ##args);

#define PM_INT_PRIORITY		( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY - 1 )

typedef enum
{
  E_PM_STATUS_WAKEUP = 0,
  E_PM_STATUS_SLEEP,
  E_PM_STATUS_MAX = 0xFF,
} PM_Status_e;

static PM_Status_e _ePMStatus = E_PM_STATUS_WAKEUP;

#if defined(CONFIG_PM_GPIO)
static PM_GPIO_e _ePMGPIOIn = E_PM_GPIO_MAX;
static PM_GPIO_e _ePMGPIOOut = E_PM_GPIO_MAX;
static BOOL _bPMGPIOInInitHigh;
static BOOL _bPMGPIOOutInitHigh;

void NVIC_Handler_PM(void)
{
  MBX_Msg tMsg;

  if (_ePMGPIOIn == E_PM_GPIO_MAX)
  {
    PM_MSG("NVIC_Handler_PM: Something worng\n");
  }
  else if (Mhal_PM_GPIO_IN(_ePMGPIOIn)^_bPMGPIOInInitHigh)
  {
    tMsg.eRoleID = E_MBX_ROLE_CP;
    tMsg.u8Index  = E_MBX_MSG_SLEEP;
    tMsg.u8ParameterCount = 1;
    tMsg.u8Parameters[0] = 0;
    MDrv_MBX_SendMsgExt(&tMsg, E_MBX_ROLE_HK);

    PM_MSG("NVIC_Handler_PM: PM GPIO resume\n");
  }
  else
    PM_MSG("NVIC_Handler_PM: do nothing\n");
}

BOOL MDrv_PM_GPIO_OEN(PM_GPIO_e eGPIO, BOOL bOEn, BOOL bInitHigh)
{
  if (eGPIO >= E_PM_GPIO_MAX || eGPIO <= E_PM_GPIO2)
    return FALSE;

  if (bOEn)
  {
    Mhal_PM_GPIO_OEN((U8)eGPIO, 1);
    Mhal_PM_GPIO_PULL((U8)eGPIO, (bInitHigh?1:0));

    _ePMGPIOOut = eGPIO;
    _bPMGPIOOutInitHigh = bInitHigh;
  }
  else
  {
    Mhal_PM_GPIO_OEN((U8)eGPIO, 0);

    MOS_IRQn_AttachIsr(NVIC_Handler_PM, PM_GPIO0_IRQn + eGPIO, PM_INT_PRIORITY);
  //  MOS_IRQn_Unmask(PM_GPIO0_IRQn + eGPIO);

    _ePMGPIOIn = eGPIO;
    _bPMGPIOInInitHigh = bInitHigh;
  }

  PM_MSG("MDrv_PM_GPIO_OEN: GPIO(%d), OEN = %d, Init = %d\n", eGPIO, bOEn, bInitHigh);

  return TRUE;

}
#endif

void MDrv_PM_Suspend(void)
{
  //if (_ePMmode == E_PM_MODE_NORMAL)
  {
    if(_ePMStatus == E_PM_STATUS_WAKEUP)
    {
      Mhal_PM_Suspend();

#if defined(CONFIG_PM_GPIO)
      if (_ePMGPIOOut != E_PM_GPIO_MAX)
      {
        Mhal_PM_GPIO_OEN((U8)_ePMGPIOOut, 1);
#if defined(CONFIG_PM_GPIO_CUST)
        Mhal_PM_GPIO_PULL((U8)_ePMGPIOOut, (_bPMGPIOOutInitHigh? 1: 0));
#else
        Mhal_PM_GPIO_PULL((U8)_ePMGPIOOut, (_bPMGPIOOutInitHigh? 0: 1));
#endif
      }

      if(_ePMGPIOIn != E_PM_GPIO_MAX)
      {
        MOS_IRQn_Unmask(PM_GPIO0_IRQn + _ePMGPIOIn);
      }
#endif

      _ePMStatus = E_PM_STATUS_SLEEP;

      //PM_MSG("MDrv_PM_Suspend: Suspend ACPU (%d)\n", MHal_Timer_GetTick()/TIMER_DELAY_1ms);
      PM_MSG("MDrv_PM_Suspend: Suspend ACPU (%d)\r\n",xTaskGetTickCount()*portTICK_PERIOD_MS);
    }
  }
}

void MDrv_PM_Resume(void)
{
  //if (_ePMmode == E_PM_MODE_NORMAL)
  {
    if(_ePMStatus == E_PM_STATUS_SLEEP)
    {
      Mhal_PM_Resume();
      MHal_BDMA_Resume(); //restore cm4 bdma

#if defined(CONFIG_PM_GPIO)
      if (_ePMGPIOOut != E_PM_GPIO_MAX)
      {
        Mhal_PM_GPIO_OEN((U8)_ePMGPIOOut, 1);
#if defined(CONFIG_PM_GPIO_CUST)
        Mhal_PM_GPIO_PULL((U8)_ePMGPIOOut, (_bPMGPIOOutInitHigh? 0: 1));
#else
        Mhal_PM_GPIO_PULL((U8)_ePMGPIOOut, (_bPMGPIOOutInitHigh? 1: 0));
#endif
      }

      if(_ePMGPIOIn != E_PM_GPIO_MAX)
      {
        MOS_IRQn_Mask(PM_GPIO0_IRQn + _ePMGPIOIn);
      }
#endif


      _ePMStatus = E_PM_STATUS_WAKEUP;

      //PM_MSG("MDrv_PM_Resume: Resume ACPU (%d)\n", MHal_Timer_GetTick()/TIMER_DELAY_1ms);
      PM_MSG("MDrv_PM_Resume: Resume ACPU (%d)\r\n",xTaskGetTickCount()*portTICK_PERIOD_MS);
    }
  }
}

void MDrv_PM_LowPower(void)
{
  //PM_MSG("MDrv_PM_LowPower: \r\n");
  //MHAL_AUD_LowPower();
  MHal_CPU_SetFreq(E_CPU_FREQ_LOW);
}

void MDrv_PM_HighPower(void)
{
  //PM_MSG("MDrv_PM_HighPower: \r\n");
  //MHAL_AUD_HighPower();
  MHal_CPU_SetFreq(E_CPU_FREQ_HIGH);
}

