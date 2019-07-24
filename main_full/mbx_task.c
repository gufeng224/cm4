/* Standard includes. */
#include <string.h>
#include <stdio.h>

/* Kernel includes. */
#include "mosWrapper.h"

/* driver includes. */
#include "drvAudio.h"
#include "drvMBX.h"
#include "drvCPU.h"
#include "drvPM.h"
#include "drvBDMA.h"

#include "apiVOC.h"

#include "voc_task.h"

/* Priorities at which the tasks are created. */
#define MBX_TASK_STACK_SIZE 128
#define	MBX_TASK_PRIORITY	( tskIDLE_PRIORITY + 4)

#define MBX_MSG(fmt, args...) MOS_DBG_PRINT(MOS_DBG_LEVEL_CMD, "[MBT] " fmt, ##args)
#define MBX_ERROR(fmt, args...) MOS_DBG_ERROR("[MBT ERR] " fmt, ##args);

StackType_t _xMbxTaskStack[ MBX_TASK_STACK_SIZE ];
StaticTask_t _xMbxTaskBuffer;

#if defined(CONFIG_VOC_TUNE)
static U8 _ps16MbxTaskMsgBuf[1024];
#endif

static void prvMbxProcTask(void *pvParameters)
{
  MBX_Msg tMsg;
  U32 u32Param1;
  U32 u32Param2;
  U16 u16Param3;
  BOOL bRet;

  /* Remove compiler warning about unused parameter. */
  (void) pvParameters;

  MBX_MSG("MbxProcTask Start\r\n");

  while(1)
  {
    bRet = TRUE;
    memset(&tMsg, 0, sizeof(tMsg));
    tMsg.eRoleID = E_MBX_ROLE_HK;
    if(E_MBX_SUCCESS == MDrv_MBX_RecvMsg(&tMsg, TRUE))
    {
      MBX_MSG("Class = %d, Idx = %d, ms = %d\r\n", tMsg.u8MsgClass, tMsg.u8Index, xTaskGetTickCount()*portTICK_PERIOD_MS);

     // MDrv_CPU_SetFreq(E_CPU_FREQ_HIGH);
#if defined(CONFIG_VOC_TUNE)
     if (tMsg.u8MsgClass == E_MBX_CLASS_TUNE)
     {
        if(tMsg.u8ParameterCount == 8)
        {
          MBX_MSG_GET_2U32(tMsg, u32Param1, u32Param2)
          if (u32Param2 > sizeof(_ps16MbxTaskMsgBuf))
          {
            MBX_ERROR("Command size failed\r\n");
            MDrv_MBX_RecvMsgEnd(&tMsg);
            continue;
          }
          MDrv_BDMA_Copy((U32)_ps16MbxTaskMsgBuf, u32Param1, u32Param2, E_BDMA_HKtoCP);

          //////////////////////////////////////////////////////////////////
          switch(tMsg.u8Index)
          {
            case E_MBX_TUNE_MSG_VP_PRAM:
                MApi_VOC_VPTunePort(_ps16MbxTaskMsgBuf, u32Param2);
              break;
            case E_MBX_TUNE_MSG_VAD_PRAM:
                //MApi_VOC_VDConfig(u32Param1, u32Param2);
              break;
            case E_MBX_TUNE_MSG_SRC_PRAM:
                //MApi_VOC_VDConfig(u32Param1, u32Param2);
              break;
            case E_MBX_TUNE_MSG_VQ_PRAM:
                //MApi_VOC_VDConfig(u32Param1, u32Param2);
              break;
            default:
              MBX_ERROR("Command Idx failed\r\n");
              break;
          }
          //////////////////////////////////////////////////////////////////
        }
     }
     else
#endif //CONFIG_VOC_TUNE
     {
       /////////////////////////////////////////////////////////////////////
       switch(tMsg.u8Index)
       {
         //////////////////////////////////////////////////////////////////
         case E_MBX_MSG_VP_ENABLE:
           if(tMsg.u8ParameterCount == 1)
           {
             VocTaskEnableFunc(tMsg.u8Parameters[0], E_FUNC_VP);
           }
           break;
         case E_MBX_MSG_VP_CONFIG:
           if(tMsg.u8ParameterCount == 1)
           {
             Mapi_VOC_VPConfig((U8)tMsg.u8Parameters[0]);
           }
           break;

         //////////////////////////////////////////////////////////////////
         case E_MBX_MSG_VD_ENABLE:
           if(tMsg.u8ParameterCount == 1)
           {
             VocTaskEnableFunc((BOOL)tMsg.u8Parameters[0], E_FUNC_VD);
#if defined(CONFIG_VOC_SEAMLESS)
             if (tMsg.u8Parameters[0])
               VocTaskEnableFunc(FALSE, E_FUNC_VS);
#endif
           }
           break;
         case E_MBX_MSG_VD_CONFIG:
           if(tMsg.u8ParameterCount == 8)
           {
             MBX_MSG_GET_2U32(tMsg, u32Param1, u32Param2)
             MApi_VOC_VDConfig(u32Param1, u32Param2);
           }
           else if(tMsg.u8ParameterCount == 10)
           {
             MBX_MSG_GET_2U32plusU16(tMsg, u32Param1, u32Param2, u16Param3)
             MApi_VOC_VDConfigExt(u32Param1, u32Param2, u16Param3);
           }
           break;

         ///////////////////////////////////////////////////////////////////
         case E_MBX_MSG_VQ_ENABLE:
           if(tMsg.u8ParameterCount == 1)
           {
             VocTaskEnableFunc((BOOL)tMsg.u8Parameters[0], E_FUNC_VQ);
           }
           break;
         case E_MBX_MSG_VQ_CONFIG:
           if(tMsg.u8ParameterCount == 1)
           {
             //MApi_VOC_VQConfig((VQ_Mode_e)tMsg.u8Parameters[0]);
             VocTaskVQConfig((VOC_VQMode_e)tMsg.u8Parameters[0]);
           }
           break;

         ///////////////////////////////////////////////////////////////////
         case E_MBX_MSG_RESET:
           VocTaskReset();
           break;

         ///////////////////////////////////////////////////////////////////
         case E_MBX_MSG_SIGEN_ENABLE:
           if(tMsg.u8ParameterCount == 1)
           {
             MDrv_AUD_SigGenEnable((BOOL)tMsg.u8Parameters[0]);
           }
           break;
         case E_MBX_MSG_MIC_CONFIG:
           if(tMsg.u8ParameterCount == 3)
           {
             bRet = MDrv_AUD_SetMicNo((AUD_MIC_e)tMsg.u8Parameters[0]);
 #if !defined(CONFIG_VOC_SW_SRC) && !defined(CONFIG_VOC_SW_SRC48K)
             MDrv_AUD_SetSampleRate((AUD_Rate_e)tMsg.u8Parameters[1]);
 #endif
             MDrv_AUD_SetBitWidth((AUD_Width_e)tMsg.u8Parameters[2]);
           }
           break;

         case E_MBX_MSG_MIC_GAIN:
           if(tMsg.u8ParameterCount == 1)
           {
 #if defined(CONFIG_AUD_HWGAIN)
             MDrv_AUD_DMICSetGain(tMsg.u8Parameters[0]);
 #else
             MApi_VOC_SetMicSwGain(tMsg.u8Parameters[0]);
 #endif
           }
           break;

 #if defined(CONFIG_VOC_MIC_MAPPING)
         case E_MBX_MSG_MIC_MAP:
           if(tMsg.u8ParameterCount == 2)
           {
             MApi_VOC_MicMapConfig(tMsg.u8Parameters[0], tMsg.u8Parameters[1]);
           }
           break;
 #endif
         ///////////////////////////////////////////////////////////////////
 #if defined(CONFIG_AUD_I2S)
         case E_MBX_MSG_I2S_ENABLE:
           if(tMsg.u8ParameterCount == 1)
           {
             MDrv_AUD_I2SEnable((BOOL)tMsg.u8Parameters[0]);
           }
           break;
         case E_MBX_MSG_I2S_CONFIG:
           if(tMsg.u8ParameterCount == 1)
           {
             //MApi_VOC_VQConfig((VQ_Mode_e)tMsg.u8Parameters[0]);
           }
           break;
 #endif

 #if defined(CONFIG_AUD_AEC)
         case E_MBX_MSG_AEC_ENABLE:
           if(tMsg.u8ParameterCount == 1)
           {
             MDrv_AUD_AECEnable((BOOL)tMsg.u8Parameters[0]);
           }
           break;
 #endif
         //////////////////////////////////////////////////////////////////
 #if defined(CONFIG_VOC_CUS0)
         case E_MBX_MSG_CUS0_ENABLE:
           if(tMsg.u8ParameterCount == 1)
           {
             VocTaskEnableFunc((BOOL)tMsg.u8Parameters[0], E_FUNC_CUS0);
           }
           break;
 #endif

 #if defined(CONFIG_VOC_SWHPF)
         case E_MBX_MSG_HPF_ENABLE:
           if(tMsg.u8ParameterCount == 1)
           {
             MApi_VOC_HpfEnable(tMsg.u8Parameters[0]);
           }
           break;
         case E_MBX_MSG_HPF_CONFIG:
           if(tMsg.u8ParameterCount == 1)
           {
             MApi_VOC_HpfConfig((S8)tMsg.u8Parameters[0]);
           }
           break;
 #endif
         //////////////////////////////////////////////////////////////////
 #if defined(CONFIG_VOC_TEST)
         case E_MBX_MSG_TEST_ENABLE:
           if(tMsg.u8ParameterCount == 1)
           {
             VocTaskEnableFunc((BOOL)tMsg.u8Parameters[0], E_FUNC_TEST);
           }
           break;
         case E_MBX_MSG_TEST_REG:
           if(tMsg.u8ParameterCount == 4)
           {
             MBX_MSG_GET_U32(tMsg, u32Param1);
             VocTaskRegisterCusFunc((VOC_PROC_CUS_FUNC_t)u32Param1);
           }
           break;
 #endif
 #if defined(CONFIG_PM_GPIO)
         case E_MBX_MSG_PM_GPIO:
           if(tMsg.u8ParameterCount == 3)
             MDrv_PM_GPIO_OEN(tMsg.u8Parameters[1], tMsg.u8Parameters[0], tMsg.u8Parameters[2]);
           break;
 #endif
         case E_MBX_MSG_SLEEP:
           if(tMsg.u8ParameterCount == 1)
           {
             if (tMsg.u8Parameters[0])
               MDrv_PM_Suspend();
             else
             {
               MDrv_PM_Resume();
               VocTaskReset();
             }
           }
           break;
 #if defined(CONFIG_AUD_AMIC)
         case E_MBX_MSG_AMIC_ENABLE:
           if(tMsg.u8ParameterCount == 1)
           {
             if (tMsg.u8Parameters[0])
             {
                MDrv_AUD_AMicOn();
             }
             else
             {
                MDrv_AUD_AMicOff();
             }
           }

           break;

         case E_MBX_MSG_AMIC_CONFIG:
           if(tMsg.u8ParameterCount == 2)
           {
             if(tMsg.u8Parameters[0]!=0xFF)
               MDrv_AUD_AMicMicAmpGain(tMsg.u8Parameters[0]);
             if(tMsg.u8Parameters[1]!=0xFF)
               MDrv_AUD_AMicAdcGain(tMsg.u8Parameters[1]);
           }
           break;
 #endif
         case E_MBX_MSG_SW_GAIN:
           if(tMsg.u8ParameterCount == 1)
           {
             MApi_VOC_SetSwGain(tMsg.u8Parameters[0]);
           }
           break;
#if defined(CONFIG_VOC_SW_VAD)
         case E_MBX_MSG_VAD_GAIN:
           if(tMsg.u8ParameterCount == 1)
           {
             MApi_VOC_SetVADGain(tMsg.u8Parameters[0]);
           }
           break;
 #endif
         default:
           MBX_ERROR("Command Idx failed\r\n");
           break;
       } //switch
      /////////////////////////////////////////////////////////////////////
     }
     MDrv_MBX_RecvMsgEndExt(&tMsg, bRet);
    }
    else
    {
      MBX_ERROR("MDrv_MBX_RecvMsg received failed\r\n");
    }

  } //while(1)
}



void MbxTaskStart(void)
{
  /* Create that task that handles the console itself. */
  xTaskCreateStatic(prvMbxProcTask, "MBX", MBX_TASK_STACK_SIZE, NULL, MBX_TASK_PRIORITY, _xMbxTaskStack, &_xMbxTaskBuffer);
}

/*-----------------------------------------------------------*/

