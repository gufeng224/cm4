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


#define REG_ADDR_BASE_PADTOP          GET_REG8_ADDR( RIU_BASE_ADDR, 0xB00 )
#define REG_ADDR_BASE_ECBBRIDGE       GET_REG8_ADDR( RIU_BASE_ADDR, 0x1000 )
#define REG_ADDR_BASE_CHIPTOP         GET_REG8_ADDR( RIU_BASE_ADDR, 0x1E00 )
//#define REG_ADDR_BASE_WDT             GET_REG8_ADDR( RIU_BASE_ADDR, 0x3460 )
#define REG_ADDR_BASE_RTC             GET_REG8_ADDR( RIU_BASE_ADDR, 0x2400 )
#define REG_ADDR_BASE_TIMER0          GET_REG8_ADDR( RIU_BASE_ADDR, 0x34E0 )
#define REG_ADDR_BASE_TIMER1          GET_REG8_ADDR( RIU_BASE_ADDR, 0x3BC0 )
#define REG_ADDR_BASE_TIMER2          GET_REG8_ADDR( RIU_BASE_ADDR, 0x3BE0 )
#define REG_ADDR_BASE_TIMER3          GET_REG8_ADDR( RIU_BASE_ADDR, 0x3D80 )
#define REG_ADDR_BASE_TIMER4          GET_REG8_ADDR( RIU_BASE_ADDR, 0x3DA0 )

#define REG_ADDR_BASE_UART0           GET_REG8_ADDR( RIU_BASE_ADDR, 0x3B00 )
#define REG_ADDR_BASE_UART1           GET_REG8_ADDR( RIU_BASE_ADDR, 0x3B40 )
#define REG_ADDR_BASE_UART2           GET_REG8_ADDR( RIU_BASE_ADDR, 0x3B80 )
#define REG_ADDR_BASE_UART3           GET_REG8_ADDR( RIU_BASE_ADDR, 0x3E00 )

#define REG_ADDR_BASE_PIU_MISC        GET_REG8_ADDR( RIU_BASE_ADDR, 0x3C00 )
#define REG_ADDR_BASE_PMU             GET_REG8_ADDR( RIU_BASE_ADDR, 0x3F00 )
#define REG_ADDR_BASE_CLKGEN          GET_REG8_ADDR( RIU_BASE_ADDR, 0x40200 )

#define REG_ADDR_BASE_PM_UART0        GET_REG8_ADDR( RIU_BASE_ADDR, 0xC00 )
#define REG_ADDR_BASE_PM_SLEEP        GET_REG8_ADDR( RIU_BASE_ADDR, 0xE00 )
#define REG_ADDR_BASE_MAILBOX         GET_REG8_ADDR( RIU_BASE_ADDR, 0x3300 )
#define REG_ADDR_BASE_CPUINT          GET_REG8_ADDR( RIU_BASE_ADDR, 0x20940 )
#define REG_ADDR_BASE_BDMA            GET_REG8_ADDR( RIU_BASE_ADDR, 0x20B00 )
#define REG_ADDR_BASE_VBDMA           GET_REG8_ADDR( RIU_BASE_ADDR, 0x20F00 )
#define REG_ADDR_BASE_tzpc_nonpm_2nd  GET_REG8_ADDR( RIU_BASE_ADDR, 0x30600 )
#define REG_ADDR_BASE_MISC_FROPLL     GET_REG8_ADDR( RIU_BASE_ADDR, 0x40C00 )
#define REG_ADDR_BASE_COIN            GET_REG8_ADDR( RIU_BASE_ADDR, 0x41F00 )


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
    #define UART_CLK            13000000
//    #define UART_CLK            110500000
    #define TIMER_OSC           13000000
#elif defined(BOOTROM_VERSION_ZEBU)
    #define UART_BAUDRATE       38400
    #define UART_CLK            12000000
    #define TIMER_OSC           12000000
#elif defined(BOOTROM_VERSION_QFP) || defined(BOOTROM_VERSION_CUST_SZ)
/*
    uart2
    BAUDRATE 9600
    CLK 147000000
*/
    #define UART_BAUDRATE       115200
    #define UART_CLK            192000000
    #define TIMER_OSC           12000000
#elif defined(BOOTROM_VERSION_MST291A) || defined(BOOTROM_VERSION_MST291B) || defined(BOOTROM_VERSION_MST291C) || defined(BOOTROM_VERSION_MST291C_D01B) || defined(BOOTROM_VERSION_MSTDB306)
    #define UART_BAUDRATE       115200
    #define UART_CLK            192000000
//    #define UART_CLK            110500000
    #define TIMER_OSC           12000000
#else
    #error "incorrect version!!"
#endif

#endif //PLATEFORM_H
