/*
 * $Id: //DAILEO/Columbus/IPCamera/source/iNfinity/iNfinity_ROM/source/include/platform.h#4 $
 * $Header: //DAILEO/Columbus/IPCamera/source/iNfinity/iNfinity_ROM/source/include/platform.h#4 $
 * $Date: 2015/06/10 $
 * $DateTime: 2015/06/10 16:00:37 $
 * $Change: 1251790 $
 * $File: //DAILEO/Columbus/IPCamera/source/iNfinity/iNfinity_ROM/source/include/platform.h $
 * $Revision: #4 $
 */

#ifndef PLATEFORM_H
#define PLATEFORM_H

#define SetDebugFlag(x) (REG(REG_ADDR_DEBUG)=x)



/*==========================================================================
    //CA4 Physical Address Mapping
===========================================================================*/

#define IMI_BASE_ADDR           0x00000000
#define RIU_BASE_ADDR           0x40000000
#ifdef BOOTROM_VERSION_ZEBU
  #define REG_ADDR_DEBUG            MIU0_START_ADDR
#else
  #define REG_ADDR_DEBUG            REG_ADDR_BASE_MAILBOX
#endif
#define FLAG_INIT_UART_BUSY         0x0BF1

/*==========================================================================
    Put the configuration base address of each IP here.
===========================================================================*/
#define GET_REG8_ADDR(x, y)  	      (x+(y)*2-((y)&1))
#define GET_REG16_ADDR(x, y)  	      (x+(y)*4)
#define GET_REG_OFFSET(x, y)  	      ((x)*0x200+(y)*0x4)

#define REG_ADDR_BASE_CHIPTOP         GET_REG8_ADDR( RIU_BASE_ADDR, 0x1E00 )
#define REG_ADDR_BASE_CLKGEN          GET_REG8_ADDR( RIU_BASE_ADDR, 0x40200 )

#define REG_ADDR_BASE_PM_UART0        GET_REG8_ADDR( RIU_BASE_ADDR, 0xC00 )
#define REG_ADDR_BASE_PM_SLEEP        GET_REG8_ADDR( RIU_BASE_ADDR, 0xE00 )
#define REG_ADDR_BASE_PM_RTC          GET_REG8_ADDR( RIU_BASE_ADDR, 0x1200 )
#define REG_ADDR_BASE_WDT             GET_REG8_ADDR( RIU_BASE_ADDR, 0x3000 )
#define REG_ADDR_BASE_TIMER0          GET_REG8_ADDR( RIU_BASE_ADDR, 0x3020 )
#define REG_ADDR_BASE_TIMER1          GET_REG8_ADDR( RIU_BASE_ADDR, 0x3040 )
#define REG_ADDR_BASE_TIMER2          GET_REG8_ADDR( RIU_BASE_ADDR, 0x3060 )

//#define REG_ADDR_BASE_MIUPLL          GET_REG8_ADDR( RIU_BASE_ADDR, 0x103100 )
#define REG_ADDR_BASE_MAILBOX         GET_REG8_ADDR( RIU_BASE_ADDR, 0x103380 )
#define REG_ADDR_BASE_INTC            GET_REG8_ADDR( RIU_BASE_ADDR, 0x1019C0 )
#define REG_ADDR_BASE_CPUINT          GET_REG8_ADDR( RIU_BASE_ADDR, 0x100540 )
#define REG_ADDR_BASE_VBDMA           GET_REG8_ADDR( RIU_BASE_ADDR, 0x151A00 )

#if 0
#if defined(BOOTROM_VERSION_FPGA)
    #define UART_BAUDRATE       115200
    #define UART_CLK            27000000
    #define TIMER_OSC           27000000
#elif defined(BOOTROM_VERSION_ASIC)
    #define UART_BAUDRATE       115200
    #define UART_CLK            172800000
    #define TIMER_OSC           12000000
#elif defined(BOOTROM_VERSION_ZEBU)
    #define UART_BAUDRATE       38400
    #define UART_CLK            12000000
    #define TIMER_OSC           12000000
#else
    #error "incorrect version!!"
#endif
#endif

#if defined(BOOTROM_VERSION_FPGA)
    #define UART_BAUDRATE       38400
    #define UART_CLK            12000000
    #define TIMER_OSC           12000000
#elif defined(BOOTROM_VERSION_ASIC)
    #define UART_BAUDRATE       115200
    #define UART_CLK            24000000
    #define TIMER_OSC           12000000
#elif defined(BOOTROM_VERSION_ZEBU)
    #define UART_BAUDRATE       38400
    #define UART_CLK            12000000
    #define TIMER_OSC           12000000
#else
    #error "incorrect version!!"
#endif


#endif //PLATEFORM_H
