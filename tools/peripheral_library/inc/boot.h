/*
 * $Id: //DAILEO/Columbus/IPCamera/source/iNfinity/iNfinity_ROM/source/include/boot.h#2 $
 * $Header: //DAILEO/Columbus/IPCamera/source/iNfinity/iNfinity_ROM/source/include/boot.h#2 $
 * $Date: 2015/06/18 $
 * $DateTime: 2015/06/18 16:04:26 $
 * $Change: 1262174 $
 * $File: //DAILEO/Columbus/IPCamera/source/iNfinity/iNfinity_ROM/source/include/boot.h $
 * $Revision: #2 $
 */

#ifndef BOOT_H
#define BOOT_H
#include <platform.h>
#include <std_c.h>

#define REG(Reg_Addr)         (*(volatile U32*)(Reg_Addr))
#define REGLB(Reg_Addr)       (*(volatile U8*)(Reg_Addr))
#define REGHB(Reg_Addr)       (*(volatile U8*)((Reg_Addr) - 1))

#define _RV1(addr, value)   (((addr) >> 8) & 0x3F), (U8)(addr), (U8)(value)
#define _RV2(addr, value)   0x40 + _RV1(addr, value), (U8)((value) >> 8)
#define _RV3(addr, value)   0x40 + _RV2(addr, value), (U8)((value) >> 16)
#define _RV4(addr, value)   0x40 + _RV3(addr, value), (U8)((value) >> 24)

#define _RV32_1(addr, value)   (U8)(((addr) >> 16) & 0xFF), (U8)(((addr) >> 8) & 0xFF), (U8)(addr), (U8)(value)
#define _RV32_2(addr, value)   0x20 + _RV32_1(addr, value), (U8)((value) >> 8)
#define _RV32_3(addr, value)   0x20 + _RV32_2(addr, value), (U8)((value) >> 16)
#define _RV32_4(addr, value)   0x20 + _RV32_3(addr, value), (U8)((value) >> 24)
#define _END_OF_TBL32_         0xFF, 0xFF, 0xFF

#define _RVM1(addr, value, mask) (((addr) >> 8) & 0x3F), (U8)(addr), (U8)(value), (U8)(mask)
#define _RVM2(addr, value, mask) 0x40 + _RVM1(addr, value, mask), (U8)((value) >> 8), (U8)((mask) >> 8)
#define _RVM3(addr, value, mask) 0x40 + _RVM2(addr, value, mask), (U8)((value) >> 16), (U8)((mask) >> 16)
#define _RVM4(addr, value, mask) 0x40 + _RVM3(addr, value, mask), (U8)((value) >> 24), (U8)((mask) >> 24)
#define _END_OF_TBL_        0xFF, 0xFF


#define READ_REGISTER_ULONG(reg)        (*(volatile unsigned long * const)(reg))
#define WRITE_REGISTER_ULONG(reg, val)  (*(volatile unsigned long * const)(reg)) = (val)
#define READ_REGISTER_USHORT(reg)       (*(volatile unsigned short*)(reg))
#define WRITE_REGISTER_USHORT(reg, val) (*(volatile unsigned short*)(reg)) = (val)
#define READ_REGISTER_UCHAR(reg)        (*(volatile unsigned char * const)(reg))
#define WRITE_REGISTER_UCHAR(reg, val)  (*(volatile unsigned char * const)(reg)) = (val)


#define INREG8(x)           READ_REGISTER_UCHAR(x)
#define OUTREG8(x, y)       WRITE_REGISTER_UCHAR(x, (UCHAR)(y))
#define SETREG8(x, y)       OUTREG8(x, INREG8(x)|(y))
#define CLRREG8(x, y)       OUTREG8(x, INREG8(x)&~(y))
#define INSREG8(addr, mask, val) OUTREG8(addr, ((INREG8(addr)&(~(mask))) | val))
#define EXTREG8(addr, mask, lsh) ((INREG8(addr) & mask) >> lsh)

#define INREG16(x)          READ_REGISTER_USHORT(x)
#define OUTREG16(x, y)      WRITE_REGISTER_USHORT(x,(USHORT)(y))
#define SETREG16(x, y)      OUTREG16(x, INREG16(x)|(y))
#define CLRREG16(x, y)      OUTREG16(x, INREG16(x)&~(y))
#define INSREG16(addr, mask, val) OUTREG16(addr, ((INREG16(addr)&(~(mask))) | val))
#define EXTREG16(addr, mask, lsh) ((INREG16(addr) & mask) >> lsh)

#define INREG32(x)          READ_REGISTER_ULONG(x)
#define OUTREG32(x, y)      WRITE_REGISTER_ULONG(x, (ULONG)(y))
#define SETREG32(x, y)      OUTREG32(x, INREG32(x)|(y))
#define CLRREG32(x, y)      OUTREG32(x, INREG32(x)&~(y))
#define INSREG32(addr, mask, val) OUTREG32(addr, ((INREG32(addr)&(~(mask))) | val))
#define EXTREG32(addr, mask, lsh) ((INREG32(addr) & mask) >> lsh)

//-----------------------------------------------------------------------------
// Error Codes
//-----------------------------------------------------------------------------
#define ERR_SUCCESSFUL              0
#define ERR_UNKNOWN_BOOTTYPE        1 // BootROM
#define ERR_DRAM_FAIL               2 // IPL
#define ERR_NAND_READ_FAIL          4 // BootROM
#define ERR_NO_SDCARD               3 // BootROM
#define ERR_SD_INIT_FAIL            5 // BootROM
#define ERR_FAT_INIT_FAIL           6 // BootROM
#define ERR_FAT_READ_FAIL           7 // BootROM


/*==========================================================================
    Hardware Strapping
===========================================================================*/
#define REG_ADDR_CHIP_CONFIG_STAT       GET_REG16_ADDR(REG_ADDR_BASE_CHIPTOP,0x65)
//BOOT
#define CHIP_CONFIG1_BOOT_SPI_NOJTAG    0   //3'b000
#define CHIP_CONFIG1_BOOT_SPI_JTAG1     1   //3'b001
#define CHIP_CONFIG1_BOOT_SPI_JTAG2     2   //3'b010
#define CHIP_CONFIG1_BOOT_ROM_NOJTAG    4   //3'b100
#define CHIP_CONFIG1_BOOT_ROM_JTAG1     5   //3'b101
#define CHIP_CONFIG1_BOOT_ROM_JTAG2     6   //3'b110

//Storage
#define CHIP_CONFIG2_STORAGE_NAND       0   //2'b00
#define CHIP_CONFIG2_STORAGE_EMMC       1   //2'b01
#define CHIP_CONFIG2_STORAGE_SDCARD     2   //2'b10



//MPLL
#define CHIP_CONFIG3_LPDDR400           0   //3'b000
#define CHIP_CONFIG3_DDR3_1333          1   //3'b001
#define CHIP_CONFIG3_DDR3_1600          2   //3'b010
#define CHIP_CONFIG3_DDR3_1866_8BIT     3   //3'b011



//-----------------------------------------------------------------------------
// SRAM Layout 64K
//-----------------------------------------------------------------------------
/*
+--------------------+
|    0xA0000000+32K  | IPL_RUN_ADDRESS
+--------------------+
|    0xA0008000+8K   | Rom code Data Region
+--------------------+
|    0xA0010000      | Rom code Stacks
+--------------------+
*/
//==================================================
typedef struct image_hdr{
    U32 startaddr;
    U32 length;
    U32 checksum;
    U32 flags_and_offset;
}image_hdr_t;



enum PMU_STATE{
    PMU_NORMAL,
    PMU_SLEEP,
    PMU_STDBY,
};

/*
efuse_outa
bank0[27:26]
    DDR selection
    00: Reserved
    01: MCP DDR is 1st vendor
    10: MCP DDR is 2nd vendor
    11: Use external DDR

bank0[23:21]
    1st vendor MCP DDR Die
    000: Winbond-2CP106256AJ010-DDR2-32MB-1333MHz (MSC314, QFN80 8x8mm)
    001: Winbond-2CP106512AJ010-DDR2-64MB-1333MHz (MSC315v2, QFN80 8x8mm)
    010: Nanya-2CP106001DU000-DDR3-128MB-1866MHz (MSC316v2, QFN88 9x9mm)
    011: Reserved
    100: Nanya-2CP106001EA000-DDR3-128MB-1866MHz (MSC316Dv2, BGA208 11x11mm)
    101: Nanya-2CP106002DA000-DDR3-256MB-1866MHz (MSC316Qv2, BGA208 13x11mm)
    110: Reserved
    111: Reserved

    2nd vendor MCP DDR Die
    000: Reserved
    001: Nanya-2CP106512FJ000-DDR2-64MB (MSC315v2, QFN80 8x8mm)
    010: Samsung-2CP106001GC000-DDR3-128MB-1866MHz (MSC316v2, QFN88 9x9mm)
    011: Reserved
    100: Samsung-2CP106001GA000-DDR3-128MB-1866MHz (MSC316Dv2, BGA208 11x11mm)
    101: Samsung-2CP106002EA000-DDR3-256MB-1866MHz (MSC316Qv2, BGA208 13x11mm)
    110: Reserved
    111: Reserved

*/

typedef enum
{
    E_QFN_DDR2_1333_32MB_W=0,   //Winbond-2CP106256AJ010-DDR2-32MB-1333MHz (MSC314, QFN80 8x8mm)
    E_QFN_DDR2_1333_64MB_W=1,     //Winbond-2CP106512AJ010-DDR2-64MB-1333MHz (MSC315v2, QFN80 8x8mm)
    E_BGA_DDR3_1866_128MB_N=2,    //Nanya-2CP106001EA000-DDR3-128MB-1866MHz (MSC316Dv2, BGA208 11x11mm)
    E_BGA_DDR3_1866_128MB_S=3,    //Samsung-2CP106001GA000-DDR3-128MB-1866MHz (MSC316Dv2, BGA208 11x11mm)
    E_BGA_DDR3_1866_256MB_N=4,    //Nanya-2CP106002DA000-DDR3-256MB-1866MHz (MSC316Qv2, BGA208 13x11mm)
    E_BGA_DDR3_1866_256MB_S=5,    //Samsung-2CP106002EA000-DDR3-256MB-1866MHz (MSC316Qv2, BGA208 13x11mm)
    E_QFN_DDR3_1866_128MB_N=6,    //Nanya-2CP106001DU000-DDR3-128MB-1866MHz (MSC316v2, QFN88 9x9mm)
    E_QFN_DDR3_1866_128MB_S=7,    //Samsung-2CP106001GC000-DDR3-128MB-1866MHz (MSC316v2, QFN88 9x9mm)
    E_QFN_DDR2_1333_64MB_N=8,     //Nanya-2CP106512FJ000-DDR2-64MB (MSC315v2, QFN80 8x8mm)

    E_EXT_DDR2_1333_64MB=0x10,
    E_EXT_DDR2_1066_64MB,
    E_EXT_DDR3_1866_128MB,
    E_EXT_DDR3_1866_256MB,        //Nanya NT5CB256M16DP-FL - 2Gb
    E_EXT_DDR3_1600_512MB_A=0x30, //Nanya NT5CB256M16DP-FLI - 4Gb on EVB
    E_EXT_DDR3_1200_512MB_A,      //Nanya NT5CB256M16DP-FLI - 4Gb on EVB
    E_EXT_DDR3_1866_512MB_A,      //Nanya NT5CB512M16DP-FLI - 4Gb on EVB
    E_EXT_DDR3_1333_512MB_A,      //Nanya NT5CB512M16DP-FLI - 4Gb on EVB
    E_EXT_DDR3_2133_512MB_A,      //Nanya NT5CB512M16DP-FLI - 4Gb on EVB
    E_UNKNOWN = 0xFF
} DDR_TYPE;

typedef struct
{
    unsigned char  magic_word[4];  //'MSBD'
    unsigned char  ddr_type;
    unsigned char  direct_boot;
} BOARD_INFO;

typedef struct
{
    unsigned char  tagPrefix[3];
    unsigned char  headerVersion[1];
    unsigned char  libType[2];
    unsigned char  chip[2];
    unsigned char  changelist[8];
    unsigned char  component[12];
    unsigned char  reserved[1];
    unsigned char  tagSuffix[3];
} MS_VERSION;



#define FLASH_BUF_LEN 512
#define FLASH_ALIGN_LEN 8
#define INVALID_TARGETADDR 0xFFFFFFFF


#define SANITY_CHECK_OK (~ERROR_SANITY_CHECK)


#define NAND_READ_MODE_DMA 0
#define NAND_READ_MODE_RIU 1

#define NAND_BOOT_RETURN  0
#define NAND_BOOT_TERMINATE 1

#define NAND_BOOT_IPL_OFFSET 0
#define NAND_BOOT_SPL_OFFSET 256
#define NAND_BOOT_TPL_OFFSET 512

#define NAND_BOOT_WINCE_OFFSET 768

#define CHECK_SANITY 0x00000002


extern void ram_start(void* ram_start, void* rom_start, U32 length);
extern void LDMCopy(void* ram_start, void* rom_start, U32 length);

#endif
