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

#define SetDebugFlag(x) (REG(REG_ADDR_DEBUG)=x)

#ifndef BOOTROM_VERSION_ZEBU
  //#define UART_BASE        REG_ADDR_BASE_UART0
  #define UART_BASE        REG_ADDR_BASE_PM_UART0
  #define UART_REG8(_x_)  ((U8 volatile *)(UART_BASE))[((_x_) * 4) - ((_x_) & 1)]
  //#define REG_UART_SEL3210    GET_REG16_ADDR(REG_ADDR_BASE_CHIPTOP, 0x53)
  //#define REG_RX_ENABLE       GET_REG16_ADDR(REG_ADDR_BASE_PM_SLEEP, 0x09)
  //#define REG_UART0_CLK       GET_REG16_ADDR(REG_ADDR_BASE_CLKGEN, 0x45)
  #define REG_PM_UART_MUX  GET_REG16_ADDR(REG_ADDR_BASE_PM_SLEEP, 0x7E)
  #define REG_PM_UART_PAD  GET_REG16_ADDR(REG_ADDR_BASE_PM_SLEEP, 0x76)
  #define REG_PM_UART_CLK  GET_REG16_ADDR(REG_ADDR_BASE_PM_SLEEP, 0x71)
#else
  #define UART_BASE        REG_ADDR_BASE_FUART
  #define UART_REG8(_x_)  ((U8 volatile *)(UART_BASE))[((_x_) * 4) - ((_x_) & 1)]
#endif

U8 uart_id(void)
{
#if ( UART_BASE == REG_ADDR_BASE_UART1 )
  return 1;
#else
  return 0;
#endif
}

/*
//Clock
bank= 0x0e    addr16=0x71 B[12:8]

[12:10]
000 : 192M
001 : 160M
010 : 144M
011 : 123M
100 : 108M
101 : XTAL 24M
110 : NA
111 : 3M
[9] INV
[8] GATE

//MUX
select uart source via  PAD_DDCA
1 => cm4 uart0
0 => hk51_uart0 / DIE UART0
bank= 0x0e    addr16=0x7e B[2]= 1  //reg_cm4_uart0_en

//PAD MUX
bank= 0x0e    addr16=0x76 B[9:6]
[2:0]
'd1: swtich gpio_pm[5] to cm4's uart_rx0 and gpio_pm[1] to cm4's uart_tx0
'd2: swtich gpio_pm[3] to cm4's uart_rx0 and gpio_pm[8] to cm4's uart_tx0
'd3: swtich gpio_pm[5] to cm4's uart_rx0 and gpio_pm[8] to cm4's uart_tx0
'd4: swtich gpio_pm[11] to cm4's uart_rx0 and gpio_pm[12] to cm4's uart_tx0
[3]: switch cm4's uart_tx0 as open drain
*/

void uart_init(void)
{
#ifndef BOOTROM_VERSION_ZEBU
    U8 count=0;

    U16 u16Val;

    //SETREG16(REG_PM_UART_MUX, (0x1<<2));

    // default not set uart pad
//    u16Val = INREG16(REG_PM_UART_PAD);
//    u16Val &= ~(7<<6);
//    OUTREG16(REG_PM_UART_PAD, (u16Val |(0x1<<6)));

    // Enable uart clock
    u16Val = INREG16(REG_PM_UART_CLK);
    u16Val &= ~(0x1f<<8);
    OUTREG16(REG_PM_UART_CLK, (u16Val | (0x14<<8)));

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
        //U16 DLR = ((UART_CLK+(8*UART_BAUDRATE)) / (16 * UART_BAUDRATE));
        U16 DLR = (UART_CLK / (16 * UART_BAUDRATE));
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

#if defined(__CONFIG_UART_MUX__)
void uart_mux(U8 en)
{
  if (en)
    SETREG16(REG_PM_UART_MUX, (0x1<<2));
  else
    CLRREG16(REG_PM_UART_MUX, (0x1<<2));
}
#endif

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

