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
/// @file   halPowerSaving.c
/// @brief  MStar Power Saving Settings
/// @author MStar Semiconductor Inc.
/// @attention
/// <b><em></em></b>
///////////////////////////////////////////////////////////////////////////////////////////////////

#define _HAL_POWER_SAVING_C

#include "halPowerSaving.h"
#include "mosWrapper.h"
#include "halTimer.h"

//Debug tool

#define ENABLE_DEBUG_BY_FORCE_WAKEUP2  (0) //see also: main_full/voc_task.c
#define ENABLE_CALC_RESUME_TIME        (1)

//Power setting
#define ENABLE_SYS_SRAM_OFF            (1)
#define ENABLE_AUSDM_OFF               (1)
#define BOARD_XTAL_POWER_FROM_VLCM     (1) //1:XTAL from VLCM, 0: XTAL from VABB,




#define POWER_SAVING_LOG(fmt, args...) MOS_DBG_ERROR("[POWER_SAVING] " fmt, ##args);


#define SCRIPT_ADDR_TO_CPU_ADDR(x)  (RIU_BASE_ADDR + ((x) << 1)  - ((x) & 0x1))

//==============================================================================
//sync with arch\arm\mach-mstar\cleveland\pm.c
#define PM_CM4_FLAG_ENABLE_UART                0x0020
#define PM_CM4_FLAG_ENABLE_AMIC                0x0040
#define PM_CM4_FLAG_SKIP_POWER_SETTING_ALL     0x0080
#define PM_CM4_FLAG_SKIP_POWER_SETTING_USB     0x0100
//==============================================================================

U8 g_u8PowerSavingBackup_1133;
U8 g_u8PowerSavingBackup_1160;
U8 g_u8PowerSavingBackup_1154;
U8 g_u8PowerSavingBackup_118b;
U8 g_u8PowerSavingBackup_1108;
U8 g_u8PowerSavingBackup_1184;
U8 g_u8PowerSavingBackup_1186;
U8 g_u8PowerSavingBackup_10800;
U8 g_u8PowerSavingBackup_40A03;
U8 g_u8PowerSavingBackup_40320;
U8 g_u8PowerSavingBackup_40323;
U16 g_u16PowerSavingBackup_43120;
U16 g_u16PowerSavingBackup_43122;
U16 g_u16PowerSavingBackup_31020;
U16 g_u16PowerSavingBackup_31022;
U16 g_u16PowerSavingBackup_13920;

#if ENABLE_AUSDM_OFF
U8 g_u8PowerSavingBackup_321F0;
U8 g_u8PowerSavingBackup_321D9;
U8 g_u8PowerSavingBackup_321E3;
U8 g_u8PowerSavingBackup_321DA;
U8 g_u8PowerSavingBackup_321DD;
U8 g_u8PowerSavingBackup_321DC;
U8 g_u8PowerSavingBackup_321E1;
U8 g_u8PowerSavingBackup_321E0;
U8 g_u8PowerSavingBackup_321EA;
U8 g_u8PowerSavingBackup_321E6;
U8 g_u8PowerSavingBackup_321E9;
U8 g_u8PowerSavingBackup_321EC;
U8 g_u8PowerSavingBackup_32176;
#endif
//==============================================================================
// APIs
//==============================================================================
#if ENABLE_CALC_RESUME_TIME
static void latch_rtc_timer(void)
{
    SETREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x000241C),0x01);  //RTC Bank_24_REG_0E[0]
    //wait 190us for latching ready.
}
#endif
//==============================================================================
static bool check_pm_flag(U16 u16Flag)
{
    if (INREG16(SCRIPT_ADDR_TO_CPU_ADDR(0x003F34)) & u16Flag) //BK_3F_REG_1A
        return true;

    return false;
}

#if ENABLE_AUSDM_OFF
static int is_amic_enabled(void)
{
    return check_pm_flag(PM_CM4_FLAG_ENABLE_AMIC);
}
#endif

static int is_uart_enabled(void)
{
    return check_pm_flag(PM_CM4_FLAG_ENABLE_UART);
}

static int is_xtal_24mhz(void)
{
    if (INREG16(SCRIPT_ADDR_TO_CPU_ADDR(0x001E16)) & 0x0020) //BK_1E_REG_06  //24MHz XTAL
        return true;

    return false; //26MHz XTAL
}

static int is_upll_not_power_off(void)
{
    if ((INREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x010800)) & 0x32) != 0x32)
        return true;

    return false;
}
//==============================================================================
//usage:
//      if (Get_Dynamic_Setting_by_MailBox(&Value))
//          OUTREG16(GET_REG16_ADDR(REG_ADDR_BASE_xxxxx, 0x00),Value);

int Get_Dynamic_Setting_by_MailBox(U16 *pSetting)
{
     if (INREG16(GET_REG16_ADDR(REG_ADDR_BASE_MAILBOX, 0x00)) != 0x8888)
        return 0;

    *pSetting = INREG16(GET_REG16_ADDR(REG_ADDR_BASE_MAILBOX, 0x01));
    return 1;
}
//==============================================================================
int check_flag_bit(int iIndex)
{
    if (INREG16(GET_REG16_ADDR(REG_ADDR_BASE_MAILBOX, 0x00))&iIndex)
    {
        POWER_SAVING_LOG("Flag ON:0x%04x\n",iIndex);
        return true;
    }
    return false;
}
//==============================================================================
static void Set_Sram_Power_Control_Clock_Enable(int iEnable)
{
    if (iEnable)
    {
        //Enable sram power control clock
        CLRREG8(GET_REG16_ADDR(REG_ADDR_BASE_CLKGEN, 0x15),0x01);
        MHal_Timer_Delay(TIMER_DELAY_1us);
    }
    else
    {
        //Disable sram power control clock
        SETREG8(GET_REG16_ADDR(REG_ADDR_BASE_CLKGEN, 0x15),0x01);
    }
}
//==============================================================================
void Mhal_Power_Saving_10_DDR_Atop_Power_Off(void)
{
    /*
    wriu -b 0x001203 0xF0 0xF0           //cke, dq, adr, cko oenz=1
    wriu -b 0x001100 0x08 0x08           //gpio mode on
    wriu -b 0x001133 0xc0 0xc0           //crash if run by CA7
    wriu -b 0x001160 0x02 0x02
    wriu -b 0x001154 0x70 0x70
    wriu -b 0x00118b 0x02 0x02           //CA7 can't run on spi_nor flash if set, need to recover before waking CA7 up.
    wriu -b 0x001108 0x3f 0x00
    wriu -b 0x001184 0x08 0x08
    wriu -b 0x001186 0x08 0x08           //ddr crc fail
    */

    //POWER_SAVING_LOG("DDR_ATOP\n");
    //Save settings
    g_u8PowerSavingBackup_1133 = INREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x001133));
    g_u8PowerSavingBackup_1160 = INREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x001160));
    g_u8PowerSavingBackup_1154 = INREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x001154));
    g_u8PowerSavingBackup_118b = INREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x00118b));
    g_u8PowerSavingBackup_1108 = INREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x001108));
    g_u8PowerSavingBackup_1184 = INREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x001184));
    g_u8PowerSavingBackup_1186 = INREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x001186));



    SETREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x001203),0xF0);
    SETREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x001100),0x08);

    SETREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x001133),0xc0);

    SETREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x001160),0x02);
    SETREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x001154),0x70);
    SETREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x00118b),0x02);
    CLRREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x001108),0x3f);
    SETREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x001184),0x08);
    SETREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x001186),0x08);

}


void Mhal_Power_Saving_10_DDR_Atop_Power_On(void)
{
    #if 0 // DDR CRC Fail
    CLRREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x00118b),0x02);
    CLRREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x001186),0x08);
    #else

    OUTREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x001133),g_u8PowerSavingBackup_1133);

    OUTREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x001160),g_u8PowerSavingBackup_1160);
    OUTREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x001154),g_u8PowerSavingBackup_1154);
    OUTREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x00118b),g_u8PowerSavingBackup_118b);
    OUTREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x001108),g_u8PowerSavingBackup_1108);
    OUTREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x001184),g_u8PowerSavingBackup_1184);
    OUTREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x001186),g_u8PowerSavingBackup_1186);
    #endif

}

//==============================================================================
void Mhal_Power_Saving_13_MIU128PLL_ARMPLL_Off(void)
{
    /*
    //MIU128_PLL
    wriu 0x00040863 0x01

    //[power down ARMPLL]
    wriu 0x00040923 0x01
    */

    OUTREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x00040863),0x01);
    OUTREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x00040923),0x01);
}

void Mhal_Power_Saving_13_MIU128PLL_ARMPLL_On(void)
{
    //do nothing; Re-Init by IPL.
}

//==============================================================================
void Mhal_Power_Saving_12_UPLL_MPLL_Off(void)
{
    if (check_pm_flag(PM_CM4_FLAG_SKIP_POWER_SETTING_USB))
        return;


    if (is_uart_enabled() && is_xtal_24mhz()) //UPLL(48Mhz) is used by UART1 when xtal@24mhz
        return;

    /*
    //UPLL0
    wriu 0x00010800 0x32
    //MPLL
    wriu 0x00040a03 0x0f
    */

    g_u8PowerSavingBackup_10800 = INREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x00010800));
    OUTREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x00010800),0x32);

    if (is_amic_enabled()) //do not turn off MPLL if use AMIC
        return;

    g_u8PowerSavingBackup_40A03 = INREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x00040a03));
    OUTREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x00040a03),0x0f);

}



void Mhal_Power_Saving_12_UPLL_MPLL_On(void)
{
    if (check_pm_flag(PM_CM4_FLAG_SKIP_POWER_SETTING_USB))
        return;


    if (is_uart_enabled() && is_xtal_24mhz()) //UPLL(48Mhz) is used by UART1 when xtal@24mhz
        return;

    //if boot form SPI_ROM ,no need to turn upll/mpll on.
    OUTREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x00010800),g_u8PowerSavingBackup_10800);

    if (is_amic_enabled())
        return;

    OUTREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x00040a03),g_u8PowerSavingBackup_40A03);
}

//==============================================================================
static void Mhal_Power_Saving_15_Vlcm_Off(void)
{
    if (is_uart_enabled())
        return;

    #if BOARD_XTAL_POWER_FROM_VLCM
    //Do not turn off vlcm if use AMIC.
    if (is_amic_enabled())
        return;
    #endif

    if (is_upll_not_power_off())
        return;

    /*
    //VLCM off
    wriu 0x00003f03 0x00
    */
    //POWER_SAVING_LOG("disabled VLCM\n");

    OUTREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x00003f03),0x00);  //turn off VLCM


}

static void Mhal_Power_Saving_15_Vlcm_On(void)
{
    if (is_uart_enabled())
        return;

    #if BOARD_XTAL_POWER_FROM_VLCM
    if (is_amic_enabled())
        return;
    #endif

    if (is_upll_not_power_off())
        return;

    OUTREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x00003f03),0x21);  //turn on VLCM

}

//==============================================================================
static void Mhal_Power_Saving_14_Xtal_Off(void)
{
    if (is_uart_enabled())
        return;

    if (is_amic_enabled())
        return;

    if (is_upll_not_power_off())
        return;

    /*
    //File:12.CM4_3_XTAL_Off.txt
    wriu -w 0x00001E68 0x9F8E
    wriu -w 0x00001E6A 0x9F8E
    wriu -b 0x00001E60 0x02 0x00
    */

    OUTREG16(SCRIPT_ADDR_TO_CPU_ADDR(0x00001E68),0x9F8E);  //turn off XTAL
    OUTREG16(SCRIPT_ADDR_TO_CPU_ADDR(0x00001E6A),0x9F8E);  //turn off XTAL
    CLRREG8 (SCRIPT_ADDR_TO_CPU_ADDR(0x00001E60),0x02);
}

static void Mhal_Power_Saving_14_Xtal_On(void)
{
    if (is_uart_enabled())
        return;

    if (is_amic_enabled())
        return;

    if (is_upll_not_power_off())
        return;

    OUTREG16(SCRIPT_ADDR_TO_CPU_ADDR(0x00001E68),0x0000);  //turn on XTAL
}

//==============================================================================
static void Mhal_Power_Saving_11_USB_P0_Off(void)
{
    if (check_pm_flag(PM_CM4_FLAG_SKIP_POWER_SETTING_USB))
        return;

    if (is_uart_enabled() && is_xtal_24mhz()) //UPLL(48Mhz) is used by UART1 when xtal@24mhz
        return;

    /*
    // USB2 P0
    wriu 0x010200 0xc7
    wriu 0x010201 0xff
    wriu 0x010208 0xaf
    wriu 0x010210 0x00
    //USBBC0
    wriu 0x001d03 0x50
    wriu 0x001d00 0x7f
    wriu 0x001d02 0x00
    */

    OUTREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x010200),0xc7);
    OUTREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x010201),0xff);
    OUTREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x010208),0xaf);
    OUTREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x010210),0x00);

    OUTREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x001d03),0x50);
    OUTREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x001d00),0x7f);
    OUTREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x001d02),0x00);
}

static void Mhal_Power_Saving_11_USB_P0_On(void)
{
    if (check_pm_flag(PM_CM4_FLAG_SKIP_POWER_SETTING_USB))
        return;

    //USB P1
    SETREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x010290),0x40);
    //USB P2
    SETREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x010310),0x40);

    if (is_uart_enabled() && is_xtal_24mhz()) //UPLL(48Mhz) is used by UART1 when xtal@24mhz
        return;
    //UPB P0
    SETREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x010210),0x40);

    //Others: ReInit by IPL.
}

//==============================================================================
void Mhal_Power_Saving_0F_Sys_Sram_Off(void)
{
    /*
    //DIG_RAM ==> miu_0/1 ==> OFF
    wriu    0x00040320 0x03
    //DIG_RAM ==> BDMA / VBDMA ==> OFF
    wriu    0x00040323 0x50
    //GPU_BLOCK
    wriu -w 0x00043120 0xffff
    wriu -w 0x00043122 0xffff
    //MM_BLOBK
    wriu -w 0x00031020 0xffff
    wriu -w 0x00031022 0xffff
    //VA_SYS_BLOCK ==> HAYDN ON / others OFF
    wriu -w 0x00013920 0xff7f
    */


    #if ENABLE_SYS_SRAM_OFF
    Set_Sram_Power_Control_Clock_Enable(1);
    g_u8PowerSavingBackup_40320  =  INREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x00040320));
    g_u8PowerSavingBackup_40323  =  INREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x00040323));
    g_u16PowerSavingBackup_43120 = INREG16(SCRIPT_ADDR_TO_CPU_ADDR(0x00043120));
    g_u16PowerSavingBackup_43122 = INREG16(SCRIPT_ADDR_TO_CPU_ADDR(0x00043122));
    g_u16PowerSavingBackup_31020 = INREG16(SCRIPT_ADDR_TO_CPU_ADDR(0x00031020));
    g_u16PowerSavingBackup_31022 = INREG16(SCRIPT_ADDR_TO_CPU_ADDR(0x00031022));
    g_u16PowerSavingBackup_13920 = INREG16(SCRIPT_ADDR_TO_CPU_ADDR(0x00013920));

    OUTREG8 (SCRIPT_ADDR_TO_CPU_ADDR(0x00040320),0x03);
    OUTREG8 (SCRIPT_ADDR_TO_CPU_ADDR(0x00040323),0x50);
    OUTREG16(SCRIPT_ADDR_TO_CPU_ADDR(0x00043120),0xFFFF);
    OUTREG16(SCRIPT_ADDR_TO_CPU_ADDR(0x00043122),0xFFFF);
    OUTREG16(SCRIPT_ADDR_TO_CPU_ADDR(0x00031020),0xFFFF);
    OUTREG16(SCRIPT_ADDR_TO_CPU_ADDR(0x00031022),0xFFFF);
    OUTREG16(SCRIPT_ADDR_TO_CPU_ADDR(0x00013920),0xFF73);
    MHal_Timer_Delay(TIMER_DELAY_1ms);
    Set_Sram_Power_Control_Clock_Enable(0);
    #endif
}

void Mhal_Power_Saving_0F_Sys_Sram_On(void)
{
    #if ENABLE_SYS_SRAM_OFF
    Set_Sram_Power_Control_Clock_Enable(1);
    OUTREG8 (SCRIPT_ADDR_TO_CPU_ADDR(0x00040320),g_u8PowerSavingBackup_40320);
    OUTREG8 (SCRIPT_ADDR_TO_CPU_ADDR(0x00040323),g_u8PowerSavingBackup_40323);
    OUTREG16(SCRIPT_ADDR_TO_CPU_ADDR(0x00043120),g_u16PowerSavingBackup_43120);
    OUTREG16(SCRIPT_ADDR_TO_CPU_ADDR(0x00043122),g_u16PowerSavingBackup_43122);
    OUTREG16(SCRIPT_ADDR_TO_CPU_ADDR(0x00031020),g_u16PowerSavingBackup_31020);
    OUTREG16(SCRIPT_ADDR_TO_CPU_ADDR(0x00031022),g_u16PowerSavingBackup_31022);
    OUTREG16(SCRIPT_ADDR_TO_CPU_ADDR(0x00013920),g_u16PowerSavingBackup_13920);
    MHal_Timer_Delay(TIMER_DELAY_1ms);
    Set_Sram_Power_Control_Clock_Enable(0);
    #endif
}

//==============================================================================
void Mhal_Power_Saving_0E_Fuart_Off(void)
{

    #if 0
    U8 u8Tmp;
    //reg_ckg_fuart0_synth_in to xtal
    u8Tmp = INREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x0004028E));
    u8Tmp &= 0xF0;
    u8Tmp |= 0x08; //bit[3:0]= 2b1000
    OUTREG8 (SCRIPT_ADDR_TO_CPU_ADDR(0x0004028E),u8Tmp);

    //set reg_ckg_fuart0 to clk_fuart0_synth_out_buf
    u8Tmp = INREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x0004028C));
    u8Tmp &= 0xF0; //bit[3:0]= 0
    OUTREG8 (SCRIPT_ADDR_TO_CPU_ADDR(0x0004028C),u8Tmp);
    #else
    /*
    //clk_fuart0/1 switch to XTAL
    wriu -w 0x0004028c 0x0000
    wriu -w 0x0004028e 0x0808
    */
    OUTREG16(SCRIPT_ADDR_TO_CPU_ADDR(0x0004028c),0x0000);
    OUTREG16(SCRIPT_ADDR_TO_CPU_ADDR(0x0004028e),0x0808);
    #endif
}

void Mhal_Power_Saving_0E_Fuart_On(void)
{

}

//==============================================================================
void Mhal_Power_Saving_0D_Ausdm_Off(void)
{
    #if (ENABLE_AUSDM_OFF == 0)
        return;
    #endif

    g_u8PowerSavingBackup_321F0  =  INREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x0321F0));
    g_u8PowerSavingBackup_321D9  =  INREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x0321D9));
    g_u8PowerSavingBackup_321E3  =  INREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x0321E3));
    g_u8PowerSavingBackup_321DA  =  INREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x0321DA));
    g_u8PowerSavingBackup_321DD  =  INREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x0321DD));
    g_u8PowerSavingBackup_321DC  =  INREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x0321DC));
    g_u8PowerSavingBackup_321E1  =  INREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x0321E1));
    g_u8PowerSavingBackup_321E0  =  INREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x0321E0));
    g_u8PowerSavingBackup_321EA  =  INREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x0321EA));
    g_u8PowerSavingBackup_321E6  =  INREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x0321E6));
    g_u8PowerSavingBackup_321E9  =  INREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x0321E9));
    g_u8PowerSavingBackup_321EC  =  INREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x0321EC));
    g_u8PowerSavingBackup_32176  =  INREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x032176));

    /*
    //------------------------------------------------------------------------------//
    //                              AUSDM POWER DOWN                                //
    //------------------------------------------------------------------------------//
    wriu -b	0x0321F0	0x80	0x80	//	0x0321F0 [7]	PD_R_MICAMP1
    wriu -b	0x0321F0	0x40	0x40	//	0x0321F0 [6]	PD_R_MICAMP0
    wriu -b	0x0321F0	0x20	0x20	//	0x0321F0 [5]	PD_L_MICAMP1
    wriu -b	0x0321F0	0x10	0x10	//	0x0321F0 [4]	PD_L_MICAMP0

    wriu -b	0x0321D9	0x03	0x03	//	0x0321D8 [9:8]	PD_INMUX [3:2]
    wriu -b	0x0321E3	0x03	0x03	//	0x0321E2 [9:8]	PD_INMUX [1:0]
    wriu -b	0x0321DA	0x02	0x02	//	0x0321DA [1]	PD_ADC0
    wriu -b	0x0321DA	0x01	0x01	//	0x0321DA [0]	PD_ADC1

    wriu -b	0x0321DD	0x20	0x20	//	0x0321DC [13]	PD_L0_DAC
    wriu -b	0x0321DD	0x10	0x10	//	0x0321DC [12]	PD_L1_DAC
    wriu -b	0x0321DD	0x02	0x02	//	0x0321DC [9]	PD_R0_DAC
    wriu -b	0x0321DD	0x01	0x01	//	0x0321DC [8]	PD_R1_DAC

    wriu -b	0x0321DC	0x20	0x20	//	0x0321DC [5]	PD_REF_DAC
    wriu -b	0x0321DC	0x10	0x10	//	0x0321DC [4]	PD_BIAS_DAC
    wriu -b	0x0321E1	0x10	0x10	//	0x0321E0 [12]	PD_IBIAS_EAR

    wriu -b	0x0321E0	0x20	0x20	//	0x0321E0 [5]	PD_RT_EAR
    wriu -b	0x0321E0	0x10	0x10	//	0x0321E0 [4]	PD_LT_EAR
    wriu -b	0x0321EA	0x08	0x08	//	0x0321EA [3]	PD_OPLP_EAR

    wriu -b	0x0321E6	0x02	0x02	//	0x0321E6 [1]	PD_MIDTOP
    wriu -b	0x0321E6	0x01	0x01	//	0x0321E6 [0]	PD_VI
    wriu -b	0x0321E9	0x80	0x80	//	0x0321E8 [15]	PD_VREF
    wriu -b	0x0321EC	0x80	0x80	//	0x0321EC [7]	PD_LDO_ADC
    wriu -b	0x0321EC	0x10	0x10	//	0x0321EC [4]	PD_LDO_DAC
    */

    if (is_amic_enabled() == false)
    {
        SETREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x0321F0),0x80);
        SETREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x0321F0),0x40);
        SETREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x0321F0),0x20);
        SETREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x0321F0),0x10);

        SETREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x0321D9),0x03);
        SETREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x0321E3),0x03);
        SETREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x0321DA),0x02);
        SETREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x0321DA),0x01);
    }

    SETREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x0321DD),0x20);
    SETREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x0321DD),0x10);
    SETREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x0321DD),0x02);
    SETREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x0321DD),0x01);

    SETREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x0321DC),0x20);
    SETREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x0321DC),0x10);
    SETREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x0321E1),0x10);

    SETREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x0321E0),0x20);
    SETREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x0321E0),0x10);
    SETREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x0321EA),0x08);

    if (is_amic_enabled() == false)
    {
        SETREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x0321E6),0x02);
        SETREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x0321E6),0x01);
        SETREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x0321E9),0x80);
        SETREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x0321EC),0x80);
    }
    SETREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x0321EC),0x10);


    /*
    //EXTAUPLL
    wriu 0x032176 0x8f
    */

    if (is_amic_enabled() == false)
    {
        OUTREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x032176),0x8f);
    }


}

void Mhal_Power_Saving_0D_Ausdm_On(void)
{
    #if (ENABLE_AUSDM_OFF == 0)
        return;
    #endif

    OUTREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x0321F0),g_u8PowerSavingBackup_321F0);
    OUTREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x0321D9),g_u8PowerSavingBackup_321D9);
    OUTREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x0321E3),g_u8PowerSavingBackup_321E3);
    OUTREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x0321DA),g_u8PowerSavingBackup_321DA);
    OUTREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x0321DD),g_u8PowerSavingBackup_321DD);
    OUTREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x0321DC),g_u8PowerSavingBackup_321DC);
    OUTREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x0321E1),g_u8PowerSavingBackup_321E1);
    OUTREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x0321E0),g_u8PowerSavingBackup_321E0);
    OUTREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x0321EA),g_u8PowerSavingBackup_321EA);
    OUTREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x0321E6),g_u8PowerSavingBackup_321E6);
    OUTREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x0321E9),g_u8PowerSavingBackup_321E9);
    OUTREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x0321EC),g_u8PowerSavingBackup_321EC);
    OUTREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x032176),g_u8PowerSavingBackup_32176);
}
void Mhal_Power_Saving_0C_Haydn_Clock_to_MPLL(void)
{

    if (is_amic_enabled() == false)
        return;

    //Set haydn clock from UPLL to MPLL, for AMIC only.

    if (is_xtal_24mhz()) //24MHz XTAL
    {
        /*
        0x032220 [9:8] = 0x1
        0x032222 [15:0] = 0x1c20
        0x032220 [7:0] = 0x00
        */
        OUTREG16(SCRIPT_ADDR_TO_CPU_ADDR(0x032222),0x1c20);
        OUTREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x032220),0x00);
    }
    else //26MHz XTAL
    {
        /*
        For mpll 442 case:
        0x032220 [9:8] = 0x1
        0x032222 [15:0] = 0x1cc6
        0x032220 [7:0] = 0xaa
        */
        OUTREG16(SCRIPT_ADDR_TO_CPU_ADDR(0x032222),0x1cc6);
        OUTREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x032220),0xaa);
    }

        OUTREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x032221),0xc1);
        OUTREG8(SCRIPT_ADDR_TO_CPU_ADDR(0x032221),0x81);
}

//==============================================================================
void Mhal_Power_Saving_Suspend(void)
{
    if (check_pm_flag(PM_CM4_FLAG_SKIP_POWER_SETTING_ALL))
        return;

    //POWER_SAVING_LOG("Mhal_Power_Saving_Suspend start\n");
    Mhal_Power_Saving_0C_Haydn_Clock_to_MPLL();
    Mhal_Power_Saving_0D_Ausdm_Off();
    Mhal_Power_Saving_0E_Fuart_Off();
    Mhal_Power_Saving_0F_Sys_Sram_Off();
    Mhal_Power_Saving_10_DDR_Atop_Power_Off();
    Mhal_Power_Saving_11_USB_P0_Off(); // before XTAL off
    Mhal_Power_Saving_12_UPLL_MPLL_Off();
    Mhal_Power_Saving_13_MIU128PLL_ARMPLL_Off();
    Mhal_Power_Saving_14_Xtal_Off();
    Mhal_Power_Saving_15_Vlcm_Off();
    //POWER_SAVING_LOG("Mhal_Power_Saving_Suspend end\n");

    #if ENABLE_DEBUG_BY_FORCE_WAKEUP2
    MHal_Timer_Delay(TIMER_DELAY_1s);
    Mhal_PM_Resume();
    #endif
}

//==============================================================================
void Mhal_Power_Saving_Resume (void)
{
    #if ENABLE_CALC_RESUME_TIME
    latch_rtc_timer();
    #endif

    if (check_pm_flag(PM_CM4_FLAG_SKIP_POWER_SETTING_ALL))
        return;

    //POWER_SAVING_LOG("Mhal_Power_Saving_Resume start\n");
    Mhal_Power_Saving_15_Vlcm_On();
    Mhal_Power_Saving_14_Xtal_On();
    Mhal_Power_Saving_13_MIU128PLL_ARMPLL_On();
    Mhal_Power_Saving_12_UPLL_MPLL_On();
    Mhal_Power_Saving_11_USB_P0_On();
    Mhal_Power_Saving_10_DDR_Atop_Power_On();
    Mhal_Power_Saving_0F_Sys_Sram_On();
    Mhal_Power_Saving_0E_Fuart_On();
    Mhal_Power_Saving_0D_Ausdm_Off();
    //POWER_SAVING_LOG("Mhal_Power_Saving_Resume end \n");
}

