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
/// @file   drvINTC.c
/// @brief  MStar Interrupt controller DRV Driver DDI
/// @author MStar Semiconductor Inc.
/// @attention
/// <b><em></em></b>
///////////////////////////////////////////////////////////////////////////////////////////////////

#define _DRV_INTC_C

/* Kernel includes. */
#include "mosWrapper.h"

/* driver includes. */
#include "drvINTC.h"
#include "halINTC.h"

/*-----------------------------------------------------------*/

#define INTC_NOPMIRQ_INT_PRIORITY ( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1 )
#define INTC_NOPMFIQ_INT_PRIORITY ( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY )

/*-----------------------------------------------------------*/

typedef struct
{
    IRQn_ISR_t      pdfIntcIsr;
    BOOL            bIsMask;
} IRQn_CTX_t;


static IRQn_CTX_t _tIrqCtxTbl[64];
static IRQn_CTX_t _tFiqCtxTbl[64];

/*-----------------------------------------------------------*/

void NVIC_Handler_NOPMIRQ(void)
{
	U32 u32IRQ;
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  IRQn_ISR_t pdfIntcIsr;

  //UartSendTrace("NVIC_Handler_NOPMIRQ....\n");

  for (u32IRQ = 0; u32IRQ < 64; u32IRQ++)
  {
  	if (!_tIrqCtxTbl[u32IRQ].bIsMask && MHal_INTC_IRQStatus(u32IRQ))
  	{
  		pdfIntcIsr =  _tIrqCtxTbl[u32IRQ].pdfIntcIsr;
      if (pdfIntcIsr)
        pdfIntcIsr();
      //MHal_INTC_IRQClear(u32IRQ);
    }
  }
  /* Call the IntQ test function for this channel. */
  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void NVIC_Handler_NOPMFIQ(void)
{
	U32 u32FIQ;
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  IRQn_ISR_t pdfIntcIsr;

  //UartSendTrace("NVIC_Handler_NOPMFIQ....\n");

  for (u32FIQ = 0; u32FIQ < 64; u32FIQ++)
  {
  	if (!_tFiqCtxTbl[u32FIQ].bIsMask && MHal_INTC_FIQStatus(u32FIQ))
  	{
  		pdfIntcIsr =  _tFiqCtxTbl[u32FIQ].pdfIntcIsr;
      if (pdfIntcIsr)
        pdfIntcIsr();
    }
    MHal_INTC_FIQClear(u32FIQ);
  }
  /* Call the IntQ test function for this channel. */
  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void MDrv_INTC_Init(void)
{
  U32 u32Id;
  for (u32Id = 0; u32Id < 64; u32Id++)
  {
    _tIrqCtxTbl[u32Id].bIsMask = 1;
    _tIrqCtxTbl[u32Id].pdfIntcIsr = 0;
    _tFiqCtxTbl[u32Id].bIsMask = 1;
    _tFiqCtxTbl[u32Id].pdfIntcIsr = 0;
  }

  MOS_IRQn_AttachIsr(NVIC_Handler_NOPMIRQ, NOPMIRQ_IRQn, INTC_NOPMIRQ_INT_PRIORITY);
  MOS_IRQn_Unmask(NOPMIRQ_IRQn);

  MOS_IRQn_AttachIsr(NVIC_Handler_NOPMFIQ, NOPMFIQ_IRQn, INTC_NOPMFIQ_INT_PRIORITY);
  MOS_IRQn_Unmask(NOPMFIQ_IRQn);

}

void MDrv_INTC_IRQAttach(void *pdfIntcIsr, U32 u32IntcNumber)
{
  _tIrqCtxTbl[u32IntcNumber].pdfIntcIsr = (IRQn_ISR_t)pdfIntcIsr;
}

void MDrv_INTC_IRQDetach(U32 u32IntcNumber)
{
  _tIrqCtxTbl[u32IntcNumber].pdfIntcIsr = (IRQn_ISR_t)0;
}

void MDrv_INTC_FIQAttach(void *pdfIntcIsr, U32 u32IntcNumber)
{
	_tFiqCtxTbl[u32IntcNumber].pdfIntcIsr = (IRQn_ISR_t)pdfIntcIsr;
}

void MDrv_INTC_FIQdetach(U32 u32IntcNumber)
{
	_tFiqCtxTbl[u32IntcNumber].pdfIntcIsr = (IRQn_ISR_t)0;
}

void MDrv_INTC_IRQSetMask(U32 u32IntcNumber, BOOL bSetMask)
{
  if (bSetMask)
  {
    MHal_INTC_IRQMask(u32IntcNumber);
	  _tIrqCtxTbl[u32IntcNumber].bIsMask = 1;
  }
  else
  {
    MHal_INTC_IRQUnMask(u32IntcNumber);
	  _tIrqCtxTbl[u32IntcNumber].bIsMask = 0;
	}
}

void MDrv_INTC_FIQSetMask(U32 u32IntcNumber, BOOL bSetMask)
{
  if (bSetMask)
  {
	  MHal_INTC_FIQMask(u32IntcNumber);
	  _tFiqCtxTbl[u32IntcNumber].bIsMask = 1;
  }
  else
  {
	  MHal_INTC_FIQUnMask(u32IntcNumber);
	  _tFiqCtxTbl[u32IntcNumber].bIsMask = 0;
	}
}

