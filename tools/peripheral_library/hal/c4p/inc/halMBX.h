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
/// @file   halMBX.h
/// @brief  MStar Mailbox HAL Driver DDI
/// @author MStar Semiconductor Inc.
/// @attention
/// <b><em></em></b>
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _HAL_MBX_H
#define _HAL_MBX_H

#ifdef _HAL_MBX_C
#define INTERFACE
#else
#define INTERFACE extern
#endif

//=============================================================================
// Includs
//=============================================================================

//=============================================================================
// Defines & Macros

//=============================================================================
//busy bit Set/Clear/Get
#define   _BUSY_S(arg)  {\
                            U8 val; \
                            val = REG8_MBX_GROUP(arg, REG8_MBX_STATE_1);\
                            REG8_MBX_GROUP(arg, REG8_MBX_STATE_1) = val | MBX_STATE1_BUSY;\
                         }

#define   _BUSY_C(arg)  {\
                            U8 val; \
                            val = REG8_MBX_GROUP(arg, REG8_MBX_STATE_1);\
                            REG8_MBX_GROUP(arg, REG8_MBX_STATE_1) = val & ~MBX_STATE1_BUSY;\
                         }

#define   _BUSY(arg)    (REG8_MBX_GROUP(arg, REG8_MBX_STATE_1) & MBX_STATE1_BUSY);

//////////////////////////////////////////////////////////////
//error bit Set/Clear/Get
#define   _ERR_S(arg)   {\
                            U8 val;\
                            val = REG8_MBX_GROUP(arg, REG8_MBX_STATE_1);\
                            REG8_MBX_GROUP(arg, REG8_MBX_STATE_1) = val | MBX_STATE1_ERROR;\
                         }

#define   _ERR_C(arg)   {\
                            U8 val;\
                            val = REG8_MBX_GROUP(arg, REG8_MBX_STATE_1);\
                            REG8_MBX_GROUP(arg, REG8_MBX_STATE_1) = val & ~MBX_STATE1_ERROR;\
                         }

#define   _ERR(arg)    (REG8_MBX_GROUP(arg, REG8_MBX_STATE_1) & MBX_STATE1_ERROR)

//////////////////////////////////////////////////////////////
//disabled bit Set/Clear/Get
#define   _DISABLED_S(arg)   {\
                            U8 val;\
                            val = REG8_MBX_GROUP(arg, REG8_MBX_STATE_1);\
                            REG8_MBX_GROUP(arg, REG8_MBX_STATE_1) = val | MBX_STATE1_DISABLED;\
                         }

#define   _DISABLED_C(arg)   {\
                            U8 val;\
                            val = REG8_MBX_GROUP(arg, REG8_MBX_STATE_1);\
                            REG8_MBX_GROUP(arg, REG8_MBX_STATE_1) = val & ~MBX_STATE1_DISABLED;\
                         }

#define   _DISABLED(arg)    (REG8_MBX_GROUP(arg, REG8_MBX_STATE_1) & MBX_STATE1_DISABLED)

////////////////////////////////////////////////////////////////////////
//overflow bit Set/Clear/Get
#define   _OVERFLOW_S(arg)  {\
                                U8 val;\
                                val = REG8_MBX_GROUP(arg, REG8_MBX_STATE_1);\
                                REG8_MBX_GROUP(arg, REG8_MBX_STATE_1) = val | MBX_STATE1_OVERFLOW;\
                              }

#define   _OVERFLOW_C(arg)  {\
                                U8 val;\
                                val = REG8_MBX_GROUP(arg, REG8_MBX_STATE_1);\
                                REG8_MBX_GROUP(arg, REG8_MBX_STATE_1) = val & ~MBX_STATE1_OVERFLOW;\
                              }

#define   _OVERFLOW(arg)   (REG8_MBX_GROUP(arg, REG8_MBX_STATE_1) & MBX_STATE1_OVERFLOW)

////////////////////////////////////////////////////////////////////////
//status bit clear
#define   _S1_C(arg)   {\
                            U8 val;\
                            val = REG8_MBX_GROUP(arg, REG8_MBX_STATE_1);\
                            REG8_MBX_GROUP(arg, REG8_MBX_STATE_1) = val & ~(MBX_STATE1_DISABLED | MBX_STATE1_OVERFLOW | MBX_STATE1_ERROR | MBX_STATE1_BUSY);\
                        }

////////////////////////////////////////////////////////////////////////
//fire bit Set/Clear/Get
#define   _FIRE_S(arg)  {\
                            U8 val;\
                            val = REG8_MBX_GROUP(arg, REG8_MBX_CTRL);\
                            REG8_MBX_GROUP(arg, REG8_MBX_CTRL) = val | MBX_CTRL_FIRE;\
                         }

#define   _FIRE_C(arg)  {\
                            U8 val;\
                            val = REG8_MBX_GROUP(arg, REG8_MBX_CTRL);\
                            REG8_MBX_GROUP(arg, REG8_MBX_CTRL) = val & ~MBX_CTRL_FIRE;\
                         }

#define   _FIRE(arg)   (REG8_MBX_GROUP(arg, REG8_MBX_CTRL) & MBX_CTRL_FIRE)

////////////////////////////////////////////////////////////////////////
//readback bit Set/Clear/Get
#define   _READBACK_S(arg)   {\
                                  U8 val;\
                                  val = REG8_MBX_GROUP(arg, REG8_MBX_CTRL);\
                                  REG8_MBX_GROUP(arg, REG8_MBX_CTRL) = val | MBX_CTRL_READBACK;\
                               }

#define   _READBACK_C(arg)   {\
                                  U8 val;\
                                  val = REG8_MBX_GROUP(arg, REG8_MBX_CTRL);\
                                  REG8_MBX_GROUP(arg, REG8_MBX_CTRL) = val & ~MBX_CTRL_READBACK;\
                               }

#define   _READBACK(arg)   (REG8_MBX_GROUP(arg, REG8_MBX_CTRL) & MBX_CTRL_READBACK)

////////////////////////////////////////////////////////////////////////
//instant bit Set/Clear/Get
#define   _INSTANT_S(arg)   {\
                                  U8 val;\
                                  val = REG8_MBX_GROUP(arg, REG8_MBX_CTRL);\
                                  REG8_MBX_GROUP(arg, REG8_MBX_CTRL) = val | MBX_CTRL_INSTANT;\
                              }

#define   _INSTANT_C(arg)   {\
                                  U8 val;\
                                  val = REG8_MBX_GROUP(arg, REG8_MBX_CTRL);\
                                  REG8_MBX_GROUP(arg, REG8_MBX_CTRL) = val & ~MBX_CTRL_INSTANT;\
                              }

#define   _INSTANT(arg)   (REG8_MBX_GROUP(arg, REG8_MBX_CTRL) & MBX_CTRL_INSTANT)



//////////////////////////////////////////////////////////////
//state0 error bit Set/Clear/Get
#define   _STATE0_ERR_S(arg)   {\
                            U8 val;\
                            val = REG8_MBX_GROUP(arg, REG8_MBX_STATE_0);\
                            REG8_MBX_GROUP(arg, REG8_MBX_STATE_0) = val | MBX_STATE0_ERROR;\
                         }

#define   _STATE0_ERR_C(arg)   {\
                            U8 val;\
                            val = REG8_MBX_GROUP(arg, REG8_MBX_STATE_0);\
                            REG8_MBX_GROUP(arg, REG8_MBX_STATE_0) = val & ~MBX_STATE0_ERROR;\
                         }

#define   _STATE0_ERR(arg)    (REG8_MBX_GROUP(arg, REG8_MBX_STATE_0) & MBX_STATE0_ERROR)

//=============================================================================
// Type and Structure Declaration
//=============================================================================

//=============================================================================
// Enums
/// MBX HAL Recv Status Define
typedef enum
{
    /// Recv Success
    E_MBXHAL_RECV_SUCCESS = 0,
    /// Recv Error: OverFlow
    E_MBXHAL_RECV_OVERFLOW = 1,
    /// Recv Error: Not Enabled
    E_MBXHAL_RECV_DISABLED = 2,
} MBXHAL_Recv_Status;

/// MBX HAL Fire Status Define
typedef enum
{
    /// Fire Success
    E_MBXHAL_FIRE_SUCCESS = 0,
    /// Still Firing
    E_MBXHAL_FIRE_ONGOING = 1,
    /// Fire Error: Overflow:
    E_MBXHAL_FIRE_OVERFLOW = 2,
    /// Fire Error: Not Enabled
    E_MBXHAL_FIRE_DISABLED = 3,
} MBXHAL_Fire_Status;

//=============================================================================
// Mailbox HAL Driver Function
//=============================================================================

INTERFACE MBX_Result MHAL_MBX_ClearAll (MBX_Msg* pMbxMsg, MBX_ROLE_ID eSrcRole);

INTERFACE MBX_Result MHAL_MBX_Init(MBX_ROLE_ID eHostRole);
INTERFACE MBX_Result MHAL_MBX_SetConfig(MBX_ROLE_ID eHostRole);

INTERFACE MBX_Result MHAL_MBX_Fire(MBX_Msg* pMbxMsg, MBX_ROLE_ID eSrcRole);
INTERFACE MBX_Result MHAL_MBX_GetFireStatus(MBX_ROLE_ID eSrcRole, MBX_ROLE_ID eDstRole, MBXHAL_Fire_Status *pFireStatus);

INTERFACE MBX_Result MHAL_MBX_Recv(MBX_Msg* pMbxMsg, MBX_ROLE_ID eDstRole);
INTERFACE MBX_Result MHAL_MBX_RecvEnd(MBX_ROLE_ID eSrcRole, MBX_ROLE_ID eDstRole, MBXHAL_Recv_Status eRecvSatus);
INTERFACE MBX_Result MHAL_MBX_RecvEndExt(MBX_ROLE_ID eSrcRole, MBX_ROLE_ID eDstRole, BOOL bSuccess);

#undef INTERFACE
#endif //_HAL_MBX_H

