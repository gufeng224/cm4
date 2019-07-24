/*
 * $Id: //DAILEO/Columbus/IPCamera/source/iNfinity/iNfinity_ROM/source/include/std_c.h#1 $
 * $Header: //DAILEO/Columbus/IPCamera/source/iNfinity/iNfinity_ROM/source/include/std_c.h#1 $
 * $Date: 2015/05/27 $
 * $DateTime: 2015/05/27 17:11:47 $
 * $Change: 1236156 $
 * $File: //DAILEO/Columbus/IPCamera/source/iNfinity/iNfinity_ROM/source/include/std_c.h $
 * $Revision: #1 $
 */


#ifndef	STD_C_H
#define STD_C_H

#if 0
    /// data type unsigned char, data length 1 byte
    typedef unsigned char               MS_U8;                              // 1 byte
    /// data type unsigned short, data length 2 byte
    typedef unsigned short              MS_U16;                             // 2 bytes
    /// data type unsigned int, data length 4 byte
    typedef unsigned long               MS_U32;                             // 4 bytes
    /// data type unsigned int, data length 8 byte
    typedef unsigned long long          MS_U64;                             // 8 bytes
    /// data type signed char, data length 1 byte
    typedef signed char                 MS_S8;                              // 1 byte
    /// data type signed short, data length 2 byte
    typedef signed short                MS_S16;                             // 2 bytes
    /// data type signed int, data length 4 byte
    typedef signed long                 MS_S32;                             // 4 bytes
    /// data type signed int, data length 8 byte
    typedef signed long long            MS_S64;                             // 8 bytes
    /// data type float, data length 4 byte
    typedef float                       MS_FLOAT;                           // 4 bytes
    /// data type pointer content
    typedef MS_U32                      MS_VIRT;                            // 8 bytes
    /// data type hardware physical address
    typedef MS_U32                      MS_PHYADDR;                         // 8 bytes
    /// data type 64bit physical address
    typedef MS_U32                      MS_PHY;                             // 8 bytes
    /// data type size_t
    typedef MS_U32                      MS_SIZE;                            // 8 bytes
    /// definition for MS_BOOL
    typedef unsigned char               MS_BOOL;                            // 1 byte


typedef unsigned int    Primitive;
typedef unsigned char   bool;
typedef unsigned char   u8;
typedef signed   char   s8;
typedef          char   ascii;
typedef unsigned short  u16;
typedef          short  s16;
typedef unsigned int    u32;
typedef          int    s32;
typedef unsigned long long u64;
typedef 	 long long s64;

#endif

typedef int Bool;
#define True 1
#define False 0

    /***
    ****	Interpretation definition
    ***/

#define TRUE	1
#define FALSE	0

#ifdef NULL
    #undef NULL
#endif
#define NULL	0

    /***
    ****	type definition
    ***/
#define U8	unsigned char
#define U16	unsigned short
#define U32 unsigned long
//#define U64 __int64
#define U64 unsigned long long int


#define UCHAR	unsigned char

#ifndef USHORT
#define USHORT	unsigned short
#endif

#ifndef UINT
#define UINT		unsigned long
#endif

#ifndef ULONG
#define ULONG	 __int64
#endif

#define UINT8 unsigned char
#define UINT16 unsigned short
#define UINT32 unsigned long
#define UINT8_PTR UINT8 *
#define UINT16_PTR UINT16 *

#define UINT32_PTR UINT32 *


#define S8  signed char
#define S16 signed short
#define S32 signed long
#define S64 signed long long

#define BOOLEAN unsigned char
#define BOOL unsigned char

/* Macro for TDA1236D		                         							*/
/********************************************************************************/
#define Data8	U8
#define Data16	S16
#define Data32	S32
#define bool	BOOLEAN
#define Bool	BOOLEAN
//#define BOOL    BOOLEAN
#define true 	TRUE
#define false 	FALSE


#define BIT0        0x1
#define BIT1        0x2
#define BIT2        0x4
#define BIT3        0x8
#define BIT4        0x10
#define BIT5        0x20
#define BIT6        0x40
#define BIT7        0x80
#define BIT8        0x100
#define BIT9        0x200
#define BIT10       0x400
#define BIT11       0x800
#define BIT12       0x1000
#define BIT13       0x2000
#define BIT14       0x4000
#define BIT15       0x8000

#ifndef MIN
#define MIN(A, B) (((A) < (B)) ? (A) : (B))
#endif

#ifndef MAX
#define MAX(A, B) (((A) > (B)) ? (A) : (B))
#endif

#ifndef ABS
#define ABS(X)    ((X>=0) ? (X) : (-X))
#endif
#define COUNTOF( array )    (sizeof(array) / sizeof((array)[0]))

/// defination for FALSE
#define FALSE           0
/// defination for TRUE
#define TRUE            1

/// defination for DISABLE
#define DISABLE     	0
/// defination for ENABLE
#define ENABLE      	1

/// 0: FAIL
#define FAIL   			0
/// 1: PASS
#define PASS   			1

/// 0: NO
#define NO     			0
/// 1: YES
#define YES    			1

#define LOWBYTE(u16)    ((U8)(u16))
#define HIGHBYTE(u16)   ((U8)((u16) >> 8))
#define HINIBBLE(u8)    ((u8) / 0x10)
#define LONIBBLE(u8)    ((u8) & 0x0F)
#define BCD2Dec(x)  	((((x) >> 4) * 10) + ((x) & 0x0F))


#define ALIGN_4(_x_)                (((_x_) + 3) & ~3)
#define ALIGN_8(_x_)                (((_x_) + 7) & ~7)
#define ALIGN_16(_x_)               (((_x_) + 15) & ~15)           // No data type specified, optimized by complier
#define ALIGN_32(_x_)               (((_x_) + 31) & ~31)           // No data type specified, optimized by complier

#define MASK(x)     (((1<<(x##_BITS))-1) << x##_SHIFT)
#ifndef BIT	//for Linux_kernel type, BIT redefined in <linux/bitops.h>
#define BIT(_bit_)                  (1 << (_bit_))
#endif
#define BIT_(x)                     BIT(x) //[OBSOLETED] //TODO: remove it later
#define BITS(_bits_, _val_)         ((BIT(((1)?_bits_)+1)-BIT(((0)?_bits_))) & (_val_<<((0)?_bits_)))
#define BMASK(_bits_)               (BIT(((1)?_bits_)+1)-BIT(((0)?_bits_)))

#endif /* STD_C_H  */
