//<MStar Software>
//******************************************************************************
// MStar Software
// Copyright (c) 2010 - 2012 MStar Semiconductor, Inc. All rights reserved.
// All software, firmware and related documentation herein ("MStar Software") are
// intellectual property of MStar Semiconductor, Inc. ("MStar") and protected by
// law, including, but not limited to, copyright law and international treaties.
// Any use, modification, reproduction, retransmission, or republication of all
// or part of MStar Software is expressly prohibited, unless prior written
// permission has been granted by MStar.
//
// By accessing, browsing and/or using MStar Software, you acknowledge that you
// have read, understood, and agree, to be bound by below terms ("Terms") and to
// comply with all applicable laws and regulations:
//
// 1. MStar shall retain any and all right, ownership and interest to MStar
//    Software and any modification/derivatives thereof.
//    No right, ownership, or interest to MStar Software and any
//    modification/derivatives thereof is transferred to you under Terms.
//
// 2. You understand that MStar Software might include, incorporate or be
//    supplied together with third party`s software and the use of MStar
//    Software may require additional licenses from third parties.
//    Therefore, you hereby agree it is your sole responsibility to separately
//    obtain any and all third party right and license necessary for your use of
//    such third party`s software.
//
// 3. MStar Software and any modification/derivatives thereof shall be deemed as
//    MStar`s confidential information and you agree to keep MStar`s
//    confidential information in strictest confidence and not disclose to any
//    third party.
//
// 4. MStar Software is provided on an "AS IS" basis without warranties of any
//    kind. Any warranties are hereby expressly disclaimed by MStar, including
//    without limitation, any warranties of merchantability, non-infringement of
//    intellectual property rights, fitness for a particular purpose, error free
//    and in conformity with any international standard.  You agree to waive any
//    claim against MStar for any loss, damage, cost or expense that you may
//    incur related to your use of MStar Software.
//    In no event shall MStar be liable for any direct, indirect, incidental or
//    consequential damages, including without limitation, lost of profit or
//    revenues, lost or damage of data, and unauthorized system use.
//    You agree that this Section 4 shall still apply without being affected
//    even if MStar Software has been modified by MStar in accordance with your
//    request or instruction for your use, except otherwise agreed by both
//    parties in writing.
//
// 5. If requested, MStar may from time to time provide technical supports or
//    services in relation with MStar Software to you for your use of
//    MStar Software in conjunction with your or your customer`s product
//    ("Services").
//    You understand and agree that, except otherwise agreed by both parties in
//    writing, Services are provided on an "AS IS" basis and the warranty
//    disclaimer set forth in Section 4 above shall apply.
//
// 6. Nothing contained herein shall be construed as by implication, estoppels
//    or otherwise:
//    (a) conferring any license or right to use MStar name, trademark, service
//        mark, symbol or any other identification;
//    (b) obligating MStar or any of its affiliates to furnish any person,
//        including without limitation, you and your customers, any assistance
//        of any kind whatsoever, or any information; or
//    (c) conferring any license or right under any intellectual property right.
//
// 7. These terms shall be governed by and construed in accordance with the laws
//    of Taiwan, R.O.C., excluding its conflict of law rules.
//    Any and all dispute arising out hereof or related hereto shall be finally
//    settled by arbitration referred to the Chinese Arbitration Association,
//    Taipei in accordance with the ROC Arbitration Law and the Arbitration
//    Rules of the Association by three (3) arbitrators appointed in accordance
//    with the said Rules.
//    The place of arbitration shall be in Taipei, Taiwan and the language shall
//    be English.
//    The arbitration award shall be final and binding to both parties.
//
//******************************************************************************
//<MStar Software>
////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2008-2009 MStar Semiconductor, Inc.
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
/// file    halAudio.c
/// @brief  MStar Audio HAL level DDI
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

#define _HAL_AUD_C

#include <string.h>

#include "mosWrapper.h"

#include "haydn_reg.h"
#include "halAudio.h"

//#define __ENABLE_MAILBOX_FOR_SCRIPT_LOAD__
//#define __ENABLE_MSG_PRINT__
//#define __ENABLE_MIU_PIPE_FLUSH__
//#define __ENABLE_CPU_CACHE_FLUSH_AND_INVALIDATE_

#define AUD_MSG(fmt, args...) MOS_DBG_PRINT(MOS_DBG_LEVEL_AUD, "[AUD] " fmt, ##args)
#define AUD_ERR(fmt, args...) MOS_DBG_ERROR("[AUD ERR] " fmt, ##args);

#if 0
void* memset(void *s, int c, size_t len)
{
  unsigned char *dst = s;
  while(len > 0)
  {
    *dst = (unsigned char) c;
    dst++;
    len--;
  }
  return s;
}

extern void *memcpy(void *, const void *, size_t);
//extern void *memset(void *dst, int val, size_t n);
extern void _chip_flush_miu_pipe(void);
#endif


#ifdef __ENABLE_MIU_PIPE_FLUSH__
inline void _chip_flush_miu_pipe(void)
{
#define  reg_flush_op_on_fire           (0x1F000000 + 0x102200*2 + 0x05*4)
#define  reg_Flush_miu_pipe_done_flag   (0x1F000000 + 0x102200*2 + 0x10*4)
  unsigned short dwReadData = 0;

  //toggle the flush miu pipe fire bit
  *(volatile unsigned short *)(reg_flush_op_on_fire) = 0x0;
  *(volatile unsigned short *)(reg_flush_op_on_fire) = 0x1;

  do
  {
    dwReadData = *(volatile unsigned short *)(reg_Flush_miu_pipe_done_flag);
    dwReadData &= (1 << 12); //Check Status of Flush Pipe Finish

  }
  while(dwReadData == 0);
}
#endif

void SysMemFlush(U32 addr, U32 size)
{
  (void) addr;
  (void) size;
#ifdef __ENABLE_CPU_CACHE_FLUSH_AND_INVALIDATE_
  flush_dcache_range((U32)_gpDmaRdPtr, (U32)size);
#endif

#ifdef __ENABLE_MIU_PIPE_FLUSH__
  _chip_flush_miu_pipe();
#endif
}

/**
 * \brief Audio DMA
 */
typedef enum
{
  BACH_DMA_READER,
  BACH_DMA_WRITER
} BachDma_e;

#define MCU_MIU_BASE_ADDR           IMI_BASE_ADDR
#define MCU_RIU_BASE_ADDR           RIU_BASE_ADDR

#define MAILBOX_REG_BANK          GET_REG8_ADDR( MCU_RIU_BASE_ADDR, 0x3300 )

//#define HAYDN_REG_BANK_0          GET_REG8_ADDR( MCU_RIU_BASE_ADDR, 0x32000 )
#define HAYDN_REG_BANK_1          GET_REG8_ADDR( MCU_RIU_BASE_ADDR, 0x32100 )
//#define HAYDN_REG_BANK_2          GET_REG8_ADDR( MCU_RIU_BASE_ADDR, 0x32200 )
//#define HAYDN_REG_BANK_3          GET_REG8_ADDR( MCU_RIU_BASE_ADDR, 0x32300 )
#define HAYDN_REG_BANK_4          GET_REG8_ADDR( MCU_RIU_BASE_ADDR, 0x32400 )
//#define HAYDN_REG_BANK_5          GET_REG8_ADDR( MCU_RIU_BASE_ADDR, 0x32500 )


#define MIU_WORD_BYTE_SIZE          (16)
#define DMA_INTERNAL_FIFO_SIZE      (8)

static U8 *_gpDmaWrBuf = NULL;
static U8 *_gpDmaWrPtr = NULL;
static U32 _gnDmaWrBufSize;

//static BachDmaWriter_e _geDmaWriter = BACH_DMA_WRITER2;
//static BachDmaReader_e _geDmaReader = BACH_DMA_READER1;

Bool gbDmaWrEnable = False;

/**
 * \brief DMA interrupt clear
 */
typedef enum
{
  BACH_INT_CLR_WR_FULL = 0,
  BACH_INT_CLR_RD_EMPTY,
} BachDmaIntClr_e;

#define WRITE_BYTE(_reg, _val)      (*((volatile U8*)(_reg)))  = (U8)(_val)
/************************************************************************
************************************************************************/
/*static void _BachWriteReg(U32 nAddr, U8 nValue)
{
    WRITE_BYTE(BACH_RIU_BASE_ADDR + ((nAddr) << 1) - ((nAddr) & 1), nValue);
}*/

#ifdef __ENABLE_MAILBOX_FOR_SCRIPT_LOAD__
//cytsai:
static U16 _BachReadMailboxReg(U8 nAddr)
{
  //volatile U16 *pnReg = (volatile U16*)(BACH_RIU_BASE_ADDR + (BACH_REG_Mailobox + nAddr) * 2);
  volatile U16 *pnReg = (volatile U16*)(MAILBOX_REG_BANK + (nAddr * 2));
  return (*pnReg);
}
#endif

static void _HaydnWriteReg(U32 nAddr, U16 nValue)
{
  volatile U16 *pnReg = (volatile U16*)(MCU_RIU_BASE_ADDR + (nAddr) * 2);
  //UartSendTrace("BachWriteReg\n");// MSG("BachWriteReg\n");   //printf("naddr 0x%x!!!!!!!\n",pnReg);

  *pnReg = nValue;
}

static U16 _HaydnReadReg(U32 nAddr)
{
  volatile U16 *pnReg = (volatile U16*)(MCU_RIU_BASE_ADDR + (nAddr) * 2);
  //UartSendTrace("BachWriteReg\n");// MSG("BachWriteReg\n");   //printf("naddr 0x%x!!!!!!!\n",pnReg);

  return (*pnReg);
}

static void _HaydnInsertReg8(U32 nAddr, U8 nMask, U8 nVal)
{
  INSREG8(GET_REG8_ADDR(MCU_RIU_BASE_ADDR, nAddr), nMask, nVal);
}

static void _HaydnInsertReg16(U32 nAddr, U16 nMask, U16 nVal)
{
  INSREG16(GET_REG8_ADDR(MCU_RIU_BASE_ADDR, nAddr), nMask, nVal);
}

/**
 * \brief write the given value (24 bits) in the given register (6 bits)
 * \return >=0 if OK
 */
static void _HaydnWriteBank1Reg(U8 nAddr, U16 nValue)
{
  //volatile U16 *pnReg = (volatile U16*)(BACH_RIU_BASE_ADDR + (BACH_REG_BANK_1 + nAddr) * 2);
  volatile U16 *pnReg = (volatile U16*)(HAYDN_REG_BANK_1 + (nAddr) * 2);
  *pnReg = nValue;
}

static void _HaydnWriteBank4Reg(U8 nAddr, U16 nValue)
{
  //volatile U16 *pnReg = (volatile U16*)(BACH_RIU_BASE_ADDR + (BACH_REG_BANK_1 + nAddr) * 2);
  volatile U16 *pnReg = (volatile U16*)(HAYDN_REG_BANK_4 + (nAddr) * 2);
  *pnReg = nValue;
}

/**
 * \brief read in the given register (8 bits)
 * \return the value of the register (16 bits) if OK
 * <0 if error
 */
//static
U16 _HaydnReadBank1Reg(U8 nAddr)
{
  //volatile U16 *pnReg = (volatile U16*)(BACH_RIU_BASE_ADDR + (BACH_REG_BANK_1 + nAddr) * 2);
  volatile U16 *pnReg = (volatile U16*)(HAYDN_REG_BANK_1 + (nAddr) * 2);
  return (*pnReg);
}

U16 _HaydnReadBank4Reg(U8 nAddr)
{
  //volatile U16 *pnReg = (volatile U16*)(BACH_RIU_BASE_ADDR + (BACH_REG_BANK_1 + nAddr) * 2);
  volatile U16 *pnReg = (volatile U16*)(HAYDN_REG_BANK_4 + (nAddr) * 2);
  return (*pnReg);
}


//cytsai: mark
#if 0
/**
 * \brief write the given value (24 bits) in the given register (6 bits)
 * \return >=0 if OK
 */
static void _BachWriteBank2Reg(U8 nAddr, U16 nValue)
{
  volatile U16 *pnReg = (volatile U16*)(BACH_RIU_BASE_ADDR + (BACH_REG_BANK_2 + nAddr) * 2);
  *pnReg = nValue;
}

/**
 * \brief read in the given register (8 bits)
 * \return the value of the register (16 bits) if OK
 * <0 if error
 */

/*
static U16 _BachReadBank2Reg(U8 nAddr)
{
   volatile U16 *pnReg = (volatile U16*)(BACH_RIU_BASE_ADDR + (BACH_REG_BANK_2 + nAddr) * 2);
   return (*pnReg);
}
*/
#endif


void HaydnDmaReset(void)
{
  U16 nConfigValue;

  /* PR98079 - Reset read and write size. Otherwise Bach update level count if REG_WR_TRIG is 1. */
  _HaydnWriteBank4Reg(HAYDN_DMA_W1_CFG_4, 0);

  nConfigValue = _HaydnReadBank4Reg(HAYDN_DMA_W1_CFG_0);
  nConfigValue |= REG_SW_RST_DMA;
  _HaydnWriteBank4Reg(HAYDN_DMA_W1_CFG_0, nConfigValue);
  MOS_uDelay(2);
  nConfigValue &= ~REG_SW_RST_DMA;
  _HaydnWriteBank4Reg(HAYDN_DMA_W1_CFG_0, nConfigValue);

  nConfigValue = _HaydnReadBank4Reg(HAYDN_DMA_W1_CFG_0);
  nConfigValue |= (REG_WR_SWAP | REG_MIU_REQUSTER_ENABLE);
  _HaydnWriteBank4Reg(HAYDN_DMA_W1_CFG_0, nConfigValue);

  /* reset pointer */
  //_gpDmaWrPtr = _gpDmaWrBuf = NULL;
  _gpDmaWrPtr = _gpDmaWrBuf;
}

U32 HaydnDmaWrLevelCnt(void)
{
  U16 nConfigValue;
  U32 nWrevelByteSize;

  nConfigValue = _HaydnReadBank4Reg(HAYDN_DMA_W1_CFG_0);
  nConfigValue |= REG_WR_LEVEL_CNT_MASK;
  _HaydnWriteBank4Reg(HAYDN_DMA_W1_CFG_0, nConfigValue);

  nConfigValue = _HaydnReadBank4Reg(HAYDN_DMA_W1_STS_0);
  nConfigValue = ((nConfigValue > DMA_INTERNAL_FIFO_SIZE) ? (nConfigValue - DMA_INTERNAL_FIFO_SIZE) : 0);
  nWrevelByteSize = nConfigValue * MIU_WORD_BYTE_SIZE;

  nConfigValue = _HaydnReadBank4Reg(HAYDN_DMA_W1_CFG_0);
  nConfigValue &= ~REG_WR_LEVEL_CNT_MASK;
  _HaydnWriteBank4Reg(HAYDN_DMA_W1_CFG_0, nConfigValue);

  return nWrevelByteSize;
}

void HaydnDmaSetWrMcuBuf(U8 *pBuf, U32 nBufSize)
{
  _gpDmaWrBuf = _gpDmaWrPtr = pBuf;
  _gnDmaWrBufSize = nBufSize;
  //flush_dcache_range((U32)pBuf, (U32)pBuf + nBufSize);
}

void HaydnDmaSetWrMiuAddr(U32 nBufAddr, U32 nBufSize)
{
  U16 nMiuAddrLo, nMiuAddrHi, nMiuSize;
  U32 nOffset;

  nOffset = nBufAddr - MCU_MIU_BASE_ADDR;
  nMiuAddrLo = (nOffset / MIU_WORD_BYTE_SIZE) & REG_WR_BASE_ADDR_LO_MSK;
  nMiuAddrHi = ((nOffset / MIU_WORD_BYTE_SIZE) >> REG_WR_BASE_ADDR_HI_OFFSET) & REG_WR_BASE_ADDR_HI_MSK;
  nMiuSize = (nBufSize / MIU_WORD_BYTE_SIZE) & REG_WR_BUFF_SIZE_MSK;

  _HaydnWriteBank4Reg(HAYDN_DMA_W1_CFG_1, nMiuAddrLo);

  _HaydnWriteBank4Reg(HAYDN_DMA_W1_CFG_2, nMiuAddrHi);

  _HaydnWriteBank4Reg(HAYDN_DMA_W1_CFG_3, nMiuSize);
}


void HaydnDmaSetWrOverrunTh(U32 nOverrunTh)
{
  U16 nMiuOverrunTh;
  nMiuOverrunTh = (nOverrunTh / MIU_WORD_BYTE_SIZE) & REG_WR_OVERRUN_TH_MSK;
  nMiuOverrunTh += DMA_INTERNAL_FIFO_SIZE; //lvl count include local buffer size
  _HaydnWriteBank4Reg(HAYDN_DMA_W1_CFG_5, nMiuOverrunTh);
}


U32 HaydnDmaReadWrData(U8 *pData, U32 nDataSize)
{
  U32 nReadSize, nRemainSize, nBufDataSize;
  U16 nConfigValue;

  nBufDataSize = HaydnDmaWrLevelCnt();
  if(nBufDataSize == 0)
    return 0;

  if(nDataSize > nBufDataSize)
  {
    nDataSize = nBufDataSize;
    //nDataSize = nDataSize & ~(BACH_ARM_CACHE_LINE_ALIGN-1);
    //nDataSize = nDataSize / MIU_WORD_BYTE_SIZE * MIU_WORD_BYTE_SIZE;
  }

  //printf("_gpDmaWrPtr :0x%x\n",_gpDmaWrPtr);

  if((U32)_gpDmaWrPtr + nDataSize <= (U32)_gpDmaWrBuf + _gnDmaWrBufSize)
  {
    nReadSize = nDataSize;
    nRemainSize = 0;

    //flush_dcache_range((U32)_gpDmaWrPtr, (U32)_gpDmaWrPtr + nReadSize);
    SysMemFlush((U32)_gpDmaWrPtr, nReadSize);
    memcpy(pData, _gpDmaWrPtr, nReadSize);
    //printf("pData : %x ,_gpDmaWrPtr : %x,_gpDmaWrPtr[0] : %x\n",pData[0],_gpDmaWrPtr,_gpDmaWrPtr[0]);
    //printf("_gpDmaWrPtr[] :%x %x %x %x\n", _gpDmaWrPtr[0], _gpDmaWrPtr[1], _gpDmaWrPtr[2], _gpDmaWrPtr[3]);
    _gpDmaWrPtr += nReadSize;

    if((U32)_gpDmaWrPtr == (U32)_gpDmaWrBuf + _gnDmaWrBufSize)
    {
      _gpDmaWrPtr = _gpDmaWrBuf;
    }
  }
  else
  {
    nReadSize = (U32)_gpDmaWrBuf + _gnDmaWrBufSize - (U32)_gpDmaWrPtr;
    nRemainSize = nDataSize - nReadSize;

    //flush_dcache_range((U32)_gpDmaWrPtr, (U32)_gpDmaWrPtr + nReadSize);
    SysMemFlush((U32)_gpDmaWrPtr, nReadSize);
    memcpy(pData, _gpDmaWrPtr, nReadSize);
    _gpDmaWrPtr = _gpDmaWrBuf;
  }
  //invalidate_dcache_range((U32)_gpDmaWrPtr, );

  if(nRemainSize > 0)
  {
    //flush_dcache_range((U32)_gpDmaWrPtr, (U32)_gpDmaWrPtr + nRemainSize);
    SysMemFlush((U32)_gpDmaWrPtr, nRemainSize);
    memcpy(pData + nReadSize, _gpDmaWrPtr, nRemainSize);
    _gpDmaWrPtr += nRemainSize;
  }

  nConfigValue = (nDataSize / MIU_WORD_BYTE_SIZE) & REG_WR_SIZE_MSK;
  _HaydnWriteBank4Reg(HAYDN_DMA_W1_CFG_4, nConfigValue);

  nConfigValue = _HaydnReadBank4Reg(HAYDN_DMA_W1_CFG_0);
  nConfigValue |= REG_WR_TRIG;
  _HaydnWriteBank4Reg(HAYDN_DMA_W1_CFG_0, nConfigValue);

  nConfigValue &= ~REG_WR_TRIG;
  _HaydnWriteBank4Reg(HAYDN_DMA_W1_CFG_0, nConfigValue);

  return nDataSize;
}

void HaydnDmaCtrlWrInt(Bool bLocalFull, Bool bOverrun, Bool bFull)
{
  U16 nConfigValue;

  nConfigValue = _HaydnReadBank4Reg(HAYDN_DMA_W1_CFG_8);

  /* local full interrupt */
  if(!bLocalFull)
  {
    nConfigValue |= REG_WR_LOCAL_FULL_INT_EN; //1:mask, 0:unmask
  }
  else
  {
    nConfigValue &= ~REG_WR_LOCAL_FULL_INT_EN;
  }

  /* Overrun interrupt */
  if(!bOverrun)
  {
    nConfigValue |= REG_WR_OVERRUN_INT_EN;
  }
  else
  {
    nConfigValue &= ~REG_WR_OVERRUN_INT_EN;
  }

  /* Empty interrupt */
  if(!bFull)
  {
    nConfigValue |= REG_WR_FULL_INT_EN;
  }
  else
  {
    nConfigValue &= ~REG_WR_FULL_INT_EN;
  }

  _HaydnWriteBank4Reg(HAYDN_DMA_W1_CFG_8, nConfigValue);
}

void HaydnDmaEnableWr(Bool bEnable)
{
  U16 nConfigValue;

  nConfigValue = _HaydnReadBank4Reg(HAYDN_DMA_W1_CFG_0);

  if(bEnable)
  {
    nConfigValue |= REG_WR_ENABLE;
  }
  else
  {
    nConfigValue &= ~REG_WR_ENABLE;
  }

  _HaydnWriteBank4Reg(HAYDN_DMA_W1_CFG_0, nConfigValue);
}

Bool HaydnDmaWrIsOverrun(void)
{
  U16 nConfigValue;

  nConfigValue = _HaydnReadBank4Reg(HAYDN_DMA_W1_STS_1);

  if(nConfigValue & REG_WR_OVERRUN_FLAG)
  {
    return True;
  }
  else
  {
    return False;
  }
}

Bool HaydnDmaWrIsFull(void)
{
  U16 nConfigValue;

  nConfigValue = _HaydnReadBank4Reg(HAYDN_DMA_W1_STS_1);

  if(nConfigValue & REG_WR_FULL_FLAG)
  {
    return True;
  }
  else
  {
    return False;
  }
}

void HaydnDmaIntStatus(U16 *pnStatus)
{
  U16 nConfigValue;

  nConfigValue = _HaydnReadBank4Reg(HAYDN_DMA_W1_STS_1);
  *pnStatus = nConfigValue;
}

void HaydnDmaIntClear(BachDmaIntClr_e eInt)
{
  U16 nConfigValue;

  nConfigValue = _HaydnReadBank4Reg(HAYDN_DMA_W1_CFG_0);

  switch(eInt)
  {
    case BACH_INT_CLR_WR_FULL:
      nConfigValue |= REG_WR_FULL_FLAG_CLR;
      _HaydnWriteBank4Reg(HAYDN_DMA_W1_CFG_0, nConfigValue);
      nConfigValue &= ~REG_WR_FULL_FLAG_CLR;
      _HaydnWriteBank4Reg(HAYDN_DMA_W1_CFG_0, nConfigValue);
      break;

    case BACH_INT_CLR_RD_EMPTY:
      break;

    default:
      return;
  }
}

#if defined(CONFIG_AUD_AMIC)

BOOL HaydnInmux0Gain(U8 u8Level)
{
  U16 nConfigValue;

  nConfigValue = _HaydnReadBank1Reg(HAYDN_AUSDM_CFG5);
  nConfigValue &= ~(REG_SEL_GAIN_INMUX0_L_MSK | REG_SEL_GAIN_INMUX0_R_MSK);
  nConfigValue |= (((U16)u8Level<<REG_SEL_GAIN_INMUX0_L_POS)&REG_SEL_GAIN_INMUX0_L_MSK);
  nConfigValue |= (((U16)u8Level<<REG_SEL_GAIN_INMUX0_R_POS)&REG_SEL_GAIN_INMUX0_R_MSK);
  _HaydnWriteBank1Reg(HAYDN_AUSDM_CFG5, nConfigValue);

  return TRUE;
}

BOOL HaydnInmux1Gain(U8 u8Level)
{
  U16 nConfigValue;

  nConfigValue = _HaydnReadBank1Reg(HAYDN_AUSDM_CFG16);
  nConfigValue &= ~(REG_SEL_GAIN_INMUX1_L_MSK | REG_SEL_GAIN_INMUX1_R_MSK);
  nConfigValue |= (((U16)u8Level<<REG_SEL_GAIN_INMUX1_L_POS)&REG_SEL_GAIN_INMUX1_L_MSK);
  nConfigValue |= (((U16)u8Level<<REG_SEL_GAIN_INMUX1_R_POS)&REG_SEL_GAIN_INMUX1_R_MSK);
  _HaydnWriteBank1Reg(HAYDN_AUSDM_CFG16, nConfigValue);

  return TRUE;
}


BOOL HaydnAMic0Gain(U8 u8Level)
{
  U16 nConfigValue;
  nConfigValue = _HaydnReadBank1Reg(HAYDN_AUSDM_CFG14);
  nConfigValue &= ~(REG_SEL_GAIN_R_MICAMP0_MSK | REG_SEL_GAIN_L_MICAMP0_MSK);
  nConfigValue |= (((U16)u8Level<<REG_SEL_GAIN_R_MICAMP0_POS)&REG_SEL_GAIN_R_MICAMP0_MSK);
  nConfigValue |= (((U16)u8Level<<REG_SEL_GAIN_L_MICAMP0_POS)&REG_SEL_GAIN_L_MICAMP0_MSK);
  _HaydnWriteBank1Reg(HAYDN_AUSDM_CFG14, nConfigValue);
  return TRUE;
}

BOOL HaydnAMic1Gain(U8 u8Level)
{
  U16 nConfigValue;
  nConfigValue = _HaydnReadBank1Reg(HAYDN_AUSDM_CFG14);
  nConfigValue &= ~(REG_SEL_GAIN_R_MICAMP1_MSK | REG_SEL_GAIN_L_MICAMP1_MSK);
  nConfigValue |= (((U16)u8Level<<REG_SEL_GAIN_R_MICAMP1_POS)&REG_SEL_GAIN_R_MICAMP1_MSK);
  nConfigValue |= (((U16)u8Level<<REG_SEL_GAIN_L_MICAMP1_POS)&REG_SEL_GAIN_L_MICAMP1_MSK);
  _HaydnWriteBank1Reg(HAYDN_AUSDM_CFG14, nConfigValue);
  return TRUE;
}

void HaydnMic01PowerOn(BOOL bEn)
{
  if(bEn)
  {
    _HaydnInsertReg8(0x321e3, 0x03, 0x00);//PD_INMUX
    _HaydnInsertReg8(0x321f0, 0x50, 0x00);//PD_MICAMP[4][6]
    _HaydnInsertReg8(0x321da, 0x02, 0x00);// PD_ADC0[1]
  }
  else
  {
    _HaydnInsertReg8(0x321e3, 0x03, 0x03);//PD_INMUX
    _HaydnInsertReg8(0x321f0, 0x50, 0x50);//PD_MICAMP[4][6]
    _HaydnInsertReg8(0x321da, 0x02, 0x02);// PD_ADC0[1]
  }

}

void HaydnMic23PowerOn(BOOL bEn)
{
  if(bEn)
  {
    _HaydnInsertReg8(0x321d9, 0x03, 0x00);//PD_INMUX
    _HaydnInsertReg8(0x321f0, 0xA0, 0x00);//PD_MICAMP[5][7]
    _HaydnInsertReg8(0x321da, 0x01, 0x00);// PD_ADC1[0]
  }
  else
  {
    _HaydnInsertReg8(0x321d9, 0x03, 0x03);//PD_INMUX
    _HaydnInsertReg8(0x321f0, 0xA0, 0xA0);//PD_MICAMP[5][7]
    _HaydnInsertReg8(0x321da, 0x01, 0x01);// PD_ADC1[0]
  }

}

void MHAL_AUD_AMicAdcGain(U8 u8Level)
{
  HaydnInmux0Gain(u8Level);
  HaydnInmux1Gain(u8Level);
}


void MHAL_AUD_AMicMicAmpGain(U8 u8Level)
{
  HaydnAMic0Gain(u8Level);
  HaydnAMic1Gain(u8Level);
}

#if 0
void MHAL_AUD_AMIC(void)
{
  /* AMIC path */
  _HaydnInsertReg8(0x321de, 0xff, 0x00);
  _HaydnInsertReg8(0x321df, 0xff, 0x78);
  _HaydnInsertReg8(0x321e0, 0xff, 0x00);
  _HaydnInsertReg8(0x321e1, 0xff, 0x00);
  _HaydnInsertReg8(0x321e2, 0xff, 0x00);
  _HaydnInsertReg8(0x321e3, 0xff, 0x10);
  _HaydnInsertReg8(0x321d8, 0xff, 0x00);
  _HaydnInsertReg8(0x321d9, 0xff, 0x00);
  _HaydnInsertReg8(0x321f6, 0xff, 0x40);
  _HaydnInsertReg8(0x321f7, 0xff, 0x92);
  _HaydnInsertReg8(0x321e6, 0xff, 0x00);
  _HaydnInsertReg8(0x321e7, 0xff, 0x00);
  _HaydnInsertReg8(0x321e8, 0xff, 0x00);
  _HaydnInsertReg8(0x321e9, 0xff, 0x00);
  _HaydnInsertReg8(0x321ea, 0xff, 0x00);
  _HaydnInsertReg8(0x321eb, 0xff, 0x00);
  _HaydnInsertReg8(0x321ec, 0xff, 0x00);
  _HaydnInsertReg8(0x321ed, 0xff, 0x18);
  _HaydnInsertReg8(0x321ee, 0xff, 0x03);
  _HaydnInsertReg8(0x321ef, 0xff, 0x03);
  _HaydnInsertReg8(0x321f2, 0xff, 0x01);
  _HaydnInsertReg8(0x321f3, 0xff, 0x0f);
  _HaydnInsertReg8(0x321dc, 0xff, 0x00);  //PD DAC
  _HaydnInsertReg8(0x321dd, 0xff, 0x00);//power on DAC
  _HaydnInsertReg8(0x321da, 0xff, 0x28);
  _HaydnInsertReg8(0x321db, 0xff, 0x03);
  _HaydnInsertReg8(0x321f0, 0xff, 0x00);

//============================
//AUSDM Verification setting..
//============================
  _HaydnInsertReg8(0x321f2, 0xff, 0x21); // [    5] EN_ADC_DITHER
  _HaydnInsertReg8(0x321f3, 0xff, 0x0f);
  _HaydnInsertReg8(0x321f0, 0xff, 0x00); // [11:10] DAC CHOP
  _HaydnInsertReg8(0x321f1, 0xff, 0x0c);
  _HaydnInsertReg8(0x321f0, 0xff, 0x00); // [ 9: 8] SEL_CHOP_FREQ_DAC
  _HaydnInsertReg8(0x321f1, 0xff, 0x0f);

   // select AMIC for DMAWR V
  _HaydnInsertReg8(0x3228d, 0x20, 0x00);
  _HaydnInsertReg8(0x32404, 0x04, 0x04);

   //16k
  _HaydnInsertReg8(0x32005, 0x0F, 0x03);
  _HaydnInsertReg8(0x32080, 0x07, 0x01);

   //select ADC in to MICIN
  _HaydnInsertReg8(0x321e2, 0xff, 0x77);   // INMUX0 Mic IN
  _HaydnInsertReg8(0x321d8, 0xff, 0x77);   // INMUX1 Mic IN

  //0+0dB
  _HaydnInsertReg8(0x321e5, 0xfc, 0);	// ADC inmux0 R Gain 0dB, L Gain 0dB
  _HaydnInsertReg8(0x321d7, 0xfc, 0);	// ADC inmux1 R Gain 0dB, L Gain 0dB

  _HaydnInsertReg8(0x321f6, 0xf0, 0x40);   // MIC0/1 L GAIN +0dB
  _HaydnInsertReg8(0x321f7, 0xff, 0x92);   // MIC0/1 R GAIN +0db

   //23+3dB
 // _HaydnInsertReg8(0x321e5, 0xfc, 0x6c);	// ADC inmux0 R Gain +3db, L Gain +3db
 // _HaydnInsertReg8(0x321d7, 0xfc, 0x6c);	// ADC inmux1 R Gain +3db, L Gain +3db

//  _HaydnInsertReg8(0x321f6, 0xf0, 0x90);   // MIC0/1 L GAIN +23dB
//  _HaydnInsertReg8(0x321f7, 0xff, 0x24);   // MIC0/1 R GAIN +23db
}
#else
void MHAL_AUD_AMIC(BOOL bEn)
{
  if(bEn)
  {
  /* AMIC path */
   // _HaydnInsertReg8(0x321e3, 0x03, 0x00);//PD_INMUX
    //_HaydnInsertReg8(0x321d9, 0x03, 0x00);//PD_INMUX
    _HaydnInsertReg8(0x321eb, 0xff, 0x00);
    _HaydnInsertReg8(0x321ec, 0x80, 0x00);//PD_LDOADC

    _HaydnInsertReg8(0x321f2, 0xff, 0x01);
    _HaydnInsertReg8(0x321f3, 0xff, 0x0f);

    //_HaydnInsertReg8(0x321da, 0xff, 0x28);//[0:1] PD_ADC
    _HaydnInsertReg8(0x321da, 0xfc, 0x28);//[0:1] PD_ADC
    _HaydnInsertReg8(0x321db, 0xff, 0x03);
   // _HaydnInsertReg8(0x321f0, 0xff, 0x00);//PD_MICAMP[4:7]

//============================
//AUSDM Verification setting..
//============================
    _HaydnInsertReg8(0x321f2, 0xff, 0x21); // [    5] EN_ADC_DITHER
    _HaydnInsertReg8(0x321f3, 0xff, 0x0f);


     //select ADC in to MICIN
    _HaydnInsertReg8(0x321e2, 0xff, 0x77);   // INMUX0 Mic IN
    _HaydnInsertReg8(0x321d8, 0xff, 0x77);   // INMUX1 Mic IN

    //0+0dB
    _HaydnInsertReg8(0x321e5, 0xfc, 0);	// ADC inmux0 R Gain 0dB, L Gain 0dB
    _HaydnInsertReg8(0x321d7, 0xfc, 0);	// ADC inmux1 R Gain 0dB, L Gain 0dB

    _HaydnInsertReg8(0x321f6, 0xf0, 0x40);   // MIC0/1 L GAIN +0dB
    _HaydnInsertReg8(0x321f7, 0xff, 0x92);   // MIC0/1 R GAIN +0db


   // select AMIC for DMAWR V
    _HaydnInsertReg8(0x3228d, 0x20, 0x00);
    _HaydnInsertReg8(0x32404, 0x04, 0x04);

   //16k
 // _HaydnInsertReg8(0x32005, 0x0F, 0x03);
 // _HaydnInsertReg8(0x32080, 0x07, 0x01);
  }
  else
  {
    // select DMIC for DMAWR V
    _HaydnInsertReg8(0x32404, 0x04, 0x00);

    HaydnMic01PowerOn(FALSE);
    HaydnMic23PowerOn(FALSE);

    _HaydnInsertReg8(0x321ec, 0x80, 0x80);
  }
}
#endif



BOOL MHAL_AUD_AdcSetRate(AudAdcRate_e eRate)
{
  U16 nValue1,nValue2;
  switch(eRate)
  {
    case AUD_ADC_RATE_8K:
      nValue1 = 0x0;
      nValue2 = 0x0;
      break;
    case AUD_ADC_RATE_16K:
      nValue1 = 0x3;
      nValue2 = 0x1;
      break;
    case AUD_ADC_RATE_32K:
      nValue1 = 0x6;
      nValue2 = 0x2;
      break;
    case AUD_ADC_RATE_48K:
      nValue1 = 0x8;
      nValue2 = 0x3;
      break;
    case AUD_ADC_RATE_96K:
      nValue1 = 0xB;
      nValue2 = 0x4;
      break;
    default:
      return FALSE;
  }

  _HaydnInsertReg16(REG_AUDIOBAND_CFG_02, AUD_CODEC_ADC_DEC_MSK ,nValue1<<AUD_CODEC_ADC_DEC_POS);
  _HaydnInsertReg16(REG_VLD_GEN_CFG_0, AUD_VLD_GEN_ADC_EN ,AUD_VLD_GEN_ADC_EN);
  _HaydnInsertReg16(REG_VLD_GEN_CFG_0, AUD_VLD_GEN_ADC_FS_SEL_MSK ,nValue2<<AUD_VLD_GEN_ADC_FS_SEL_POS);

  return TRUE;
}

BOOL MHAL_AUD_AdcSetChn(AudAdcChn_e eChn)
{
  switch(eChn)
  {
    case AUD_ADC_CHN2:
        HaydnMic01PowerOn(TRUE);
        HaydnMic23PowerOn(FALSE);
        break;
    case AUD_ADC_CHN4:
        HaydnMic01PowerOn(TRUE);
        HaydnMic23PowerOn(TRUE);
        break;
    default:
        return FALSE;
  }
  return TRUE;
}

#endif



void MHAL_AUD_DMAReset(void)
{
  //_TRACE((_CUS8 | LEVEL_4, "HalAudioDmaReset"));
  HaydnDmaReset();
}

void MHAL_AUD_DMASetBufAddr(U8 *pnBufAddr, U32 nBufSize)
{
  //_TRACE((_CUS8 | LEVEL_4, "HalAudioDmaRecSetBufAddr: pnBufAddr:%x, nBufSize:%d", pnBufAddr, nBufSize));
  HaydnDmaSetWrMcuBuf(pnBufAddr, nBufSize);
  HaydnDmaSetWrMiuAddr((U32)pnBufAddr, nBufSize);
}

void MHAL_AUD_DMASetBufOverThr(U32 nOverrun)
{
  //_TRACE((_CUS8 | LEVEL_4, "HalAudioDmaRecSetBufThreshold: nOverrun:%d, nUnderrun:%d", nOverrun, nUnderrun));
  HaydnDmaSetWrOverrunTh(nOverrun);
}

U32 MHAL_AUD_DMAGetBufLevel(void)
{
  //_TRACE((_CUS8 | LEVEL_4, "HalAudioDmaRecGetBufInfo: pnDataSize:%x, pbOverrun:%x, pbUnderrun:%x", pnDataSize, pbOverrun, pbUnderrun));
  return HaydnDmaWrLevelCnt();
}

void MHAL_AUD_DMAGetIntStatus(Bool *pbFull, Bool *pbOverrun)
{
  //_TRACE((_CUS8 | LEVEL_4, "HalAudioDmaRecGetBufInfo: pnDataSize:%x, pbOverrun:%x, pbUnderrun:%x", pnDataSize, pbOverrun, pbUnderrun));
  *pbFull = HaydnDmaWrIsFull();
  *pbOverrun  = HaydnDmaWrIsOverrun();
}

void MHAL_AUD_DMAEnableInt(Bool bFull, Bool bOverrun)
{
  //_TRACE((_CUS8 | LEVEL_4, "HalAudioDmaRecCtrlInt: bFull:%d, bOverrun:%d, bUnderrun:%d", bFull, bOverrun, bUnderrun));
  Bool bLocalFull = FALSE;
  HaydnDmaCtrlWrInt(bLocalFull, bOverrun, bFull);
}

void MHAL_AUD_DMAReadData(U8 *pnDestAddr, U32 nDataSize, U32 *pnDataRead)
{
  U32 nActualSize;
  //_TRACE((_CUS8 | LEVEL_4, "HalAudioDmaRecReadData: pnDestAddr:%x, nDataSize:%d, pnDataRead:%x", pnDestAddr, nDataSize, pnDataRead));
  //printf("HalAudioDmaRecReadData: pnDestAddr:%x, nDataSize:%d,
  //pnDataRead:%x\r\n", pnDestAddr, nDataSize, pnDataRead);
  nActualSize = HaydnDmaReadWrData(pnDestAddr, nDataSize);
  *pnDataRead = nActualSize;
}

void MHAL_AUD_DMAStart(void)
{
  //_TRACE((_CUS8 | LEVEL_4, "HalAudioDmaRecStart"));
  HaydnDmaIntClear(BACH_INT_CLR_WR_FULL);
  HaydnDmaEnableWr(True);
  gbDmaWrEnable = True;
}

void MHAL_AUD_DMAStop(void)
{
  //_TRACE((_CUS8 | LEVEL_4, "HalAudioDmaRecStop"));
  HaydnDmaEnableWr(False);
  gbDmaWrEnable = False;
}

void MHAL_AUD_DMAPause(void)
{
  //_TRACE((_CUS8 | LEVEL_4, "HalAudioDmaRecPause"));
  HaydnDmaEnableWr(False);
}

void MHAL_AUD_DMAResume(void)
{
  //_TRACE((_CUS8 | LEVEL_4, "HalAudioDmaRecResume"));
  HaydnDmaEnableWr(True);
}

Bool MHAL_AUD_DMAIsRunning(void)
{
  return gbDmaWrEnable;
}

void MHAL_AUD_DMAClearInt(void)
{
  HaydnDmaIntClear(BACH_INT_CLR_WR_FULL);
}

void MHAL_AUD_VADClearInt(void)
{
  U16 nConfigValue;

  nConfigValue = _HaydnReadReg(0x032D0E);
  nConfigValue |= (1 << 4);
  //Clear: 0x32D0E[4]
  //Debug: 0x32D0E[6]
  //Debug value: 0x32D0E[5]
  _HaydnWriteReg(0x032D0E, nConfigValue);

  nConfigValue &= ~(1 << 4);
  _HaydnWriteReg(0x032D0E, nConfigValue);

}

void MHAL_AUD_VADSetThr(U8 nThr)
{
  U16 nConfigValue;

  nConfigValue = _HaydnReadReg(0x032D0E);
  nConfigValue &= ~(0xff<<8);
  nConfigValue |=  (nThr<<8);
  _HaydnWriteReg(0x032D0E, nConfigValue);
  //iir, th:[15:8], int_dbg:[6], int_dbg_value[5], int_clr[4]
  //iir, default coef for 16k
}

void MHAL_AUD_VADEn(Bool bEn)
{
  _HaydnInsertReg16(0x032D0E, 0x8, (bEn?0x8:0));
}

void MHAL_AUD_DMASetWidth(AudDmaWidth_e eWidth)
{
  U16 nSel;
  switch(eWidth)
  {
      case AUD_DMA_WIDTH16:
          nSel=0;
          break;
      case AUD_DMA_WIDTH24:
      case AUD_DMA_WIDTH32:
          nSel=1;
          break;
      default:
          return;
  }

  _HaydnInsertReg16(0x32460, REG_WR_BIT_MODE, (nSel?REG_WR_BIT_MODE:0));
}

void MHAL_AUD_DMASetChn(AudDmaChn_e eChn)
{
  U16 nChn = 0;
  U16 nConfigValue = 0;

  nConfigValue = _HaydnReadReg(0x032480);

  switch(eChn)
  {
    case AUD_DMA_CHN2:
      nChn = 0x0;
      break;
    case AUD_DMA_CHN4:
      nChn = 0x1;
      break;
    case AUD_DMA_CHN6:
      nChn = 0x2;
      break;
    case AUD_DMA_CHN8:
      nChn = 0x3;
      break;
    default:
      return;
  }

  nConfigValue &= ~(0x7);
  nConfigValue |= nChn;
  _HaydnWriteReg(0x032480, nConfigValue);
  //_HaydnWriteReg(0x032480, 0x0080); //[2:0] 0:2ch, 1:4ch, 2:6ch, 3:8ch
}

void MHAL_AUD_DMASetMode(U16 nMode)
{
  //0x032408[4:0]auto selection for (sampling rate, decimation rate, output clock)
  // Mode	Fs	Bck
  // 1	8	400
  // 2	8	800
  // 3	8	1000
  // 4	8	1200
  // 5	8	2000
  // 6	8	2400
  // 7	8	4000
  // 8	16	800
  // 9	16	1600
  // 10	16	2000
  // 11	16	2400
  // 12	16	4000
  // 13	16	4800
  // 14	16	8000
  // 15	32	4000
  // 16	48	4800
  _HaydnWriteReg(0x032408, nMode); //REG_MODE_ALL
}

void MHAL_AUD_DMASetModeExt(U16 val)
{
  _HaydnWriteReg(0x03240A, val);
}

void MHAL_AUD_DMAEnableSigGen(Bool bEn)
{
  if (bEn)
    _HaydnWriteReg(0x032406, 0x2021); //[15:12] SIN_FREQ_SEL
  else
    _HaydnWriteReg(0x032406, 0); //[15:12] SIN_FREQ_SEL
  //[11:8]  SIN_GAIN
  //[5:4]   SIN_PATH_SEL     //0:normal, 1:fifo in, 2:hpf in, 3:reserved
  //[2:1]   SIN_PATH_SEL_LR  //0:LR, 1:L, 2:R, 3:reserved
  //[0]     SIN_ENABLE
}


/*
[14:12] gain for ch2 (6dB per step)
[10:8] gain for ch1 (6dB per step)
*/
void MHAL_AUD_DMicSetGain(U8 u8Step)
{
  if(u8Step < 8)
  {
   U16 nConfigValue;
   nConfigValue = _HaydnReadReg(0x032442);
   nConfigValue &= ~(0xFF00);
   nConfigValue |= ((u8Step<<8) | (u8Step<<12));
   _HaydnWriteReg(0x032442, nConfigValue);

   nConfigValue = _HaydnReadReg(0x032444);
   nConfigValue &= ~(0xFF00);
   nConfigValue |= ((u8Step<<8) | (u8Step<<12));
   _HaydnWriteReg(0x032444, nConfigValue);

   nConfigValue = _HaydnReadReg(0x032446);
   nConfigValue &= ~(0xFF00);
   nConfigValue |= ((u8Step<<8) | (u8Step<<12));
   _HaydnWriteReg(0x032446, nConfigValue);

   nConfigValue = _HaydnReadReg(0x032448);
   nConfigValue &= ~(0xFF00);
   nConfigValue |= ((u8Step<<8) | (u8Step<<12));
   _HaydnWriteReg(0x032448, nConfigValue);
  }
}


/*
[15] auto mode
[13:8] phase for ch2
[5:0] phase for ch1
*/
void MHAL_AUD_DMicSelPhase(U16 u16Sel)
{
  _HaydnWriteReg(0x032424, u16Sel); //sel
  _HaydnWriteReg(0x032428, u16Sel); //sel
  _HaydnWriteReg(0x03242C, u16Sel); //sel
  _HaydnWriteReg(0x032430, u16Sel); //sel
}

void MHAL_AUD_LowPower(void)
{
/*
wriu -b 0x032440 0xE0 0xE0 // PD CIC 1R & 2L/R
wriu -b 0x032441 0x0F 0x0F // PD CIC 3 & 4
*/
  _HaydnInsertReg8(0x032440, 0xE0, 0xE0);
  _HaydnInsertReg8(0x032441, 0x0F, 0x0F);
}

void MHAL_AUD_HighPower(void)
{
/*
wriu -b 0x032440 0xE0 0x00 // ON CIC 1R & 2L/R
wriu -b 0x032441 0x0F 0x00 // ON CIC 3 & 4
*/
  _HaydnInsertReg8(0x032440, 0xE0, 0x00);
  _HaydnInsertReg8(0x032441, 0x0F, 0x00);
}


void MHAL_AUD_MicEn(Bool bEn)
{
/*
reg_all_pad_in
reg[000b00]#1 = 0b
reg_test_in_mode
reg[000ba0]#1~#0 = 00b
reg_test_out_mode
reg[000ba0]#5~#4 = 00b
reg_fuart3_mode
reg[000bae]#3~#2 = 00b
reg_led_dig0_mode
reg[000ba8]#0 = 0b
reg_i8m6_mode
reg[000ba4]#3~#1 = 000b
*/
  _HaydnInsertReg16(0x000ba0, 0x3, 0x0);  //reg_test_in_mode
  _HaydnInsertReg16(0x000ba0, 0x30, 0x0); //reg_test_out_mode
  _HaydnInsertReg16(0x000bae, 0xc, 0x0);  //reg_fuart3_mode
  _HaydnInsertReg16(0x000ba8, 0x1, 0x0);  //reg_led_dig0_mode
  _HaydnInsertReg16(0x000bb6, 0x7, 0x0);  //reg_ttl_mode
  _HaydnInsertReg16(0x000ba4, 0xe, 0x0);  //reg_i8m6_mode

  _HaydnInsertReg16(0x000bf7, 0x2, 0x2);

  _HaydnInsertReg16(0x000bd7, 0x2, bEn?0x2:0);
}

void MHAL_AUD_Init(void)
{

#ifdef __ENABLE_MAILBOX_FOR_SCRIPT_LOAD__
  _BachWriteReg(0x100404, 0xff);
  //UartSendTrace("Wait load script ok \n");
  while(0xff == _BachReadMailboxReg(0x04));
  return;
#endif


  //=================================================
  // top setting start !!!");
  //=================================================
#if 0
  _HaydnWriteReg(0x040a06, 0x0011);
  _HaydnWriteReg(0x040a02, 0x0000);
  _HaydnWriteReg(0x0409c0, 0xd2a3);
  _HaydnWriteReg(0x0409c2, 0x0050);
  _HaydnWriteReg(0x0409c4, 0x0001);
  _HaydnWriteReg(0x040922, 0x0000);
  _HaydnWriteReg(0x040806, 0x001f);
  _HaydnWriteReg(0x040862, 0x0001);
  _HaydnWriteReg(0x040210, 0x0008);
  _HaydnWriteReg(0x040210, 0x0018);
  _HaydnWriteReg(0x040c62, 0x0001);
  _HaydnWriteReg(0x040220, 0x0010);
  _HaydnWriteReg(0x001202, 0x0858);
  _HaydnWriteReg(0x001204, 0x11c5);
  _HaydnWriteReg(0x001206, 0x0020);
  _HaydnWriteReg(0x001210, 0x0031);
  _HaydnWriteReg(0x00122a, 0x400f);
  _HaydnWriteReg(0x022322, 0x0001);
  _HaydnWriteReg(0x022340, 0x0f0f);
  _HaydnWriteReg(0x022342, 0x081f);
  _HaydnWriteReg(0x022344, 0x2f08);
  _HaydnWriteReg(0x022346, 0x0f01);
  _HaydnWriteReg(0x022348, 0x070f);
  _HaydnWriteReg(0x02234a, 0x0104);
  _HaydnWriteReg(0x02234c, 0x0528);
  _HaydnWriteReg(0x02234e, 0x000e);
  _HaydnWriteReg(0x022350, 0x000e);
  _HaydnWriteReg(0x022352, 0x0406);
  _HaydnWriteReg(0x022354, 0x0f02);
  _HaydnWriteReg(0x022356, 0x0004);
  _HaydnWriteReg(0x022386, 0x0000);
  _HaydnWriteReg(0x0223fe, 0x0001);
  _HaydnWriteReg(0x0223de, 0x8005);
  // read addr : 0x00122c
  _HaydnWriteReg(0x00122c, 0x8221);
  _HaydnWriteReg(0x0012fc, 0x61a1);
  _HaydnWriteReg(0x02238a, 0x0307);
  _HaydnWriteReg(0x022380, 0x80f0);
  _HaydnWriteReg(0x022630, 0xc01d);
  _HaydnWriteReg(0x022670, 0xc01d);
  _HaydnWriteReg(0x0226b0, 0xc01d);
  _HaydnWriteReg(0x0226f0, 0xc01d);
  _HaydnWriteReg(0x022730, 0xc01d);
  _HaydnWriteReg(0x022770, 0x001d);
  _HaydnWriteReg(0x0227b0, 0x001d);
  _HaydnWriteReg(0x001170, 0x0800);
  _HaydnWriteReg(0x001158, 0x0303);
  _HaydnWriteReg(0x00115a, 0x3333);
  _HaydnWriteReg(0x00115c, 0x3333);
  _HaydnWriteReg(0x00115e, 0x3333);
  _HaydnWriteReg(0x001174, 0x0606);
  _HaydnWriteReg(0x001176, 0x0606);
  _HaydnWriteReg(0x00111a, 0x8333);
  _HaydnWriteReg(0x00111c, 0x0020);
  _HaydnWriteReg(0x001108, 0x003f);
  _HaydnWriteReg(0x00121e, 0x8c00);
  _HaydnWriteReg(0x001100, 0x2010);
  _HaydnWriteReg(0x001100, 0x0000);
  _HaydnWriteReg(0x001118, 0x0000);
  _HaydnWriteReg(0x00117c, 0x0000);
  _HaydnWriteReg(0x001154, 0xc000);
  _HaydnWriteReg(0x00111e, 0x0005);
  _HaydnWriteReg(0x00111e, 0x000f);
  _HaydnWriteReg(0x00111e, 0x0005);
  _HaydnWriteReg(0x001100, 0x0000);
  _HaydnWriteReg(0x001200, 0x0000);
  _HaydnWriteReg(0x02249c, 0xe0ff);
  _HaydnWriteReg(0x02249e, 0x003f);
  _HaydnWriteReg(0x001200, 0x0008);
  _HaydnWriteReg(0x001200, 0x000c);
  _HaydnWriteReg(0x001200, 0x000e);
  _HaydnWriteReg(0x001200, 0x001f);
  // read addr : 0x001200 , check data : 0x0Xxxx
  _HaydnWriteReg(0x022606, 0x0000);
  _HaydnWriteReg(0x022646, 0x0000);
  _HaydnWriteReg(0x022686, 0x0000);
  _HaydnWriteReg(0x0226c6, 0x0000);
  _HaydnWriteReg(0x022706, 0x0000);
  _HaydnWriteReg(0x022746, 0x0000);
  _HaydnWriteReg(0x00121e, 0x8c08);
  // read addr : 0x0012fe
  _HaydnWriteReg(0x0012fe, 0x8000);
  _HaydnWriteReg(0x0012f8, 0x0000);
  // read addr : 0x0012fe
  _HaydnWriteReg(0x0012fe, 0x8000);
  _HaydnWriteReg(0x0012e0, 0x0000);
  _HaydnWriteReg(0x0012e2, 0x0080);
  _HaydnWriteReg(0x0012e4, 0x0010);
  _HaydnWriteReg(0x0012e6, 0x0000);
  _HaydnWriteReg(0x0012e8, 0x5aa5);
  // read addr : 0x0012d8
  _HaydnWriteReg(0x0012d8, 0x0000);
  _HaydnWriteReg(0x0012e0, 0x0006);
  _HaydnWriteReg(0x0012e0, 0x0007);
  // read addr : 0x0012e0 , check data : 0x0Xxxx
  _HaydnWriteReg(0x0012e0, 0x0000);
  _HaydnWriteReg(0x0012f8, 0x0000);
  // read addr : 0x0012fe
  _HaydnWriteReg(0x0012fe, 0x8000);
  _HaydnWriteReg(0x0012e0, 0x0000);
  _HaydnWriteReg(0x0012e2, 0x0080);
  _HaydnWriteReg(0x0012e4, 0x0010);
  _HaydnWriteReg(0x0012e6, 0x0000);
  _HaydnWriteReg(0x0012e8, 0x5aa5);
  // read addr : 0x0012d8
  _HaydnWriteReg(0x0012d8, 0x0000);
  _HaydnWriteReg(0x0012e0, 0x0106);
  _HaydnWriteReg(0x0012e0, 0x0107);
  // read addr : 0x0012e0 , check data : 0x0Xxxx
  _HaydnWriteReg(0x0012e0, 0x0000);
  _HaydnWriteReg(0x003f16, 0x00a5);
  _HaydnWriteReg(0x001e06, 0x0007);
#endif
  //=================================================
  // top/haydn setting start !!!
  //=================================================

  //_HaydnWriteReg(0x000B00, 0x0000); //[1] reg_all_pad_in

  //PDM_DIN1 PDM_DIN2 PDM_DIN3
  //_HaydnWriteReg(0x000BB8, 0x3000); //[13:12] reg_pdm_din_mode

#if !defined(CONFIG_AUD_MIC_EXT)
  if (((_HaydnReadReg(0x1ece)&0xff00)>>8) == 0)
  {

    AUD_MSG("MHAL_AUD_Init: Config PAD_UART3 as PDM_DIN (U01)\n");

    _HaydnInsertReg16(0x000BB8, 0x7<<12, 0x3<<12);  //[13:12] reg_pdm_din_mode
    _HaydnWriteReg(0x003F8A, _HaydnReadReg(0x003F8A) | 0x0100); //active VPER2
  }
  else
  {
#if defined(BOOTROM_VERSION_MST291B) || defined(BOOTROM_VERSION_MST291A) || defined(BOOTROM_VERSION_MSTDB306)
    AUD_MSG("MHAL_AUD_Init: Config PAD_UART3 as PDM_DIN (U02)\n");

    _HaydnInsertReg16(0x000BB8, 0x7<<12, 0x3<<12);  //[13:12] reg_pdm_din_mode
    _HaydnWriteReg(0x003F8A, _HaydnReadReg(0x003F8A) | 0x0100); //active VPER2

#if defined(BOOTROM_VERSION_MSTDB306)
    _HaydnInsertReg8(0x003ffc, 0x1<<6, 0x0);  //PAD_PM_GPIO06 out
    _HaydnInsertReg8(0x003ffa, 0x1<<6, 0x1<<6);

    _HaydnInsertReg8(0x003ffd, 0x1<<3, 0x0);  //PAD_PM_GPIO011 out
    _HaydnInsertReg8(0x003ffb, 0x1<<3, 0x1<<3);
#endif

#else

    AUD_MSG("MHAL_AUD_Init: Config PAD_I2S0 as PDM_DIN (U02)\n");

    _HaydnInsertReg16(0x000BB8, 0x7<<12, 0x0<<12);  //[13:12] reg_pdm_din_mode
    _HaydnInsertReg16(0x000BBC, 0xf, 0); //reg_i2s0_mode/reg_i2s0_slv_mode
    _HaydnInsertReg16(0x000BFC, 0x3<<12, 0x1<<12);  //[13:12] reg_pdm_din_mode1

    #if defined(BOOTROM_VERSION_MST291C_D01B)
    _HaydnWriteReg(0x003F8A, _HaydnReadReg(0x003F8A) | 0x0001); //active VPER1
    AUD_MSG("MHAL_AUD_Init: Config PAD_I2S0_WS as GPIO\n");
    _HaydnInsertReg8(0x000ba0, 0x3, 0x0);  //reg_test_in_mode
    _HaydnInsertReg8(0x000ba0, 0x30, 0x0); //reg_test_out_mode
    _HaydnInsertReg8(0x000bb1, 0x30, 0x0);  //reg_sd0_mode
    _HaydnInsertReg8(0x000bbc, 0xc, 0x0);  //reg_i2s0_slv_mode
    _HaydnInsertReg8(0x000bbc, 0x3, 0x0);  //reg_i2s0_mode
    _HaydnInsertReg8(0x000baa, 0x4, 0x0);  //reg_ccir1_mode

    _HaydnInsertReg8(0x000be4, 0x1, 0x0);  //output
    _HaydnInsertReg8(0x000bc4, 0x1, 0x1);  //pull high
    #else
    AUD_MSG("MHAL_AUD_Init: Config PAD_TTL_IO9 as GPIO\n");
    _HaydnInsertReg8(0x000ba0, 0x3, 0x0);  //reg_test_in_mode
    _HaydnInsertReg8(0x000ba0, 0x30, 0x0); //reg_test_out_mode
    _HaydnInsertReg8(0x000bae, 0xc, 0x0);  //reg_fuart3_mode
    _HaydnInsertReg8(0x000ba8, 0x1, 0x0);  //reg_led_dig0_mode
    _HaydnInsertReg8(0x000bb6, 0x7, 0x0);  //reg_ttl_mode
    _HaydnInsertReg8(0x000ba4, 0xe, 0x0);  //reg_i8m6_mode

    _HaydnInsertReg8(0x000bf7, 0x2, 0x0);  //output
    _HaydnInsertReg8(0x000bd7, 0x2, 0x2);  //pull high
    #endif
#endif
  }
#endif

  //PM_PDM_BCK PM_PDM_DIN0
  _HaydnWriteReg(0x003F44, 0x0001); //[0] reg_pdm_mode

  _HaydnWriteReg(0x03220A, _HaydnReadReg(0x03220A) | 0x0004);
  _HaydnWriteReg(0x032208, _HaydnReadReg(0x032208) | 0x8000);
  //_HaydnWriteReg(0x03220A, 0x0004); //[2] enable clk_miu
  //_HaydnWriteReg(0x032208, 0x8000); //[15] enable clk_24m

/*
  _HaydnWriteReg(0x032D02, 0x8000); //[15] bck from vrec
  _HaydnWriteReg(0x032D14, 0x0001); //[0] sel data to iir
  _HaydnWriteReg(0x03228C, 0x0001); //[1:0] sel data to vad
*/

  _HaydnWriteReg(0x032D02, 0x0000); //[15] bck from vrec
  _HaydnWriteReg(0x032D14, 0x0000); //[0] sel data to iir
  _HaydnWriteReg(0x03228C, 0x0001); //[1:0] sel data to vad


  //=================================================
  // voice recording setting start !!!
  //=================================================

  // Mode	Fs	Bck
  // 1	8	400
  // 2	8	800
  // 3	8	1000
  // 4	8	1200
  // 5	8	2000
  // 6	8	2400
  // 7	8	4000
  // 8	16	800
  // 9	16	1600
  // 10	16	2000
  // 11	16	2400
  // 12	16	4000
  // 13	16	4800
  // 14	16	8000
  // 15	32	4000
  // 16	48	4800

  //VAD
  _HaydnWriteReg(0x032D08, 0x0005); //REG_MODE_ALL
  _HaydnWriteReg(0x032D40, 0x0001); //cic
  _HaydnWriteReg(0x032D02, 0x0001); //sel enable
  _HaydnWriteReg(0x032D22, 0x0001); //sel sync enable
  _HaydnWriteReg(0x032D24, 0x8000); //sel auto
  _HaydnWriteReg(0x032D20, 0x0000); //sel pd
  _HaydnWriteReg(0x032D0C, 0x011F); //audioband/timeGen enable
  // replace by MHAL_AUD_VADSetThr
  _HaydnWriteReg(0x032D0E, 0x0008); //iir
  //iir, th:[15:8], int_dbg:[6], int_dbg_value[5], int_clr[4]
  //iir, default coef for 16k

  //VREC
  // replace by MHAL_AUD_DMASetMode
  _HaydnWriteReg(0x032408, 0x000B); //REG_MODE_ALL 2.4M clock
  _HaydnWriteReg(0x032440, 0x0001); //cic
  _HaydnWriteReg(0x032402, 0x0031); //sel enable
  _HaydnWriteReg(0x03240C, 0x8000); //audioband sram init
//#50000;
  _HaydnWriteReg(0x03240C, 0x0000);
  _HaydnWriteReg(0x032404, 0x0001); //[0] sel imi

  //dma setting
  // replace by MHAL_AUD_DMASetChn
  //_HaydnWriteReg(0x032480, 0x0080); //[2:0] 0:2ch, 1:4ch, 2:6ch, 3:8ch [7]debug
  _HaydnWriteReg(0x032480, 0x0000); //[2:0] 0:2ch, 1:4ch, 2:6ch, 3:8ch [7]debug
#if 0
  wriu - w 0x032462 0x0100
  wriu - w 0x032464 0x0000 //base addr
  wriu - w 0x032466 0x0400 //size
  wriu - w 0x03246a 0x0100 //overrun -th
  wriu - w 0x032460 0x8000 //reset
  wriu - w 0x032460 0x000b //enable
#endif

  _HaydnWriteReg(0x032422, 0x0001); //sel
  _HaydnWriteReg(0x032424, 0x0904); //sel //modify by roger2018/5/23 and black for 2.4M
  _HaydnWriteReg(0x032426, 0x0001); //sel
  _HaydnWriteReg(0x032428, 0x0904); //sel
  _HaydnWriteReg(0x03242A, 0x0001); //sel
  _HaydnWriteReg(0x03242C, 0x0904); //sel
  _HaydnWriteReg(0x03242E, 0x0001); //sel
  _HaydnWriteReg(0x032430, 0x0904); //sel
  _HaydnWriteReg(0x032420, 0x0000); //sel pd
  _HaydnWriteReg(0x03240C, 0x011F); //audioband/timeGen enable
  // replace by MHAL_AUD_DMAEnableSigGen
  //_HaydnWriteReg(0x032406, 0x2021); //[15:12] SIN_FREQ_SEL
  _HaydnWriteReg(0x032406, 0); //[15:12] SIN_FREQ_SEL
  //[11:8]  SIN_GAIN
  //[5:4]   SIN_PATH_SEL     //0:normal, 1:fifo in, 2:hpf in, 3:reserved
  //[2:1]   SIN_PATH_SEL_LR  //0:LR, 1:L, 2:R, 3:reserved
  //[0]     SIN_ENABLE

  //_HaydnWriteReg(0x032296, 0x000f);

  //_HaydnWriteReg(0x032296, 0x0002);
  _HaydnWriteReg(0x032296, _HaydnReadReg(0x032296) | 0x0002);
  //_HaydnWriteReg(0x0322E0, 0x0800); //fpga_basic_0629_1136.sof
/*
0x032210[15:14]
00: FRO
01: UPLL
10: XTAL
*/
  //_HaydnWriteReg(0x032210, _HaydnReadReg(0x032210) & 0x3fff); //[15:14] = 0, FRO for DMIC clock
  if(_HaydnReadReg(0x1e16)&(1<<5))//xtal 24M
    _HaydnInsertReg16(0x032210, 0x3<<14, 0x2<<14); //xtal
  else
    _HaydnInsertReg16(0x032210, 0x3<<14, 0x1<<14); //upll

  HaydnDmaIntClear(BACH_INT_CLR_WR_FULL);

}

