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
/// @file   halUART.c
/// @brief  MStar UART HAL Driver DDI
/// @author MStar Semiconductor Inc.
/// @attention
/// <b><em></em></b>
///////////////////////////////////////////////////////////////////////////////////////////////////

#define _HAL_UART_C

#include "halUART.h"

#define __UART_INIT_PAD_CLK__

#define SetDebugFlag(x) (REG(REG_ADDR_DEBUG)=x)

#define REG_CHIPTOP_CHIPID   GET_REG16_ADDR(REG_ADDR_BASE_CHIPTOP, 0x67)
#define REG_UART_PAD_TOP   GET_REG16_ADDR(REG_ADDR_BASE_PADTOP, 0x57)
#define REG_UART_PIU_MISC  GET_REG16_ADDR(REG_ADDR_BASE_PIU_MISC, 0x18)
#define REG_UART_CLK_GLITCHMUX   GET_REG16_ADDR(REG_ADDR_BASE_CLKGEN, 0x7e)
#define REG_PM_SLEEP  GET_REG16_ADDR(REG_ADDR_BASE_PM_SLEEP, 0x7E)

#if defined(BOOTROM_VERSION_ASIC)
  #define UART_BASE        REG_ADDR_BASE_UART1
  #define REG_UART_CLK       GET_REG16_ADDR(REG_ADDR_BASE_CLKGEN, 0x45)
#elif defined(BOOTROM_VERSION_QFP)|| defined(BOOTROM_VERSION_CUST_SZ)
  #define UART_BASE        REG_ADDR_BASE_UART2
  #define REG_UART_CLK       GET_REG16_ADDR(REG_ADDR_BASE_CLKGEN, 0x46) //uart2
#else
  #define UART_BASE        REG_ADDR_BASE_UART1
  #define REG_UART_CLK       GET_REG16_ADDR(REG_ADDR_BASE_CLKGEN, 0x45)
#endif

#define UART_REG8(_x_)  ((U8 volatile *)(UART_BASE))[((_x_) * 4) - ((_x_) & 1)]

U8 uart_id(void)
{
#if ( UART_BASE == REG_ADDR_BASE_UART1 )
  return 1;
#elif ( UART_BASE == REG_ADDR_BASE_UART2 )
  return 2;
#else
  return 0;
#endif
}

void uart_init_pad_clk(void)
{
#ifndef BOOTROM_VERSION_ZEBU
    //U8 val;
    U16 u16Val;

    //C4P//
    //OUTREG8(REG_UART0_CLK, (val & 0xFFF0));

    //Murphy//OUTREG16(REG_PM_SLEEP, 4);
    // Enable uart0 clock
    //OUTREG8(REG_UART0_CLK, 0x09);  //clk_xtal and gating
    //CLRREG8(REG_UART0_CLK, 0x01);  //clear gating

    //C4P//
    u16Val = INREG16(REG_UART_CLK);

#if ( UART_BASE == REG_ADDR_BASE_UART1 )
/*
clk_uart1 clock setting
[0]: disable clock
[1]: invert clock
{reg_clkgen_dummy_7e[4], [3:2]}: Select clock source
     1000:  110.5 MHz
     1001:  88.4 MHz
     1010:  55.3 MHz
     1011:  48 MHz
     0xxx:  xtal div2
*/
    //CLRREG16(REG_UART_CLK_GLITCHMUX, (0x1<<4));
    //OUTREG16(REG_UART_CLK, (u16Val & 0x00FF));//110.5MHz
    //SETREG16(REG_UART_CLK_GLITCHMUX, (0x1<<4));

    u16Val = INREG16(REG_UART_PIU_MISC);
    u16Val &= ~(0xf<<4);
    OUTREG8(REG_UART_PIU_MISC, (u16Val | (1<<4))); //UART PAD1 map to which UART

    u16Val = INREG16(REG_UART_PAD_TOP);
    u16Val &= ~(3<<8);
    OUTREG16(REG_UART_PAD_TOP, (u16Val | (1<<8)));

#elif ( UART_BASE == REG_ADDR_BASE_UART2 )
/*
clk_fuart0 clock setting
[0]: disable clock
[1]: invert clock
[3:2]: Select clock source
     000:  clk_fuart0_synth_out_buf
     001:  192 MHz
     010:  176.8 MHz
     011:  147 MHz
*/
#if defined(BOOTROM_VERSION_CUST_SZ)
    u16Val = INREG16(REG_UART_CLK);
//    CLRREG16(REG_UART_CLK_GLITCHMUX, (0x1<<0));
    OUTREG16(REG_UART_CLK, ((u16Val & 0xFFF0)|0x0004)); // 011:  147 MHz 001:  192 MHz

    u16Val = INREG16(REG_UART_PIU_MISC);
    u16Val &= ~(0xf<<8);
    OUTREG8(REG_UART_PIU_MISC, (u16Val | (2<<8))); //UART PAD2 map to which UART
   // OUTREG8(REG_UART_PIU_MISC, (u16Val | (1<<4))); //UART PAD1 map to which UART

    u16Val = INREG16(REG_UART_PAD_TOP);
    u16Val &= ~(3<<10);
    OUTREG16(REG_UART_PAD_TOP, (u16Val | (1<<10)));

#else
  if (((INREG16(REG_CHIPTOP_CHIPID)&0xff00)>>8) == 0)
  {
    u16Val = INREG16(REG_UART_CLK);
//    CLRREG16(REG_UART_CLK_GLITCHMUX, (0x1<<0));
    OUTREG16(REG_UART_CLK, ((u16Val & 0xFFF0)|0x0004)); // 011:  147 MHz 001:  192 MHz
//    SETREG16(REG_UART_CLK_GLITCHMUX, (0x1<<0));
    u16Val = INREG16(REG_UART_PIU_MISC);
    u16Val &= ~(0xf<<4);
    OUTREG8(REG_UART_PIU_MISC, (u16Val | (1<<4))); //UART PAD1 map to which UART
    u16Val = INREG16(REG_UART_PAD_TOP);
    u16Val &= ~(3<<10);
    OUTREG16(REG_UART_PAD_TOP, (u16Val | (1<<10)));
  }
  else
  {
    u16Val = INREG16(REG_UART_CLK);
//    CLRREG16(REG_UART_CLK_GLITCHMUX, (0x1<<0));
    OUTREG16(REG_UART_CLK, ((u16Val & 0xFFF0)|0x0004)); // 011:  147 MHz 001:  192 MHz
//    SETREG16(REG_UART_CLK_GLITCHMUX, (0x1<<0));
    u16Val = INREG16(REG_UART_PIU_MISC);
    u16Val &= ~(0xf<<4);
    OUTREG8(REG_UART_PIU_MISC, (u16Val | (2<<4))); //UART PAD1 map to UART2
    u16Val = INREG16(REG_UART_PAD_TOP);
    u16Val &= ~(3<<8);
    OUTREG16(REG_UART_PAD_TOP, (u16Val | (3<<8))); // to TTL
  }
#endif
#else
/*
clk_uart0 clock setting
[0]: disable clock
[1]: invert clock
{reg_clkgen_dummy_7e[0], [3:2]}: Select clock source
     1000:  110.5 MHz
     1001:  88.4 MHz
     1010:  55.3 MHz
     1011:  48 MHz
     0xxx:
*/
    CLRREG16(REG_UART_CLK_GLITCHMUX, (0x1<<0));
    OUTREG16(REG_UART_CLK, (u16Val & 0xFF00)); //110.5M
    SETREG16(REG_UART_CLK_GLITCHMUX, (0x1<<0));
#endif
}

void uart_init(void)
{
  U8 count=0;

#if defined(__UART_INIT_PAD_CLK__)
  uart_init_pad_clk();
#endif
  // Reset RX_enable
  //CLRREG16(REG_RX_ENABLE, BIT11);

  // Reset PM uart pad digmux
  //OUTREG16(REG_UART_SEL3210, 0x3210);

  // Toggle SW reset
  UART_REG8(UART_RST) &= ~0x01;
  UART_REG8(UART_RST) |= 0x01;

  // Disable all interrupts
  UART_REG8(UART_IER) = 0x00;

  // Set "reg_mcr_loopback";
  UART_REG8(UART_MCR) |= 0x10;

  // Poll "reg_usr_busy" till 0; (10 times)
  while(UART_REG8(UART_USR) & 0x01 && count++ < 10)
      ;

  if(count == 10)
  {
      SetDebugFlag(FLAG_INIT_UART_BUSY); /* 0x0BF1 */
  }
  else // Set divisor
  {
#if defined(BOOTROM_VERSION_FPGA)
      U16 DLR = ((UART_CLK+(8*UART_BAUDRATE)) / (16 * UART_BAUDRATE));
#else
      U16 DLR = (UART_CLK / (16 * UART_BAUDRATE));
#endif
      UART_REG8(UART_LCR) |= UART_LCR_DLAB;
      UART_REG8(UART_DLL) = DLR & 0xFF;
      UART_REG8(UART_DLM) = (DLR >> 8) & 0xFF;
      UART_REG8(UART_LCR) &= ~(UART_LCR_DLAB);
  }

  // Set 8 bit char, 1 stop bit, no parity
  UART_REG8(UART_LCR) = UART_LCR_WLEN8 & ~(UART_LCR_STOP2 | UART_LCR_PARITY);

  // Unset loopback
  UART_REG8(UART_MCR) &= ~0x10;

  // Enable TX/RX fifo
  UART_REG8(UART_FCR) = UART_FCR_ENABLE_FIFO | UART_FCR_CLEAR_RCVR | UART_FCR_CLEAR_XMIT;

  // Set PM uart pad digmux to UART0
  //OUTREG16(REG_UART_SEL3210, 0x3012);

  // Set RX_enable
  //SETREG16(REG_RX_ENABLE, BIT11);
#else
  UART_REG8(UART_LCR) |= UART_LCR_DLAB;
  UART_REG8(UART_DLL) = 0x04;
  UART_REG8(UART_DLM) = 0;
  UART_REG8(UART_LCR) &= ~(UART_LCR_DLAB);
  UART_REG8(UART_LCR) = UART_LCR_WLEN8 & ~(UART_LCR_STOP2 | UART_LCR_PARITY);
  UART_REG8(UART_MCR) = UART_MCR_AFCE | UART_MCR_RTS;
  UART_REG8(UART_FCR) = UART_FCR_ENABLE_FIFO | UART_FCR_CLEAR_RCVR | UART_FCR_CLEAR_XMIT;
#endif
}

void uart_int_enable(void)
{
    // Enable all interrupts
  UART_REG8(UART_IER) = UART_IER_RDI | UART_IER_RLSI | UART_IER_MSI;
}

U8 uart_int_source(void)
{
	return UART_REG8(UART_IIR) & UART_IIR_ID_MASK;
}

void uart_clear_msr(void)
{
	U8 nSource=0;
  nSource = UART_REG8(UART_MSR);

  (void)nSource;
}

void uart_clear_lsr(void)
{
	U8 nSource=0;
  nSource = UART_REG8(UART_LSR);
  (void)nSource;
}

U8 uart_read_byte(void)
{
  while (!(UART_REG8(UART_LSR) & UART_LSR_DR));
  return UART_REG8(UART_RX);
}

// void uart_write_byte(U8 c)
// {
    // uart_start_putk(c);
// }

void uart_write_byte(U8 c)
{
    while (!(UART_REG8(UART_LSR) & UART_LSR_THRE));

    UART_REG8(UART_TX) = c;
}

// int fputc(int ch, FILE *f)
// {
    // f=f;

    // while (!(UART_REG8(UART_LSR) & UART_LSR_THRE));

    // UART_REG8(UART_TX) = ch;

    // return ch;
// }


// int fgetc(FILE *f)
// {
    // f=f;

    // while (!(UART_REG8(UART_LSR) & UART_LSR_DR));

    // return UART_REG8(UART_RX);//

// }
void uart_write_string(U8 *ptr)
{
    while((*ptr)!=0)
    {
    uart_write_byte(*ptr++);
    }
}

//#if (BR_DEBUG_MSG || BOOT_TIME_MEASURE)
void uart_write_U8_hex(U8 c)
{
    U8 tmp=0;
    tmp=c;
    if((c>>4)>=10)
    {
        c=(0x61+(c>>4)-10);
        uart_write_byte(c);
    }
    else
    {
        c=(0x30+(c>>4));
        uart_write_byte(c);
    }

    c=tmp;
    if((c&0x0f)>=10)
    {
        c=(0x61+(c&0x0f)-10);
        uart_write_byte(c);
    }
    else
    {
        c=(0x30+(c&0x0f));
        uart_write_byte(c);
    }

}

void uart_write_U16_hex(U16 val)
{
    U16 tmp=0;
    U8 *ptr=NULL;
    int i=0;
    tmp=val;

    ptr = (U8 *) &tmp;
    for(i=1;i>=0;i--)
    {
        uart_write_U8_hex(ptr[i]);
    }
}

void uart_write_U32_hex(U32 val)
{
    U32 tmp=0;
    U8 *ptr=NULL;
    int i=0;
    tmp=val;

    ptr = (U8 *) &tmp;
    for(i=3;i>=0;i--)
    {
        uart_write_U8_hex(ptr[i]);
    }
}


// void uart_dump_memory(U32 addr)
// {
    // U32 value=0;

    // uart_write_byte('[');
    // uart_write_byte('0');
    // uart_write_byte('x');
    // uart_write_U32_hex(addr);
    // uart_write_byte(']');
    // uart_write_byte(':');
    // value = (*((U32 *) addr));

    // uart_write_byte('0');
    // uart_write_byte('x');
    // uart_write_U32_hex(value);
    // uart_write_byte('\r');
    // uart_write_byte('\n');
// }

