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
/// file    halBDMA.c
/// @brief  MStar BDMA hal DDI
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

#define _MHAL_BDMA_C

//=============================================================================
// Include Files
//=============================================================================
#include "halBDMA.h"

#include "mosWrapper.h"

#define BDMA_MSG(fmt, args...) MOS_DBG_PRINT(MOS_DBG_LEVEL_BDMA, "[BDMA] " fmt, ##args)
#define BDMA_ERR(fmt, args...) MOS_DBG_ERROR("[BDMA ERR] " fmt, ##args);

//please check clkgen register bit to enable BDMA clock (miu clock or others) with designer
#define BDMA_CLK_REG                GET_REG16_ADDR(REG_ADDR_BASE_CLKGEN, 0x5a)
#define BDMA_BASE_REG               REG_ADDR_BASE_VBDMA

#define BDMA_SET_CH_REG(x)         (GET_REG16_ADDR(BDMA_BASE_REG,x))

#define BDMA_REG_CTRL(ch)           BDMA_SET_CH_REG(0x00+ch*0x10)
#define BDMA_REG_STATUS(ch)         BDMA_SET_CH_REG(0x01+ch*0x10)
#define BDMA_REG_DEV_SEL(ch)        BDMA_SET_CH_REG(0x02+ch*0x10)
#define BDMA_REG_MISC(ch)           BDMA_SET_CH_REG(0x03+ch*0x10)
#define BDMA_REG_SRC_ADDR_L(ch)     BDMA_SET_CH_REG(0x04+ch*0x10)
#define BDMA_REG_SRC_ADDR_H(ch)     BDMA_SET_CH_REG(0x05+ch*0x10)
#define BDMA_REG_DST_ADDR_L(ch)     BDMA_SET_CH_REG(0x06+ch*0x10)
#define BDMA_REG_DST_ADDR_H(ch)     BDMA_SET_CH_REG(0x07+ch*0x10)
#define BDMA_REG_SIZE_L(ch)         BDMA_SET_CH_REG(0x08+ch*0x10)
#define BDMA_REG_SIZE_H(ch)         BDMA_SET_CH_REG(0x09+ch*0x10)
#define BDMA_REG_CMD0_L(ch)         BDMA_SET_CH_REG(0x0A+ch*0x10)
#define BDMA_REG_CMD0_H(ch)         BDMA_SET_CH_REG(0x0B+ch*0x10)
#define BDMA_REG_CMD1_L(ch)         BDMA_SET_CH_REG(0x0C+ch*0x10)
#define BDMA_REG_CMD1_H(ch)         BDMA_SET_CH_REG(0x0D+ch*0x10)
#define BDMA_REG_CMD2_L(ch)         BDMA_SET_CH_REG(0x0E+ch*0x10)
#define BDMA_REG_CMD2_H(ch)         BDMA_SET_CH_REG(0x0F+ch*0x10)



//---------------------------------------------
// definition for BDMA_REG_CH0_CTRL/BDMA_REG_CH1_CTRL
//---------------------------------------------
#define BDMA_CH_TRIGGER             BIT(0)
#define BDMA_CH_STOP                BIT(4)


//---------------------------------------------
// definition for REG_BDMA_CH0_STATUS/REG_BDMA_CH1_STATUS
//---------------------------------------------
#define BDMA_CH_QUEUED              BIT(0)
#define BDMA_CH_BUSY                BIT(1)
#define BDMA_CH_INT                 BIT(2)
#define BDMA_CH_DONE                BIT(3)
#define BDMA_CH_RESULT              BIT(4)
#define BDMA_CH_CLEAR_STATUS        (BDMA_CH_INT|BDMA_CH_DONE|BDMA_CH_RESULT)

//---------------------------------------------
// definition for REG_BDMA_CH0_MISC/REG_BDMA_CH1_MISC
//---------------------------------------------

#define BDMA_CH_ADDR_DECDIR         BIT(0)
#define BDMA_CH_DONE_INT_EN         BIT(1)
#define BDMA_CH_CRC_REFLECTION      BIT(4)
#define BDMA_CH_MOBF_EN             BIT(5)

BOOL MHal_BDMA_WaitDone(BDMA_Ch channel,U32 u32Timeoutmsec)
{
    U32 count=0;
    for(count=0;count<u32Timeoutmsec;count++)
    {
        if(INREG16(BDMA_REG_STATUS(channel)) & BDMA_CH_DONE)
        {
            return TRUE;
        }
        MOS_mDelay(1);
    }

    return FALSE;
}

BOOL MHal_BDMA_PollingDone(BDMA_Ch channel)
{
    while(1)
    {
        if(INREG16(BDMA_REG_STATUS(channel)) & BDMA_CH_DONE)
        {
            return TRUE;
        }
    }
}

BOOL MHal_BDMA_IsBusy(BDMA_Ch channel)
{
    return (BDMA_CH_BUSY==(INREG16(BDMA_REG_STATUS(channel)) & BDMA_CH_BUSY));
}

BOOL MHal_BDMA_FlashToMIU(BDMA_Ch channel, U32 u32FlashAddr, U32 u32DramAddr, U32 u32Len)
{
    int i=0;
    //u32DramAddr &= (~MIU0_START_ADDR);
    if((u32Len & 0x0F) || (u32FlashAddr& 0x0F) || (u32DramAddr & 0x0F))
    {
    BDMA_ERR("MHal_BDMA_FlashToMIU: address or length should be 16 bytes aligned!!\n");
    return FALSE;
    }

    for(i=0;i<5000;i++)
    {
    if(0==(INREG16(BDMA_REG_STATUS(channel)) & BDMA_CH_BUSY))
    {
    break;
    }
      MOS_mDelay(1);
    }

    if(5000==i)
    {
    BDMA_ERR("MHal_BDMA_FLASH_TO_MEM error!! device is busy!!\n");
    return FALSE;
    }

    OUTREG16(BDMA_REG_STATUS(channel),BDMA_CH_CLEAR_STATUS);

    OUTREG16(BDMA_REG_DEV_SEL(channel),0x4035);
    OUTREG16(BDMA_REG_SRC_ADDR_L(channel),(U16)(u32FlashAddr & 0xFFFF));
    OUTREG16(BDMA_REG_SRC_ADDR_H(channel),(U16)((u32FlashAddr>>16) & 0xFFFF));

    OUTREG16(BDMA_REG_DST_ADDR_L(channel),(U16)(u32DramAddr & 0xFFFF));
    OUTREG16(BDMA_REG_DST_ADDR_H (channel),(U16)((u32DramAddr>>16) & 0xFFFF));

    OUTREG16(BDMA_REG_SIZE_L(channel),(U16)(u32Len & 0xFFFF));
    OUTREG16(BDMA_REG_SIZE_H(channel),(U16)((u32Len>>16) & 0xFFFF));

    OUTREG16(BDMA_REG_CTRL(channel),BDMA_CH_TRIGGER);

    return TRUE;
}

BOOL MHal_BDMA_CalculateCRC32FromMIU(BDMA_Ch channel, U32 u32DramAddr, U32 u32Len)
{
    //u32DramAddr &= (~MIU0_START_ADDR);

    OUTREG16(BDMA_REG_STATUS(channel),BDMA_CH_CLEAR_STATUS);

    OUTREG16(BDMA_REG_DEV_SEL(channel),0x0340);
    OUTREG16(BDMA_REG_SRC_ADDR_L(channel),(U16)(u32DramAddr & 0xFFFF));
    OUTREG16(BDMA_REG_SRC_ADDR_H(channel),(U16)((u32DramAddr>>16) & 0xFFFF));

    OUTREG16(BDMA_REG_DST_ADDR_L(channel), 0);
    OUTREG16(BDMA_REG_DST_ADDR_H(channel), 0);

    OUTREG16(BDMA_REG_SIZE_L(channel),(U16)(u32Len & 0xFFFF));
    OUTREG16(BDMA_REG_SIZE_H(channel),(U16)((u32Len>>16) & 0xFFFF));

    OUTREG16(BDMA_REG_CMD0_L(channel), 0x1db7);
    OUTREG16(BDMA_REG_CMD0_H(channel), 0x04c1);

    OUTREG16(BDMA_REG_CMD1_L(channel), 0xFFFF);
    OUTREG16(BDMA_REG_CMD1_H(channel), 0xFFFF);

    OUTREG16(BDMA_REG_CTRL(channel),BDMA_CH_TRIGGER);

    return TRUE;
}

U32 MHal_BDMA_GetCRC32(BDMA_Ch channel)
{
    return (INREG16(BDMA_REG_CMD1_L(channel)) | (INREG16(BDMA_REG_CMD1_H(channel)) <<16));
}

BOOL MHal_BDMA_IMItoMIU(BDMA_Ch channel, U32 u32Src, U32 u32Dst, U32 u32Len)
{

    //u32Src &= (~MIU_BASE_ADDR);
    //u32Dst &= (~MIU_BASE_ADDR);

    OUTREG16(BDMA_REG_STATUS(channel),BDMA_CH_CLEAR_STATUS);

    //OUTREG16(BDMA_REG_MISC(channel),BDMA_CH_DONE_INT_EN | 0x2000);
    OUTREG16(BDMA_REG_MISC(channel), 0x2000);

    //OUTREG16(BDMA_REG_DEV_SEL(channel),0x4041);
    OUTREG16(BDMA_REG_DEV_SEL(channel),0x4140); //replace MIU channel0
    OUTREG16(BDMA_REG_SRC_ADDR_L(channel),(U16)(u32Src & 0xFFFF));
    OUTREG16(BDMA_REG_SRC_ADDR_H(channel),(U16)((u32Src>>16) & 0xFFFF));

    OUTREG16(BDMA_REG_DST_ADDR_L(channel), (U16)(u32Dst & 0xFFFF));
    OUTREG16(BDMA_REG_DST_ADDR_H(channel), (U16)((u32Dst>>16) & 0xFFFF));

    OUTREG16(BDMA_REG_SIZE_L(channel),(U16)(u32Len & 0xFFFF));
    OUTREG16(BDMA_REG_SIZE_H(channel),(U16)((u32Len>>16) & 0xFFFF));

    OUTREG16(BDMA_REG_CTRL(channel),BDMA_CH_TRIGGER);

    return TRUE;
}

BOOL MHal_BDMA_MIUtoIMI(BDMA_Ch channel, U32 u32Src, U32 u32Dst, U32 u32Len)
{

    //u32Src &= (~MIU_BASE_ADDR);
    //u32Dst &= (~MIU_BASE_ADDR);

    OUTREG16(BDMA_REG_STATUS(channel),BDMA_CH_CLEAR_STATUS);

    //OUTREG16(BDMA_REG_MISC(channel),BDMA_CH_DONE_INT_EN | 0x2000);
    OUTREG16(BDMA_REG_MISC(channel), 0x8000);

    //OUTREG16(BDMA_REG_DEV_SEL(channel),0x4041);
    OUTREG16(BDMA_REG_DEV_SEL(channel),0x4140); //replace MIU channel0
    OUTREG16(BDMA_REG_SRC_ADDR_L(channel),(U16)(u32Src & 0xFFFF));
    OUTREG16(BDMA_REG_SRC_ADDR_H(channel),(U16)((u32Src>>16) & 0xFFFF));

    OUTREG16(BDMA_REG_DST_ADDR_L(channel), (U16)(u32Dst & 0xFFFF));
    OUTREG16(BDMA_REG_DST_ADDR_H(channel), (U16)((u32Dst>>16) & 0xFFFF));

    OUTREG16(BDMA_REG_SIZE_L(channel),(U16)(u32Len & 0xFFFF));
    OUTREG16(BDMA_REG_SIZE_H(channel),(U16)((u32Len>>16) & 0xFFFF));

    OUTREG16(BDMA_REG_CTRL(channel),BDMA_CH_TRIGGER);

    return TRUE;
}


BOOL MHal_BDMA_IMItoIMI(BDMA_Ch channel, U32 u32Src, U32 u32Dst, U32 u32Len)
{

    //u32Src &= (~IMI_BASE_ADDR);
    //u32Dst &= (~IMI_BASE_ADDR);

    //BDMA_MSG("MHal_BDMA_IMItoIMI = 0x%x -> 0x%x (0x%x)\n", u32Src, u32Dst, u32Len);

    OUTREG16(BDMA_REG_STATUS(channel),BDMA_CH_CLEAR_STATUS);

    OUTREG16(BDMA_REG_MISC(channel),0x8000);

    OUTREG16(BDMA_REG_DEV_SEL(channel),0x4141);

    OUTREG16(BDMA_REG_SRC_ADDR_L(channel),(U16)(u32Src & 0xFFFF));
    OUTREG16(BDMA_REG_SRC_ADDR_H(channel),(U16)((u32Src>>16) & 0xFFFF));

    OUTREG16(BDMA_REG_DST_ADDR_L(channel), (U16)(u32Dst & 0xFFFF));
    OUTREG16(BDMA_REG_DST_ADDR_H(channel), (U16)((u32Dst>>16) & 0xFFFF));

    OUTREG16(BDMA_REG_SIZE_L(channel),(U16)(u32Len & 0xFFFF));
    OUTREG16(BDMA_REG_SIZE_H(channel),(U16)((u32Len>>16) & 0xFFFF));

    OUTREG16(BDMA_REG_CTRL(channel),BDMA_CH_TRIGGER);

    return TRUE;
}


BOOL MHal_BDMA_ClearStatus(BDMA_Ch channel)
{
    OUTREG16(BDMA_REG_STATUS(channel),BDMA_CH_CLEAR_STATUS);
    return TRUE;
}

/*
clk_bdma1 clock setting
[0]: disable clock
[1]: invert clock
[4:2]: Select clock source
     0xx:  clk_xtal div2(gf-mux)
     100:  clk_nfcie_p_buf
     101:  clk_miu_p_buf
     110:  1'b0
     111:  1'b0
*/
void MHal_BDMA_Init(void)
{
    U16 u16Val;
    u16Val = INREG16(BDMA_CLK_REG);

#if ( BDMA_BASE_REG == REG_ADDR_BASE_BDMA )
    u16Val &= ~(0x00FF);
    CLRREG16(BDMA_CLK_REG, (0x1<<4));
    OUTREG16(BDMA_CLK_REG, u16Val | 0x4); //miu clock
    SETREG16(BDMA_CLK_REG, (0x1<<4));
#else
    u16Val &= ~(0xFF00);
    CLRREG16(BDMA_CLK_REG, (0x1<<12));
    OUTREG16(BDMA_CLK_REG, u16Val | 0x400); //miu clock
    SETREG16(BDMA_CLK_REG, (0x1<<12));
#endif
}

void MHal_BDMA_Suspend(void)
{
#if ( BDMA_BASE_REG == REG_ADDR_BASE_BDMA )
    CLRREG16(BDMA_CLK_REG, (0x1<<4));
    SETREG16(BDMA_CLK_REG, (0x1<<0)); //gating
#else
    CLRREG16(BDMA_CLK_REG, (0x1<<12));
    SETREG16(BDMA_CLK_REG, (0x1<<8)); //gating
#endif
}

void MHal_BDMA_Resume(void)
{
    U16 u16Val;
    u16Val = INREG16(BDMA_CLK_REG);

#if ( BDMA_BASE_REG == REG_ADDR_BASE_BDMA )
    u16Val &= ~(0x00FF);
    CLRREG16(BDMA_CLK_REG, (0x1<<4));
    OUTREG16(BDMA_CLK_REG, u16Val | 0x4); //miu clock
    SETREG16(BDMA_CLK_REG, (0x1<<4));
#else
    u16Val &= ~(0xFF00);
    CLRREG16(BDMA_CLK_REG, (0x1<<12));
    OUTREG16(BDMA_CLK_REG, u16Val | 0x400); //miu clock
    SETREG16(BDMA_CLK_REG, (0x1<<12));
#endif
/*#if ( BDMA_BASE_REG == REG_ADDR_BASE_BDMA )
    CLRREG16(BDMA_CLK_REG, (0x1<<0));
    SETREG16(BDMA_CLK_REG, (0x1<<4));
#else
    CLRREG16(BDMA_CLK_REG, (0x1<<8));
    SETREG16(BDMA_CLK_REG, (0x1<<12));
#endif*/
}

