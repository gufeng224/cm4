/* Standard includes. */
#include <string.h>
#include <stdlib.h>

/* Kernel includes. */
#include "mosWrapper.h"

/* FreeRTOS+CLI includes. */
#include "FreeRTOS_CLI.h"

//#include "cleveland.h"
#include "drvAudio.h"
#include "drvBDMA.h"
#include "drvMBX.h"
#include "drvCPU.h"
#include "drvPM.h"

#include "halCPUINT.h"
#include "halPM.h"
#include "halRTC.h"
#include "halTimer.h"
#include "halUART.h"

#include "apiVOC.h"
#include "voc_task.h"
#include "mbx_task.h"

//#define __AUDIO_TEST_BDMA__
//#define __AUDIO_TEST_REC__
//#define __AUDIO_TEST_BDMA_IMITOMIU__
//#define __AUDIO_TEST_CYBER_VQ__
//#define __AUDIO_TEST_CYBER_VQ2__
//#define __AUDIO_TEST_NUANCE_VQ__
//#define __AUDIO_TEST_PM__
//#define __AUDIO_TEST_SYS__
#define __AUDIO_TEST_VAD__
//#define __AUDIO_TEST_HPF__

#define CLI_MSG(fmt, args...) MOS_DBG_PRINT(MOS_DBG_LEVEL_CMD, "[CLI] " fmt, ##args)
#define CLI_ERROR(fmt, args...) MOS_DBG_ERROR("[CLI] " fmt, ##args);

/*-----------------------------------------------------------*/

#if defined(__AUDIO_TEST_REC__) || defined(__AUDIO_TEST_BDMA__)
__attribute__((section(".dma_buffer_64"))) __attribute__((aligned(64))) static U8 _gMcuDmaBuf3[AUDIO_DMA_BUF_SIZE*10];
#endif

#if 0
static U8 temp_tone_48k[192 * 2];
static U8 temp_tone_48k_mono[192];

const U8 tone_48k_mono[192] =
{
  0x00, 0x00, 0x5F, 0x08, 0x9B, 0x10, 0x8D, 0x18,
  0x13, 0x20, 0x0D, 0x27, 0x5C, 0x2D, 0xE5, 0x32,
  0x8F, 0x37, 0x44, 0x3B, 0xF7, 0x3D, 0x9A, 0x3F,
  0x27, 0x40, 0x9A, 0x3F, 0xF6, 0x3D, 0x45, 0x3B,
  0x8F, 0x37, 0xE5, 0x32, 0x5C, 0x2D, 0x0E, 0x27,
  0x13, 0x20, 0x8C, 0x18, 0x9A, 0x10, 0x60, 0x08,
  0x00, 0x00, 0xA1, 0xF7, 0x66, 0xEF, 0x73, 0xE7,
  0xED, 0xDF, 0xF3, 0xD8, 0xA3, 0xD2, 0x1B, 0xCD,
  0x71, 0xC8, 0xBC, 0xC4, 0x09, 0xC2, 0x66, 0xC0,
  0xD9, 0xBF, 0x66, 0xC0, 0x09, 0xC2, 0xBB, 0xC4,
  0x72, 0xC8, 0x1B, 0xCD, 0xA4, 0xD2, 0xF2, 0xD8,
  0xEC, 0xDF, 0x73, 0xE7, 0x66, 0xEF, 0xA1, 0xF7,
  0x00, 0x00, 0x5F, 0x08, 0x9B, 0x10, 0x8D, 0x18,
  0x13, 0x20, 0x0D, 0x27, 0x5C, 0x2D, 0xE5, 0x32,
  0x8F, 0x37, 0x44, 0x3B, 0xF7, 0x3D, 0x9A, 0x3F,
  0x27, 0x40, 0x9A, 0x3F, 0xF6, 0x3D, 0x45, 0x3B,
  0x8F, 0x37, 0xE5, 0x32, 0x5C, 0x2D, 0x0E, 0x27,
  0x13, 0x20, 0x8C, 0x18, 0x9A, 0x10, 0x60, 0x08,
  0x00, 0x00, 0xA1, 0xF7, 0x66, 0xEF, 0x73, 0xE7,
  0xED, 0xDF, 0xF3, 0xD8, 0xA3, 0xD2, 0x1B, 0xCD,
  0x71, 0xC8, 0xBC, 0xC4, 0x09, 0xC2, 0x66, 0xC0,
  0xD9, 0xBF, 0x66, 0xC0, 0x09, 0xC2, 0xBB, 0xC4,
  0x72, 0xC8, 0x1B, 0xCD, 0xA4, 0xD2, 0xF2, 0xD8,
  0xEC, 0xDF, 0x73, 0xE7, 0x66, 0xEF, 0xA1, 0xF7
};



const U8 tone_48k[192 * 2] =
{
  0x00, 0x00, 0x00, 0x00, 0x5F, 0x08, 0x60, 0x08, 0x9B, 0x10, 0x9A, 0x10, 0x8D, 0x18, 0x8C, 0x18,
  0x13, 0x20, 0x13, 0x20, 0x0D, 0x27, 0x0D, 0x27, 0x5C, 0x2D, 0x5D, 0x2D, 0xE5, 0x32, 0xE4, 0x32,
  0x8F, 0x37, 0x8E, 0x37, 0x44, 0x3B, 0x44, 0x3B, 0xF7, 0x3D, 0xF7, 0x3D, 0x9A, 0x3F, 0x99, 0x3F,
  0x27, 0x40, 0x26, 0x40, 0x9A, 0x3F, 0x99, 0x3F, 0xF6, 0x3D, 0xF7, 0x3D, 0x45, 0x3B, 0x44, 0x3B,
  0x8F, 0x37, 0x8E, 0x37, 0xE5, 0x32, 0xE5, 0x32, 0x5C, 0x2D, 0x5D, 0x2D, 0x0E, 0x27, 0x0D, 0x27,
  0x13, 0x20, 0x13, 0x20, 0x8C, 0x18, 0x8D, 0x18, 0x9A, 0x10, 0x9B, 0x10, 0x60, 0x08, 0x60, 0x08,
  0x00, 0x00, 0x00, 0x00, 0xA1, 0xF7, 0xA1, 0xF7, 0x66, 0xEF, 0x65, 0xEF, 0x73, 0xE7, 0x73, 0xE7,
  0xED, 0xDF, 0xEC, 0xDF, 0xF3, 0xD8, 0xF4, 0xD8, 0xA3, 0xD2, 0xA3, 0xD2, 0x1B, 0xCD, 0x1B, 0xCD,
  0x71, 0xC8, 0x72, 0xC8, 0xBC, 0xC4, 0xBC, 0xC4, 0x09, 0xC2, 0x09, 0xC2, 0x66, 0xC0, 0x66, 0xC0,
  0xD9, 0xBF, 0xDA, 0xBF, 0x66, 0xC0, 0x66, 0xC0, 0x09, 0xC2, 0x09, 0xC2, 0xBB, 0xC4, 0xBC, 0xC4,
  0x72, 0xC8, 0x71, 0xC8, 0x1B, 0xCD, 0x1C, 0xCD, 0xA4, 0xD2, 0xA3, 0xD2, 0xF2, 0xD8, 0xF3, 0xD8,
  0xEC, 0xDF, 0xED, 0xDF, 0x73, 0xE7, 0x74, 0xE7, 0x66, 0xEF, 0x66, 0xEF, 0xA1, 0xF7, 0xA1, 0xF7,
  0x00, 0x00, 0x00, 0x00, 0x5F, 0x08, 0x60, 0x08, 0x9B, 0x10, 0x9A, 0x10, 0x8D, 0x18, 0x8C, 0x18,
  0x13, 0x20, 0x13, 0x20, 0x0D, 0x27, 0x0D, 0x27, 0x5C, 0x2D, 0x5D, 0x2D, 0xE5, 0x32, 0xE4, 0x32,
  0x8F, 0x37, 0x8E, 0x37, 0x44, 0x3B, 0x44, 0x3B, 0xF7, 0x3D, 0xF7, 0x3D, 0x9A, 0x3F, 0x99, 0x3F,
  0x27, 0x40, 0x26, 0x40, 0x9A, 0x3F, 0x99, 0x3F, 0xF6, 0x3D, 0xF7, 0x3D, 0x45, 0x3B, 0x44, 0x3B,
  0x8F, 0x37, 0x8E, 0x37, 0xE5, 0x32, 0xE5, 0x32, 0x5C, 0x2D, 0x5D, 0x2D, 0x0E, 0x27, 0x0D, 0x27,
  0x13, 0x20, 0x13, 0x20, 0x8C, 0x18, 0x8D, 0x18, 0x9A, 0x10, 0x9B, 0x10, 0x60, 0x08, 0x60, 0x08,
  0x00, 0x00, 0x00, 0x00, 0xA1, 0xF7, 0xA1, 0xF7, 0x66, 0xEF, 0x65, 0xEF, 0x73, 0xE7, 0x73, 0xE7,
  0xED, 0xDF, 0xEC, 0xDF, 0xF3, 0xD8, 0xF4, 0xD8, 0xA3, 0xD2, 0xA3, 0xD2, 0x1B, 0xCD, 0x1B, 0xCD,
  0x71, 0xC8, 0x72, 0xC8, 0xBC, 0xC4, 0xBC, 0xC4, 0x09, 0xC2, 0x09, 0xC2, 0x66, 0xC0, 0x66, 0xC0,
  0xD9, 0xBF, 0xDA, 0xBF, 0x66, 0xC0, 0x66, 0xC0, 0x09, 0xC2, 0x09, 0xC2, 0xBB, 0xC4, 0xBC, 0xC4,
  0x72, 0xC8, 0x71, 0xC8, 0x1B, 0xCD, 0x1C, 0xCD, 0xA4, 0xD2, 0xA3, 0xD2, 0xF2, 0xD8, 0xF3, 0xD8,
  0xEC, 0xDF, 0xED, 0xDF, 0x73, 0xE7, 0x74, 0xE7, 0x66, 0xEF, 0x66, 0xEF, 0xA1, 0xF7, 0xA1, 0xF7
};

/* left - 1k, right - 3k */
U8 tone_48k_2[192] =
{
  0x00, 0x00, 0x00, 0x00, 0x5F, 0x08, 0x8D, 0x18, 0x9B, 0x10, 0x5C, 0x2D, 0x8D, 0x18, 0x44, 0x3B,
  0x13, 0x20, 0x27, 0x40, 0x0D, 0x27, 0x44, 0x3B, 0x5C, 0x2D, 0x5C, 0x2D, 0xE5, 0x32, 0x8C, 0x18,
  0x8F, 0x37, 0x00, 0x00, 0x44, 0x3B, 0x74, 0xE7, 0xF7, 0x3D, 0xA3, 0xD2, 0x9A, 0x3F, 0xBC, 0xC4,
  0x27, 0x40, 0xD9, 0xBF, 0x9A, 0x3F, 0xBC, 0xC4, 0xF6, 0x3D, 0xA4, 0xD2, 0x45, 0x3B, 0x74, 0xE7,
  0x8F, 0x37, 0x01, 0x00, 0xE5, 0x32, 0x8D, 0x18, 0x5C, 0x2D, 0x5D, 0x2D, 0x0E, 0x27, 0x44, 0x3B,
  0x13, 0x20, 0x26, 0x40, 0x8C, 0x18, 0x44, 0x3B, 0x9A, 0x10, 0x5C, 0x2D, 0x60, 0x08, 0x8D, 0x18,
  0x00, 0x00, 0x00, 0x00, 0xA1, 0xF7, 0x74, 0xE7, 0x66, 0xEF, 0xA4, 0xD2, 0x73, 0xE7, 0xBC, 0xC4,
  0xED, 0xDF, 0xD9, 0xBF, 0xF3, 0xD8, 0xBC, 0xC4, 0xA3, 0xD2, 0xA4, 0xD2, 0x1B, 0xCD, 0x74, 0xE7,
  0x71, 0xC8, 0xFF, 0xFF, 0xBC, 0xC4, 0x8D, 0x18, 0x09, 0xC2, 0x5D, 0x2D, 0x66, 0xC0, 0x45, 0x3B,
  0xD9, 0xBF, 0x26, 0x40, 0x66, 0xC0, 0x44, 0x3B, 0x09, 0xC2, 0x5C, 0x2D, 0xBB, 0xC4, 0x8D, 0x18,
  0x72, 0xC8, 0x00, 0x00, 0x1B, 0xCD, 0x74, 0xE7, 0xA4, 0xD2, 0xA4, 0xD2, 0xF2, 0xD8, 0xBC, 0xC4,
  0xEC, 0xDF, 0xDA, 0xBF, 0x73, 0xE7, 0xBB, 0xC4, 0x66, 0xEF, 0xA3, 0xD2, 0xA1, 0xF7, 0x73, 0xE7
};


/* left - 1k, right - 3k */
U8 tone_48k_3[96] =
{
  0x00, 0x00, 0x5F, 0x08, 0x9B, 0x10, 0x8D, 0x18, 0x13, 0x20, 0x0D, 0x27, 0x5C, 0x2D, 0xE5, 0x32,
  0x8F, 0x37, 0x44, 0x3B, 0xF7, 0x3D, 0x9A, 0x3F, 0x27, 0x40, 0x9A, 0x3F, 0xF6, 0x3D, 0x45, 0x3B,
  0x8F, 0x37, 0xE5, 0x32, 0x5C, 0x2D, 0x0E, 0x27,	0x13, 0x20, 0x8C, 0x18, 0x9A, 0x10, 0x60, 0x08,
  0x00, 0x00, 0xA1, 0xF7, 0x66, 0xEF, 0x73, 0xE7,	0xED, 0xDF, 0xF3, 0xD8, 0xA3, 0xD2, 0x1B, 0xCD,
  0x71, 0xC8, 0xBC, 0xC4, 0x09, 0xC2, 0x66, 0xC0,	0xD9, 0xBF, 0x66, 0xC0, 0x09, 0xC2, 0xBB, 0xC4,
  0x72, 0xC8, 0x1B, 0xCD, 0xA4, 0xD2, 0xF2, 0xD8,	0xEC, 0xDF, 0x73, 0xE7, 0x66, 0xEF, 0xA1, 0xF7
};

U8 tone_48k_4[96] =
{
  0x00, 0x00, 0x8D, 0x18, 0x5C, 0x2D, 0x44, 0x3B,	0x27, 0x40, 0x44, 0x3B, 0x5C, 0x2D, 0x8C, 0x18,
  0x00, 0x00, 0x74, 0xE7, 0xA3, 0xD2, 0xBC, 0xC4,	0xD9, 0xBF, 0xBC, 0xC4, 0xA4, 0xD2, 0x74, 0xE7,
  0x01, 0x00, 0x8D, 0x18, 0x5D, 0x2D, 0x44, 0x3B,	0x26, 0x40, 0x44, 0x3B, 0x5C, 0x2D, 0x8D, 0x18,
  0x00, 0x00, 0x74, 0xE7, 0xA4, 0xD2, 0xBC, 0xC4,	0xD9, 0xBF, 0xBC, 0xC4, 0xA4, 0xD2, 0x74, 0xE7,
  0xFF, 0xFF, 0x8D, 0x18, 0x5D, 0x2D, 0x45, 0x3B,	0x26, 0x40, 0x44, 0x3B, 0x5C, 0x2D, 0x8D, 0x18,
  0x00, 0x00, 0x74, 0xE7, 0xA4, 0xD2, 0xBC, 0xC4,	0xDA, 0xBF, 0xBB, 0xC4, 0xA3, 0xD2, 0x73, 0xE7
};
#endif

#if defined(__AUDIO_TEST_CYBER_VQ__)
INCBIN(voice, "test2.pcm");
#elif defined(__AUDIO_TEST_NUANCE_VQ__)
INCBIN(voice, "../../3rd/Nuance/hello_dragon_fangxu.pcm.bin");
#endif

#if defined(__AUDIO_TEST_CYBER_VQ__) || defined(__AUDIO_TEST_NUANCE_VQ__)
static BaseType_t prxTestVQ( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
  BOOL bRet;

  char *buffer_start = &incbin_voice_start;
  char *buffer_end = &incbin_voice_end;
  int audioBytes;

  ( void ) pcWriteBuffer;
	( void ) pcCommandString;
	( void ) xWriteBufferLen;

  CLI_MSG("prxTestVQ: buffer size 0x%x, t = %d\r\n", (U32)(buffer_end - buffer_start), (MHal_Timer_GetTick()/TIMER_DELAY_1ms));
  for (audioBytes = 0; audioBytes < (buffer_end - buffer_start); audioBytes += 640)
  {
    bRet =MApi_VOC_VQPort((S16 *)(buffer_start + audioBytes), 320);
    if(bRet)
    {
      CLI_MSG("prxTestVQ: VQ process success, audioBytes = %d\r\n", audioBytes);
    }
  }

  return pdFALSE;;
}

static const CLI_Command_Definition_t xCliTestVQ =
{
	"audio-vq-self", /* The command string to type. */
	"\r\naudio-vq-self:\r\n  self VQ\r\n",
	prxTestVQ, /* The function to run. */
	0 /* No parameters are expected. */
};
#endif


#if defined(__AUDIO_TEST_CYBER_VQ2__)
extern int nResponseTime;
static BaseType_t prvCyberTimeTestCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
const char *pcParameter;
BaseType_t xParameterStringLength, xReturn;
static UBaseType_t uxParameterNumber = 0;

long val;

	( void ) pcCommandString;
	( void ) xWriteBufferLen;
	configASSERT( pcWriteBuffer );

	if( uxParameterNumber == 0 )
	{
		uxParameterNumber = 1U;
		xReturn = pdPASS;
	}
	else
	{
		/* Obtain the parameter string. */
		pcParameter = FreeRTOS_CLIGetParameter
						(
							pcCommandString,		/* The command string itself. */
							uxParameterNumber,		/* Return the next parameter. */
							&xParameterStringLength	/* Store the parameter string length. */
						);

		/* Sanity check something was returned. */
		configASSERT( pcParameter );

		/* Return the parameter string. */
		memset( pcWriteBuffer, 0x00, xWriteBufferLen );
    val = strtol(pcParameter, NULL, 10);

	 if( uxParameterNumber == 1 )
	 {

     nResponseTime = val;
		 xReturn = pdFALSE;
		 uxParameterNumber = 0;
   }
 	 else
	 {
			/* There are more parameters to return after this one. */
			xReturn = pdTRUE;
			uxParameterNumber++;
	 }
	}

	return xReturn;
}

static const CLI_Command_Definition_t xCliCyberTime =
{
	"cyber-time", /* The command string to type. */
	"\r\ncyber-time:\r\n Set Cyber Time\r\n",
	prvCyberTimeTestCommand, /* The function to run. */
	1 /* No parameters are expected. */
};
#endif

/*-----------------------------------------------------------*/
#if defined(__AUDIO_TEST_BDMA_IMITOMIU__)
BaseType_t xAudioBDMATestIMIToMIU(short *buffer, uint32_t count)
{
  U8 *pMiuPtr = 0;
  U32 nMiuSize = sizeof(_gMcuDmaBuf3);
  static U8 *ptr = 0;

  int size = count*2;

  if (ptr == pMiuPtr)
    CLI_MSG("prvAudioTest: copy DMA data size %d to 0x%x\r\n", size, ptr);

  if((U32)ptr + size > (U32)pMiuPtr + nMiuSize)
  {
    //memcpy(ptr, buffer, _gMcuDmaBuf3 + sizeof(_gMcuDmaBuf3) - ptr);
    MDrv_BDMA_Copy((U32)buffer, (U32)ptr, (U32)(pMiuPtr + nMiuSize - ptr), E_BDMA_CPtoHK);
    //_chip_flush_miu_pipe();
    ptr = pMiuPtr + nMiuSize;
  }
  else
  {
    //memcpy(ptr, buffer, size);
    MDrv_BDMA_Copy((U32)buffer, (U32)ptr, (U32)size, E_BDMA_CPtoHK);
    ptr += size;
  }

  if((U32)ptr >= (U32)pMiuPtr + nMiuSize)
  {
    CLI_MSG("prvAudioTest finished 0x%x\r\n", ptr);
    ptr = pMiuPtr;
    return pdPASS;
  }

  return pdFAIL;
}


static BaseType_t prvAudioBDMATestIMIToMIUCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
  ( void ) pcWriteBuffer;
	( void ) pcCommandString;
	( void ) xWriteBufferLen;

  VocTaskRegisterCusFunc(xAudioBDMATestIMIToMIU);
  VocTaskEnableFunc(TRUE, E_FUNC_CUS);
  return pdFALSE;;
}

static const CLI_Command_Definition_t xCliTestBDMAIMIToMIU =
{
	"audio-bdma-miu", /* The command string to type. */
	"\r\naudio-bdma-miu:\r\n Move PCM data by BDMA\r\n",
	prvAudioBDMATestIMIToMIUCommand, /* The function to run. */
	0 /* No parameters are expected. */
};
#endif

#if defined(__AUDIO_TEST_BDMA__)
BaseType_t xAudioBDMATest(short *buffer, uint32_t count)
{
  static U8 *ptr = _gMcuDmaBuf3;
  int size = count*2;

  if (ptr == _gMcuDmaBuf3)
    CLI_MSG("prvAudioTest: copy DMA data size %d to 0x%x\r\n", size, ptr);

  if((U32)ptr + size > (U32)_gMcuDmaBuf3 + sizeof(_gMcuDmaBuf3))
  {
    MDrv_BDMA_Copy((U32)buffer, (U32)ptr, (U32)(_gMcuDmaBuf3 + sizeof(_gMcuDmaBuf3) - ptr), E_BDMA_CPtoCP);
    //_chip_flush_miu_pipe();
    ptr = _gMcuDmaBuf3 + sizeof(_gMcuDmaBuf3);
  }
  else
  {
    MDrv_BDMA_Copy((U32)buffer, (U32)ptr, (U32)size, E_BDMA_CPtoCP);
    ptr += size;
  }

  if((U32)ptr >= (U32)_gMcuDmaBuf3 + sizeof(_gMcuDmaBuf3))
  {
    CLI_MSG("prvAudioTest finished 0x%x\r\n", ptr);
    ptr = _gMcuDmaBuf3;
    return pdPASS;
  }

  return pdFAIL;
}

#if 0
static BaseType_t prvAudioBDMATestCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
  ( void ) pcWriteBuffer;
	( void ) pcCommandString;
	( void ) xWriteBufferLen;

  vVocProcRegisterCusFunc(xAudioBDMATest);
  vVocProcStartCus();
  return pdFALSE;;
}
#else


static BaseType_t prvAudioBDMATestCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
const char *pcParameter;
BaseType_t xParameterStringLength, xReturn;
static UBaseType_t uxParameterNumber = 0;

MBX_Msg tMsg;
long val;

	( void ) pcCommandString;
	( void ) xWriteBufferLen;
	configASSERT( pcWriteBuffer );

	if( uxParameterNumber == 0 )
	{
		uxParameterNumber = 1U;
		xReturn = pdPASS;
	}
	else
	{
		/* Obtain the parameter string. */
		pcParameter = FreeRTOS_CLIGetParameter
						(
							pcCommandString,		/* The command string itself. */
							uxParameterNumber,		/* Return the next parameter. */
							&xParameterStringLength	/* Store the parameter string length. */
						);

		/* Sanity check something was returned. */
		configASSERT( pcParameter );

		/* Return the parameter string. */
		memset( pcWriteBuffer, 0x00, xWriteBufferLen );
    val = strtol(pcParameter, NULL, 10);
    //uxParameterNumber++;

		if( uxParameterNumber == 1U )
		{
      if (val)
      {
        CLI_MSG("prvAudioBDMATestCommand\r\n");
        VocTaskRegisterCusFunc(xAudioBDMATest);
        VocTaskEnableFunc(TRUE, E_FUNC_CUS);
      }
      else
      {
        CLI_MSG("prvAudioBDMATestCommand by MBX\r\n");
        tMsg.eRoleID = E_MBX_ROLE_CP;
        tMsg.eMsgType = E_MBX_MSG_TYPE_NORMAL;
        tMsg.u8MsgClass = E_MBX_CLASS_VOC;
        tMsg.u8Index  = E_MBX_MSG_TEST_REG;
        MBX_MSG_PUT_U32(tMsg, (U32)xAudioBDMATest);
        MDrv_MBX_SendMsgExt(&tMsg, E_MBX_ROLE_HK);

        tMsg.u8Index  = E_MBX_MSG_TEST_ENABLE;
        tMsg.u8ParameterCount = 1;
        tMsg.u8Parameters[0] = TRUE;
        MDrv_MBX_SendMsgExt(&tMsg, E_MBX_ROLE_HK);
      }

			xReturn = pdFALSE;
			uxParameterNumber = 0;
		}
 		else
		{
			xReturn = pdTRUE;
			uxParameterNumber++;
		}
	}

	return xReturn;
}
#endif

static const CLI_Command_Definition_t xCliTestBDMA =
{
	"audio-bdma", /* The command string to type. */
	"\r\naudio-bdma:\r\n Move PCM data by BDMA\r\n",
	prvAudioBDMATestCommand, /* The function to run. */
	1 /* No parameters are expected. */
};

#endif

#if defined(__AUDIO_TEST_REC__)
BaseType_t xAudioRecTest(short *buffer, uint32_t count)
{

  static U8 *ptr = _gMcuDmaBuf3;
  int size = count*2;

  static U32 ms = 0, ms2 = 0;
  U32 rtccount;
  U16 rtcsubcnt;

  if (ptr == _gMcuDmaBuf3)
  {
    ms2 =  xTaskGetTickCount()*portTICK_PERIOD_MS;
    ms = MHal_Timer_GetTick()/TIMER_DELAY_1ms;
    MHal_RTC_GetTime(&rtccount, &rtcsubcnt);
    CLI_MSG("prvAudioTest: rtc =  0x%x (%d)\r\n", rtccount, rtcsubcnt*1000/0x8000);
    CLI_MSG("prvAudioTest: copy DMA data size %d to 0x%x\r\n", size, ptr);

  }
  if((U32)ptr + size > (U32)_gMcuDmaBuf3 + sizeof(_gMcuDmaBuf3))
  {
    memcpy(ptr, buffer, _gMcuDmaBuf3 + sizeof(_gMcuDmaBuf3) - ptr);
    //_chip_flush_miu_pipe();
    ptr = _gMcuDmaBuf3 + sizeof(_gMcuDmaBuf3);
  }
  else
  {
    memcpy(ptr, buffer, size);
    //printf("ptr : %x, pBuf : %x\n",ptr,pBuf[0]);
    //AUD_DEBUG_MSG("data %x %x %x %x\n", buffer[0], buffer[1], buffer[2], buffer[3]);
    //AUD_DEBUG_MSG("ptr %x %x %x %x\n",  ptr[0], ptr[1], ptr[2], ptr[3]);
    ptr += size;
  }

  if((U32)ptr >= (U32)_gMcuDmaBuf3 + sizeof(_gMcuDmaBuf3))
  {
    CLI_MSG("prvAudioTest finished addr 0x%x\r\n", ptr);
    CLI_MSG("prvAudioTest time = %d (systick), %d\r\n", ((xTaskGetTickCount()*portTICK_PERIOD_MS) - ms2), ((MHal_Timer_GetTick()/TIMER_DELAY_1ms)- ms));
    MHal_RTC_GetTime(&rtccount, &rtcsubcnt);
    CLI_MSG("prvAudioTest: rtc =  0x%x (%d)\r\n", rtccount, rtcsubcnt*1000/0x8000);
    ptr = _gMcuDmaBuf3;
    return pdPASS;
  }

  return pdFAIL;
}

#if 0
static BaseType_t prvAudioRecTestCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
  ( void ) pcWriteBuffer;
	( void ) pcCommandString;
	( void ) xWriteBufferLen;
  vVocProcRegisterCusFunc(xAudioRecTest);
  vVocProcStartCus();
  return pdFALSE;;
}
#else

static BaseType_t prvAudioRecTestCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
const char *pcParameter;
BaseType_t xParameterStringLength, xReturn;
static UBaseType_t uxParameterNumber = 0;

MBX_Msg tMsg;
long val;

	( void ) pcCommandString;
	( void ) xWriteBufferLen;
	configASSERT( pcWriteBuffer );

	if( uxParameterNumber == 0 )
	{
		uxParameterNumber = 1U;
		xReturn = pdPASS;
	}
	else
	{
		/* Obtain the parameter string. */
		pcParameter = FreeRTOS_CLIGetParameter
						(
							pcCommandString,		/* The command string itself. */
							uxParameterNumber,		/* Return the next parameter. */
							&xParameterStringLength	/* Store the parameter string length. */
						);

		/* Sanity check something was returned. */
		configASSERT( pcParameter );

		/* Return the parameter string. */
		memset( pcWriteBuffer, 0x00, xWriteBufferLen );
    val = strtol(pcParameter, NULL, 10);
    //uxParameterNumber++;

		if( uxParameterNumber == 1U )
		{
      if (val)
      {
        CLI_MSG("prvAudioRecTestCommand\r\n");
        VocTaskRegisterCusFunc(xAudioRecTest);
        VocTaskEnableFunc(TRUE, E_FUNC_TEST);
      }
      else
      {
        CLI_MSG("prvAudioRecTestCommand by MBX\r\n");
        tMsg.eRoleID = E_MBX_ROLE_CP;
        tMsg.eMsgType = E_MBX_MSG_TYPE_NORMAL;
        tMsg.u8MsgClass = E_MBX_CLASS_VOC;
        tMsg.u8Index  = E_MBX_MSG_TEST_REG;
        MBX_MSG_PUT_U32(tMsg, (U32)xAudioRecTest);
        MDrv_MBX_SendMsgExt(&tMsg, E_MBX_ROLE_HK);

        tMsg.u8Index  = E_MBX_MSG_TEST_ENABLE;
        tMsg.u8ParameterCount = 1;
        tMsg.u8Parameters[0] = TRUE;
        MDrv_MBX_SendMsgExt(&tMsg, E_MBX_ROLE_HK);
      }

			xReturn = pdFALSE;
			uxParameterNumber = 0;
		}
 		else
		{
			xReturn = pdTRUE;
			uxParameterNumber++;
		}
	}

	return xReturn;
}
#endif

static const CLI_Command_Definition_t xCliTestRec =
{
	"audio-rec", /* The command string to type. */
	"\r\naudio-rec:\r\n  Move PCM data by memcpy\r\n",
	prvAudioRecTestCommand, /* The function to run. */
	1 /* No parameters are expected. */
};
#endif


#if defined(__AUDIO_TEST_BDMA__)
void INTC_Handler_CPToHK0_IRQn(void)
{
MBX_Msg tMsg;
U32 count;
  MHal_CPUINT_Clear( E_CPUINT_HK0 ,E_CPUINT_CP);

  tMsg.eRoleID = E_MBX_ROLE_CP;
  MHAL_MBX_Recv(&tMsg,  E_MBX_ROLE_HK);
  MBX_MSG_GET_U32(tMsg, count);

  UartSendTrace("INTC_Handler_CPToHK0_IRQn....%d\n", count);
}

static BaseType_t prvAudioBDMAIntTestCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{

MBX_Msg tMsg;
U32 u32Param1;
U32 u32Param2;

  ( void ) pcWriteBuffer;
	( void ) pcCommandString;
	( void ) xWriteBufferLen;

  MOS_IRQn_AttachIsr(INTC_Handler_CPToHK0_IRQn, CPToHK0_IRQn, 0);
  MOS_IRQn_Unmask(CPToHK0_IRQn);

  CLI_MSG("prvAudioBDMAIntTestCommand by MBX\r\n");
  tMsg.eRoleID = E_MBX_ROLE_CP;
  tMsg.eMsgType = E_MBX_MSG_TYPE_NORMAL;
  tMsg.u8MsgClass = E_MBX_CLASS_VOC;
  tMsg.u8Index  = E_MBX_MSG_VD_CONFIG;
  MBX_MSG_PUT_2U32(tMsg, (U32)_gMcuDmaBuf3, (U32)sizeof(_gMcuDmaBuf3));
  MDrv_MBX_SendMsgExt(&tMsg, E_MBX_ROLE_HK);

  tMsg.u8Index  = E_MBX_MSG_VD_ENABLE;
  tMsg.u8ParameterCount = 1;
  tMsg.u8Parameters[0] = TRUE;
  MDrv_MBX_SendMsgExt(&tMsg, E_MBX_ROLE_HK);
  return pdFALSE;;
}

static const CLI_Command_Definition_t xCliTestBDMAInt =
{
	"audio-bdma-int", /* The command string to type. */
	"\r\naudio-bdma-int:\r\n  Move PCM data by memcpy\r\n",
	prvAudioBDMAIntTestCommand, /* The function to run. */
	0 /* No parameters are expected. */
};
#endif

#if defined(CONFIG_AUD_I2S)
static BaseType_t prvEnableI2SCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
const char *pcParameter;
BaseType_t xParameterStringLength, xReturn;
static UBaseType_t uxParameterNumber = 0;

MBX_Msg tMsg;
long val;

	( void ) pcCommandString;
	( void ) xWriteBufferLen;
	configASSERT( pcWriteBuffer );

	if( uxParameterNumber == 0 )
	{
		uxParameterNumber = 1U;
		xReturn = pdPASS;
	}
	else
	{
		/* Obtain the parameter string. */
		pcParameter = FreeRTOS_CLIGetParameter
						(
							pcCommandString,		/* The command string itself. */
							uxParameterNumber,		/* Return the next parameter. */
							&xParameterStringLength	/* Store the parameter string length. */
						);

		/* Sanity check something was returned. */
		configASSERT( pcParameter );

		/* Return the parameter string. */
		memset( pcWriteBuffer, 0x00, xWriteBufferLen );
    val = strtol(pcParameter, NULL, 10);
    //uxParameterNumber++;

		if( uxParameterNumber == 1U )
		{
      if (!val)
      {
        CLI_MSG("prvEnableI2SCommand\r\n");
        MDrv_AUD_I2SEnable(TRUE);
      }
      else
      {
        CLI_MSG("prvEnableI2SCommand by MBX\r\n");
        tMsg.eRoleID = E_MBX_ROLE_CP;
        tMsg.eMsgType = E_MBX_MSG_TYPE_NORMAL;
        tMsg.u8MsgClass = E_MBX_CLASS_VOC;
        tMsg.u8Index  = E_MBX_MSG_I2S_ENABLE;
        tMsg.u8ParameterCount = 1;
        tMsg.u8Parameters[0] = TRUE;
        MDrv_MBX_SendMsgExt(&tMsg, E_MBX_ROLE_HK);
      }

			xReturn = pdFALSE;
			uxParameterNumber = 0;
		}
 		else
		{
			xReturn = pdTRUE;
			uxParameterNumber++;
		}
	}

	return xReturn;
}

static const CLI_Command_Definition_t xCliEnableI2S =
{
	"i2s-enable", /* The command string to type. */
	"\r\ni2s-enable:\r\n  Enable I2S\r\n",
	prvEnableI2SCommand, /* The function to run. */
	1 /* No parameters are expected. */
};

static BaseType_t prvDisableI2Sommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
  ( void ) pcWriteBuffer;
	( void ) pcCommandString;
	( void ) xWriteBufferLen;

  MDrv_AUD_I2SEnable(FALSE);
  return pdFALSE;;
}

static const CLI_Command_Definition_t xCliDisableI2S =
{
	"i2s-disable", /* The command string to type. */
	"\r\ni2s-disable:\r\n  Disable I2S\r\n",
	prvDisableI2Sommand, /* The function to run. */
	0 /* No parameters are expected. */
};

#endif


#if defined(CONFIG_AUD_AEC)
static BaseType_t prvEnableAECCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
const char *pcParameter;
BaseType_t xParameterStringLength, xReturn;
static UBaseType_t uxParameterNumber = 0;

MBX_Msg tMsg;
long val;

	( void ) pcCommandString;
	( void ) xWriteBufferLen;
	configASSERT( pcWriteBuffer );

	if( uxParameterNumber == 0 )
	{
		uxParameterNumber = 1U;
		xReturn = pdPASS;
	}
	else
	{
		/* Obtain the parameter string. */
		pcParameter = FreeRTOS_CLIGetParameter
						(
							pcCommandString,		/* The command string itself. */
							uxParameterNumber,		/* Return the next parameter. */
							&xParameterStringLength	/* Store the parameter string length. */
						);

		/* Sanity check something was returned. */
		configASSERT( pcParameter );

		/* Return the parameter string. */
		memset( pcWriteBuffer, 0x00, xWriteBufferLen );
    val = strtol(pcParameter, NULL, 10);
    //uxParameterNumber++;

		if( uxParameterNumber == 1U )
		{
      if (!val)
      {
        CLI_MSG("prvEnableAECCommand\r\n");
        MDrv_AUD_AECEnable(TRUE);
      }
      else
      {
        CLI_MSG("prvEnableAECCommand by MBX\r\n");
        tMsg.eRoleID = E_MBX_ROLE_CP;
        tMsg.eMsgType = E_MBX_MSG_TYPE_NORMAL;
        tMsg.u8MsgClass = E_MBX_CLASS_VOC;
        tMsg.u8Index  = E_MBX_MSG_AEC_ENABLE;
        tMsg.u8ParameterCount = 1;
        tMsg.u8Parameters[0] = TRUE;
        MDrv_MBX_SendMsgExt(&tMsg, E_MBX_ROLE_HK);
      }

			xReturn = pdFALSE;
			uxParameterNumber = 0;
		}
 		else
		{
			xReturn = pdTRUE;
			uxParameterNumber++;
		}
	}

	return xReturn;
}

static const CLI_Command_Definition_t xCliEnableAEC =
{
	"aec-enable", /* The command string to type. */
	"\r\ni2s-enable:\r\n  Enable HW AEC\r\n",
	prvEnableAECCommand, /* The function to run. */
	1 /* No parameters are expected. */
};

static BaseType_t prvDisableAECommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
  ( void ) pcWriteBuffer;
	( void ) pcCommandString;
	( void ) xWriteBufferLen;

  MDrv_AUD_AECEnable(FALSE);
  return pdFALSE;;
}

static const CLI_Command_Definition_t xCliDisableAEC =
{
	"aec-disable", /* The command string to type. */
	"\r\naec-disable:\r\n  Disable HW AEC\r\n",
	prvDisableAECommand, /* The function to run. */
	0 /* No parameters are expected. */
};

#endif

/*
static BaseType_t prvAudioStartVQCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
  ( void ) pcWriteBuffer;
	( void ) pcCommandString;
	( void ) xWriteBufferLen;

  VocTaskEnableFunc(TRUE, E_FUNC_VP);
  VocTaskEnableFunc(TRUE, E_FUNC_VQ);
  return pdFALSE;;
}
*/

static BaseType_t prvAudioStartVQCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
const char *pcParameter;
BaseType_t xParameterStringLength, xReturn;
static UBaseType_t uxParameterNumber = 0;

MBX_Msg tMsg;
long val;

	( void ) pcCommandString;
	( void ) xWriteBufferLen;
	configASSERT( pcWriteBuffer );

	if( uxParameterNumber == 0 )
	{
		uxParameterNumber = 1U;
		xReturn = pdPASS;
	}
	else
	{
		/* Obtain the parameter string. */
		pcParameter = FreeRTOS_CLIGetParameter
						(
							pcCommandString,		/* The command string itself. */
							uxParameterNumber,		/* Return the next parameter. */
							&xParameterStringLength	/* Store the parameter string length. */
						);

		/* Sanity check something was returned. */
		configASSERT( pcParameter );

		/* Return the parameter string. */
		memset( pcWriteBuffer, 0x00, xWriteBufferLen );
    val = strtol(pcParameter, NULL, 10);
    //uxParameterNumber++;

		if( uxParameterNumber == 1U )
		{
      if (!val)
      {
        CLI_MSG("prvAudioStartVQCommand\r\n");
        VocTaskEnableFunc(TRUE, E_FUNC_VP);
        //MApi_VOC_VQConfig(E_VQ_MODE_PM);
        VocTaskVQConfig(E_VQ_MODE_PM);
        VocTaskEnableFunc(TRUE, E_FUNC_VQ);
      }
      else
      {
        CLI_MSG("prvAudioStartVQCommand (LOOP) by MBX\r\n");
        tMsg.eRoleID = E_MBX_ROLE_CP;
        tMsg.eMsgType = E_MBX_MSG_TYPE_NORMAL;
        tMsg.u8MsgClass = E_MBX_CLASS_VOC;

        tMsg.u8Index  = E_MBX_MSG_VP_ENABLE;
        tMsg.u8ParameterCount = 1;
        tMsg.u8Parameters[0] = TRUE;
        MDrv_MBX_SendMsgExt(&tMsg, E_MBX_ROLE_HK);

        tMsg.u8Index  = E_MBX_MSG_VQ_CONFIG;
        tMsg.u8ParameterCount = 1;
        tMsg.u8Parameters[0] = E_MBX_VQ_MODE_LOOP;
        MDrv_MBX_SendMsgExt(&tMsg, E_MBX_ROLE_HK);

        tMsg.u8Index  = E_MBX_MSG_VQ_ENABLE;
        tMsg.u8ParameterCount = 1;
        tMsg.u8Parameters[0] = TRUE;
        MDrv_MBX_SendMsgExt(&tMsg, E_MBX_ROLE_HK);
      }

			xReturn = pdFALSE;
			uxParameterNumber = 0;
		}
 		else
		{
			xReturn = pdTRUE;
			uxParameterNumber++;
		}
	}

	return xReturn;
}

static const CLI_Command_Definition_t xCliStartVQ =
{
	"vq-start", /* The command string to type. */
	"\r\nvq-start:\r\n  Start VQ\r\n",
	prvAudioStartVQCommand, /* The function to run. */
	1 /* No parameters are expected. */
};

static BaseType_t prvAudioStopVQCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
  ( void ) pcWriteBuffer;
	( void ) pcCommandString;
	( void ) xWriteBufferLen;

  VocTaskEnableFunc(FALSE, E_FUNC_VQ);
  VocTaskEnableFunc(FALSE, E_FUNC_VP);
  return pdFALSE;;
}

static const CLI_Command_Definition_t xCliStopVQ =
{
	"vq-stop", /* The command string to type. */
	"\r\nvq-stop:\r\n  Stop VQ\r\n",
	prvAudioStopVQCommand, /* The function to run. */
	0 /* No parameters are expected. */
};


static BaseType_t prvVPStartTestCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
  ( void ) pcWriteBuffer;
	( void ) pcCommandString;
	( void ) xWriteBufferLen;

  VocTaskEnableFunc(TRUE, E_FUNC_VP_TEST);
  CLI_MSG("prvVPStartTestCommand\r\n");
  return pdFALSE;;
}
static const CLI_Command_Definition_t xCliStartVP =
{
	"vp-start", /* The command string to type. */
	"\r\nvp-start:\r\n Start VP\r\n",
	prvVPStartTestCommand, /* The function to run. */
	0 /* No parameters are expected. */
};

static BaseType_t prvVPStopTestCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
  ( void ) pcWriteBuffer;
	( void ) pcCommandString;
	( void ) xWriteBufferLen;

  VocTaskEnableFunc(FALSE, E_FUNC_VP_TEST);
  CLI_MSG("prvVPStopTestCommand\r\n");
  return pdFALSE;;
}


static const CLI_Command_Definition_t xCliStopVP =
{
	"vp-stop", /* The command string to type. */
	"\r\nvp-stop:\r\n Stop VP\r\n",
	prvVPStopTestCommand, /* The function to run. */
	0 /* No parameters are expected. */
};


static BaseType_t prvAudioStopCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
  ( void ) pcWriteBuffer;
	( void ) pcCommandString;
	( void ) xWriteBufferLen;

  VocTaskReset();
  return pdFALSE;;
}

static const CLI_Command_Definition_t xCliStop =
{
	"audio-stop", /* The command string to type. */
	"\r\naudio-stop:\r\n  Stop\r\n",
	prvAudioStopCommand, /* The function to run. */
	0 /* No parameters are expected. */
};


static BaseType_t prvAudioConfigureCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
const char *pcParameter;
BaseType_t xParameterStringLength, xReturn;
static UBaseType_t uxParameterNumber = 0;

static Bool bEn;
static AUD_MIC_e eMicNo = AUD_MIC_NO2;
static AUD_Rate_e eRate = AUD_RATE_16K;


long ret;

	/* Remove compile time warnings about unused parameters, and check the
	write buffer is not NULL.  NOTE - for simplicity, this example assumes the
	write buffer length is adequate, so does not check for buffer overflows. */
	( void ) pcCommandString;
	( void ) xWriteBufferLen;
	configASSERT( pcWriteBuffer );

	if( uxParameterNumber == 0 )
	{
		/* The first time the function is called after the command has been
		entered just a header string is returned. */
		//sprintf( pcWriteBuffer, "The three parameters were:\r\n" );

		/* Next time the function is called the first parameter will be echoed
		back. */
		uxParameterNumber = 1U;

		/* There is more data to be returned as no parameters have been echoed
		back yet. */
		xReturn = pdPASS;
	}
	else
	{
		/* Obtain the parameter string. */
		pcParameter = FreeRTOS_CLIGetParameter
						(
							pcCommandString,		/* The command string itself. */
							uxParameterNumber,		/* Return the next parameter. */
							&xParameterStringLength	/* Store the parameter string length. */
						);

		/* Sanity check something was returned. */
		configASSERT( pcParameter );

		/* Return the parameter string. */
		memset( pcWriteBuffer, 0x00, xWriteBufferLen );
		//sprintf( pcWriteBuffer, "%d: ", ( int ) uxParameterNumber );
		//strncat( pcWriteBuffer, pcParameter, ( size_t ) xParameterStringLength );
		//strncat( pcWriteBuffer, "\r\n", strlen( "\r\n" ) );

    ret = strtol(pcParameter, NULL, 10);

    CLI_MSG("prvAudioConfigureCommand %d\r\n", ret);

		/* If this is the last of the three parameters then there are no more
		strings to return after this one. */
		if( uxParameterNumber == 3U )
		{
			/* If this is the last of the three parameters then there are no more
			strings to return after this one. */
			bEn = (Bool)ret;

			CLI_MSG("MDrv_AUD_Config %d %d %d\r\n", eMicNo, eRate, bEn);
			//MDrv_AUD_Config(eChn, nMode, bEn);
			MDrv_AUD_SetMicNo(eMicNo);
			CLI_MSG("MDrv_AUD_SetMicNo %d\r\n", eMicNo);
			MDrv_AUD_SetSampleRate(eRate);
			CLI_MSG("MDrv_AUD_SetSampleRate %d\r\n", eRate);
			MDrv_AUD_SigGenEnable(bEn);
			CLI_MSG("MDrv_AUD_SigGenEnable %d\r\n", bEn);

			xReturn = pdFALSE;
			uxParameterNumber = 0;
		}
    else
    if( uxParameterNumber == 2U )
		{
			/* If this is the last of the three parameters then there are no more
			strings to return after this one. */
			eRate = (AUD_Rate_e)ret;
			xReturn = pdTRUE;
			uxParameterNumber++;
		}
    else
    if( uxParameterNumber == 1U )
		{
			/* If this is the last of the three parameters then there are no more
			strings to return after this one. */
			eMicNo = (AUD_MIC_e)ret;
			xReturn = pdTRUE;
			uxParameterNumber++;
		}
		else
		{
			/* There are more parameters to return after this one. */
			xReturn = pdTRUE;
			uxParameterNumber++;
		}
	}

	return xReturn;
}

static const CLI_Command_Definition_t xCliDmicConfig =
{
	"audio-config", /* The command string to type. */
	"\r\naudio-config:\r\n  Config Audio\r\n",
	prvAudioConfigureCommand, /* The function to run. */
	3 /* No parameters are expected. */
};

static BaseType_t prvAudioConfigureGainCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
const char *pcParameter;
BaseType_t xParameterStringLength, xReturn;
static UBaseType_t uxParameterNumber = 0;

long val;

	( void ) pcCommandString;
	( void ) xWriteBufferLen;
	configASSERT( pcWriteBuffer );

	if( uxParameterNumber == 0 )
	{
		uxParameterNumber = 1U;
		xReturn = pdPASS;
	}
	else
	{
		/* Obtain the parameter string. */
		pcParameter = FreeRTOS_CLIGetParameter
						(
							pcCommandString,		/* The command string itself. */
							uxParameterNumber,		/* Return the next parameter. */
							&xParameterStringLength	/* Store the parameter string length. */
						);

		/* Sanity check something was returned. */
		configASSERT( pcParameter );

		/* Return the parameter string. */
		memset( pcWriteBuffer, 0x00, xWriteBufferLen );
    val = strtol(pcParameter, NULL, 10);
    //uxParameterNumber++;

		if( uxParameterNumber == 1U )
		{
#if defined(CONFIG_AUD_HWGAIN)
      MDrv_AUD_DMICSetGain((U8)val);
      CLI_MSG("MDrv_AUD_DMICSetGain %d\r\n", val);
#else
      MApi_VOC_SetMicSwGain((U8)val);
      CLI_MSG("MApi_VOC_SetMicSwGain %d\r\n", val);
#endif
			xReturn = pdFALSE;
			uxParameterNumber = 0;
		}
 		else
		{
			xReturn = pdTRUE;
			uxParameterNumber++;
		}
	}

	return xReturn;
}

static const CLI_Command_Definition_t xCliDmicGain =
{
	"audio-gain", /* The command string to type. */
	"\r\naudio-gain:\r\n  Config Audio Gain\r\n",
	prvAudioConfigureGainCommand, /* The function to run. */
	1 /* No parameters are expected. */
};


static BaseType_t prvAudioConfigureWidthCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
const char *pcParameter;
BaseType_t xParameterStringLength, xReturn;
static UBaseType_t uxParameterNumber = 0;

long val;

	( void ) pcCommandString;
	( void ) xWriteBufferLen;
	configASSERT( pcWriteBuffer );

	if( uxParameterNumber == 0 )
	{
		uxParameterNumber = 1U;
		xReturn = pdPASS;
	}
	else
	{
		/* Obtain the parameter string. */
		pcParameter = FreeRTOS_CLIGetParameter
						(
							pcCommandString,		/* The command string itself. */
							uxParameterNumber,		/* Return the next parameter. */
							&xParameterStringLength	/* Store the parameter string length. */
						);

		/* Sanity check something was returned. */
		configASSERT( pcParameter );

		/* Return the parameter string. */
		memset( pcWriteBuffer, 0x00, xWriteBufferLen );
		val = strtol(pcParameter, NULL, 10);
		//uxParameterNumber++;

		if( uxParameterNumber == 1U )
		{
			//MDrv_AUD_DMAConfig(AUD_DMA_CHN_MAX, 17, val);
			MDrv_AUD_SetBitWidth((AUD_Width_e) val);
			CLI_MSG("MDrv_AUD_SetBitWidth %d\r\n", val);

			xReturn = pdFALSE;
			uxParameterNumber = 0;
		}
 		else
		{
			xReturn = pdTRUE;
			uxParameterNumber++;
		}
	}

	return xReturn;
}

static const CLI_Command_Definition_t xCliDmaWidth =
{
	"audio-width", /* The command string to type. */
	"\r\naudio-width:\r\n  Config Audio Width\r\n",
	prvAudioConfigureWidthCommand, /* The function to run. */
	1 /* No parameters are expected. */
};


#if defined(CONFIG_VOC_MIC_MAPPING)
static BaseType_t prvAudioConfigureMapCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
const char *pcParameter;
BaseType_t xParameterStringLength, xReturn;
static UBaseType_t uxParameterNumber = 0;

long ret;

	/* Remove compile time warnings about unused parameters, and check the
	write buffer is not NULL.  NOTE - for simplicity, this example assumes the
	write buffer length is adequate, so does not check for buffer overflows. */
	( void ) pcCommandString;
	( void ) xWriteBufferLen;
	configASSERT( pcWriteBuffer );

	if( uxParameterNumber == 0 )
	{
		/* The first time the function is called after the command has been
		entered just a header string is returned. */
		//sprintf( pcWriteBuffer, "The three parameters were:\r\n" );

		/* Next time the function is called the first parameter will be echoed
		back. */
		uxParameterNumber = 1U;

		/* There is more data to be returned as no parameters have been echoed
		back yet. */
		xReturn = pdPASS;
	}
	else
	{
		/* Obtain the parameter string. */
		pcParameter = FreeRTOS_CLIGetParameter
						(
							pcCommandString,		/* The command string itself. */
							uxParameterNumber,		/* Return the next parameter. */
							&xParameterStringLength	/* Store the parameter string length. */
						);

		/* Sanity check something was returned. */
		configASSERT( pcParameter );

		/* Return the parameter string. */
		memset( pcWriteBuffer, 0x00, xWriteBufferLen );
		//sprintf( pcWriteBuffer, "%d: ", ( int ) uxParameterNumber );
		//strncat( pcWriteBuffer, pcParameter, ( size_t ) xParameterStringLength );
		//strncat( pcWriteBuffer, "\r\n", strlen( "\r\n" ) );

    ret = strtol(pcParameter, NULL, 10);

		/* If this is the last of the three parameters then there are no more
		strings to return after this one. */
		if( uxParameterNumber == 4U )
		{
			MApi_VOC_MicMapConfig(3, ret);
			xReturn = pdFALSE;
			uxParameterNumber = 0;
		}
    else
		if( uxParameterNumber == 3U )
		{
			MApi_VOC_MicMapConfig(2, ret);
			xReturn = pdTRUE;
			uxParameterNumber++;
		}
    else
    if( uxParameterNumber == 2U )
		{
			MApi_VOC_MicMapConfig(1, ret);
			xReturn = pdTRUE;
			uxParameterNumber++;
		}
    else
    if( uxParameterNumber == 1U )
		{
      MApi_VOC_MicMapConfig(0, ret);
			xReturn = pdTRUE;
			uxParameterNumber++;
		}
		else
		{
			/* There are more parameters to return after this one. */
			xReturn = pdTRUE;
			uxParameterNumber++;
		}
	}

	return xReturn;
}

static const CLI_Command_Definition_t xCliMicMapConfig =
{
	"audio-map", /* The command string to type. */
	"\r\naudio-map:\r\n  Config Mic Mapping\r\n",
	prvAudioConfigureMapCommand, /* The function to run. */
	4 /* No parameters are expected. */
};
#endif

#if defined(__AUDIO_TEST_SYS__)
static BaseType_t prvSwInterruptCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
const char *pcParameter;
BaseType_t xParameterStringLength, xReturn;
static UBaseType_t uxParameterNumber = 0;

long reg;
long val;

void INTC_Handler_HK0ToCP_IRQn(void)
{
  MHal_CPUINT_Clear( E_CPUINT_CP ,E_CPUINT_HK0);
  CLI_MSG("HK0ToCP_IRQn....\r\n");
}
void INTC_Handler_HK1ToCP_IRQn(void)
{
  MHal_CPUINT_Clear( E_CPUINT_CP ,E_CPUINT_HK1);
  CLI_MSG("HK1ToCP_IRQn....\r\n");
}

	/* Remove compile time warnings about unused parameters, and check the
	write buffer is not NULL.  NOTE - for simplicity, this example assumes the
	write buffer length is adequate, so does not check for buffer overflows. */
	( void ) pcCommandString;
	( void ) xWriteBufferLen;
	configASSERT( pcWriteBuffer );

	if( uxParameterNumber == 0 )
	{
		/* The first time the function is called after the command has been
		entered just a header string is returned. */
		//sprintf( pcWriteBuffer, "The three parameters were:\r\n" );

		/* Next time the function is called the first parameter will be echoed
		back. */
		uxParameterNumber = 1U;

		/* There is more data to be returned as no parameters have been echoed
		back yet. */
		xReturn = pdPASS;
	}
	else
	{
		/* Obtain the parameter string. */
		pcParameter = FreeRTOS_CLIGetParameter
						(
							pcCommandString,		/* The command string itself. */
							uxParameterNumber,		/* Return the next parameter. */
							&xParameterStringLength	/* Store the parameter string length. */
						);

		/* Sanity check something was returned. */
		configASSERT( pcParameter );

		/* Return the parameter string. */
		memset( pcWriteBuffer, 0x00, xWriteBufferLen );
		//sprintf( pcWriteBuffer, "%d: ", ( int ) uxParameterNumber );
		//strncat( pcWriteBuffer, pcParameter, ( size_t ) xParameterStringLength );
		//strncat( pcWriteBuffer, "\r\n", strlen( "\r\n" ) );

    reg = strtol(pcParameter, NULL, 10);
    uxParameterNumber++;

		pcParameter = FreeRTOS_CLIGetParameter
						(
							pcCommandString,		/* The command string itself. */
							uxParameterNumber,		/* Return the next parameter. */
							&xParameterStringLength	/* Store the parameter string length. */
						);

		/* Sanity check something was returned. */
		configASSERT( pcParameter );

    val = strtol(pcParameter, NULL, 10);

		/* If this is the last of the three parameters then there are no more
		strings to return after this one. */
		if( uxParameterNumber == 2U )
		{
      MOS_IRQn_AttachIsr(INTC_Handler_HK0ToCP_IRQn, HK0ToCP_IRQn, 0);
      MOS_IRQn_AttachIsr(INTC_Handler_HK1ToCP_IRQn, HK1ToCP_IRQn, 0);
      MOS_IRQn_Unmask(HK0ToCP_IRQn);
      MOS_IRQn_Unmask(HK1ToCP_IRQn);

      CLI_MSG("prvSwInterruptCommand Host%d to Host%d\r\n", reg, val);
			/* If this is the last of the three parameters then there are no more
			strings to return after this one. */
      MHal_CPUINT_Fire (val, reg);

			xReturn = pdFALSE;
			uxParameterNumber = 0;
		}
 		else
		{
			/* There are more parameters to return after this one. */
			xReturn = pdTRUE;
			uxParameterNumber++;
		}
	}

	return xReturn;
}

static const CLI_Command_Definition_t xCliInt =
{
	"sw-int", /* The command string to type. */
	"\r\nsw-int arg1(1,2,3) arg2(0/1):\r\n  SW Interrupt\r\n",
	prvSwInterruptCommand, /* The function to run. */
	2 /* No parameters are expected. */
};

static BaseType_t prvPMGpioInCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
const char *pcParameter;
BaseType_t xParameterStringLength, xReturn;
static UBaseType_t uxParameterNumber = 0;

long val;

	( void ) pcCommandString;
	( void ) xWriteBufferLen;
	configASSERT( pcWriteBuffer );

	if( uxParameterNumber == 0 )
	{
		uxParameterNumber = 1U;
		xReturn = pdPASS;
	}
	else
	{
		/* Obtain the parameter string. */
		pcParameter = FreeRTOS_CLIGetParameter
						(
							pcCommandString,		/* The command string itself. */
							uxParameterNumber,		/* Return the next parameter. */
							&xParameterStringLength	/* Store the parameter string length. */
						);

		/* Sanity check something was returned. */
		configASSERT( pcParameter );

		/* Return the parameter string. */
		memset( pcWriteBuffer, 0x00, xWriteBufferLen );
    val = strtol(pcParameter, NULL, 10);

	 if( uxParameterNumber == 1 )
	 {
     //MDrv_AUD_VadEnable(TRUE, (U8)val);
     MDrv_PM_GPIO_OEN((PM_GPIO_e)val, 0, 0);

		 xReturn = pdFALSE;
		 uxParameterNumber = 0;
   }
 	 else
	 {
			/* There are more parameters to return after this one. */
			xReturn = pdTRUE;
			uxParameterNumber++;
	 }
	}

	return xReturn;
}

static const CLI_Command_Definition_t xCliPMGpioIn =
{
	"pm-gpio-in", /* The command string to type. */
	"\r\npm-gpio-in:\r\n Test PM GPIO In\r\n",
	prvPMGpioInCommand, /* The function to run. */
	1 /* No parameters are expected. */
};


static BaseType_t prvPMGpioOutCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
const char *pcParameter;
BaseType_t xParameterStringLength, xReturn;
static UBaseType_t uxParameterNumber = 0;

long val;

	( void ) pcCommandString;
	( void ) xWriteBufferLen;
	configASSERT( pcWriteBuffer );

	if( uxParameterNumber == 0 )
	{
		uxParameterNumber = 1U;
		xReturn = pdPASS;
	}
	else
	{
		/* Obtain the parameter string. */
		pcParameter = FreeRTOS_CLIGetParameter
						(
							pcCommandString,		/* The command string itself. */
							uxParameterNumber,		/* Return the next parameter. */
							&xParameterStringLength	/* Store the parameter string length. */
						);

		/* Sanity check something was returned. */
		configASSERT( pcParameter );

		/* Return the parameter string. */
		memset( pcWriteBuffer, 0x00, xWriteBufferLen );
    val = strtol(pcParameter, NULL, 10);

	 if( uxParameterNumber == 1 )
	 {
     //MDrv_AUD_VadEnable(TRUE, (U8)val);
     MDrv_PM_GPIO_OEN((PM_GPIO_e)val, 1, 1);

		 xReturn = pdFALSE;
		 uxParameterNumber = 0;
   }
 	 else
	 {
			/* There are more parameters to return after this one. */
			xReturn = pdTRUE;
			uxParameterNumber++;
	 }
	}

	return xReturn;
}

static const CLI_Command_Definition_t xCliPMGpioOut =
{
	"pm-gpio-out", /* The command string to type. */
	"\r\npm-gpio-out:\r\n Test PM GPIO In\r\n",
	prvPMGpioOutCommand, /* The function to run. */
	1 /* No parameters are expected. */
};


#endif

#if defined(__AUDIO_TEST_VAD__) && (defined(CONFIG_VOC_SW_VAD) || defined(CONFIG_AUD_VAD))
static BaseType_t prvVadStartTestCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
const char *pcParameter;
BaseType_t xParameterStringLength, xReturn;
static UBaseType_t uxParameterNumber = 0;

long val;

	( void ) pcCommandString;
	( void ) xWriteBufferLen;
	configASSERT( pcWriteBuffer );

	if( uxParameterNumber == 0 )
	{
		uxParameterNumber = 1U;
		xReturn = pdPASS;
	}
	else
	{
		/* Obtain the parameter string. */
		pcParameter = FreeRTOS_CLIGetParameter
						(
							pcCommandString,		/* The command string itself. */
							uxParameterNumber,		/* Return the next parameter. */
							&xParameterStringLength	/* Store the parameter string length. */
						);

		/* Sanity check something was returned. */
		configASSERT( pcParameter );

		/* Return the parameter string. */
		memset( pcWriteBuffer, 0x00, xWriteBufferLen );
    val = strtol(pcParameter, NULL, 10);

	 if( uxParameterNumber == 1 )
	 {
     //MDrv_AUD_VadEnable(TRUE, (U8)val);
     if (val)
     {
#if defined(CONFIG_AUD_VAD)
       MDrv_AUD_VadSetThr((U8)val);
       MDrv_AUD_VadEnable(TRUE);
#endif
       MApi_VOC_VADTunePort((U8 *)&val, 0);
     }
     else
     {
       VocTaskEnableFunc(TRUE, E_FUNC_VP);
       VocTaskVQConfig(E_VQ_MODE_TEST);
       VocTaskEnableFunc(TRUE, E_FUNC_VQ);
     }

		 xReturn = pdFALSE;
		 uxParameterNumber = 0;
   }
 	 else
	 {
			/* There are more parameters to return after this one. */
			xReturn = pdTRUE;
			uxParameterNumber++;
	 }
	}

	return xReturn;
}

static const CLI_Command_Definition_t xCliVadStart =
{
	"vad-start", /* The command string to type. */
	"\r\nvad-start:\r\n Start VAD with Threshold\r\n",
	prvVadStartTestCommand, /* The function to run. */
	1 /* No parameters are expected. */
};

static BaseType_t prvVadStopTestCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
  ( void ) pcWriteBuffer;
	( void ) pcCommandString;
	( void ) xWriteBufferLen;

  VocTaskEnableFunc(FALSE, E_FUNC_VQ);
  VocTaskEnableFunc(FALSE, E_FUNC_VP);

  return pdFALSE;;
}


static const CLI_Command_Definition_t xCliVadStop =
{
	"vad-stop", /* The command string to type. */
	"\r\nvad-stop:\r\n Stop VAD\r\n",
	prvVadStopTestCommand, /* The function to run. */
	0 /* No parameters are expected. */
};
#endif


#if defined(__AUDIO_TEST_HPF__)
static BaseType_t prvHpfStartTestCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
  ( void ) pcWriteBuffer;
	( void ) pcCommandString;
	( void ) xWriteBufferLen;

  MApi_VOC_HpfEnable(2);
  return pdFALSE;;
}
static const CLI_Command_Definition_t xCliHpfStart =
{
	"hpf-start", /* The command string to type. */
	"\r\nhpf-start:\r\n Start HPF with Threshold\r\n",
	prvHpfStartTestCommand, /* The function to run. */
	0 /* No parameters are expected. */
};

static BaseType_t prvHpfStopTestCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
  ( void ) pcWriteBuffer;
	( void ) pcCommandString;
	( void ) xWriteBufferLen;

  MApi_VOC_HpfEnable(0);
  return pdFALSE;;
}


static const CLI_Command_Definition_t xCliHpfStop =
{
	"hpf-stop", /* The command string to type. */
	"\r\nhpf-stop:\r\n Stop HPF\r\n",
	prvHpfStopTestCommand, /* The function to run. */
	0 /* No parameters are expected. */
};
#endif

#if defined(__AUDIO_TEST_SYS__)
static BaseType_t prvSelfResetCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
  ( void ) pcWriteBuffer;
	( void ) pcCommandString;
	( void ) xWriteBufferLen;

  Mhal_PM_SelfReset();
  return pdFALSE;;
}


static const CLI_Command_Definition_t xCliSelfReset =
{
	"self-reset", /* The command string to type. */
	"\r\nself-reset:\r\n  Reset\r\n",
	prvSelfResetCommand, /* The function to run. */
	0 /* No parameters are expected. */
};

static BaseType_t prvUartMuxCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
  ( void ) pcWriteBuffer;
	( void ) pcCommandString;
	( void ) xWriteBufferLen;

#if defined(__CONFIG_UART_MUX__)
  //Mhal_PM_SelfReset();
  uart_mux(0);
#endif
  return pdFALSE;;
}


static const CLI_Command_Definition_t xCliUartMux =
{
	"du", /* The command string to type. */
	"\r\ndu:\r\n  disable uart\r\n",
	prvUartMuxCommand, /* The function to run. */
	0 /* No parameters are expected. */
};

static BaseType_t prvBDMATestIMIToMIUCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
const char *pcParameter;
BaseType_t xParameterStringLength, xReturn;
static UBaseType_t uxParameterNumber = 0;

unsigned int buffer[16];
unsigned int buffer2[16];
buffer[0] = 0x78787878;
buffer[15] = 0x55665566;
long val;

	( void ) pcCommandString;
	( void ) xWriteBufferLen;
	configASSERT( pcWriteBuffer );

	if( uxParameterNumber == 0 )
	{
		uxParameterNumber = 1U;
		xReturn = pdPASS;
	}
	else
	{
		/* Obtain the parameter string. */
		pcParameter = FreeRTOS_CLIGetParameter
						(
							pcCommandString,		/* The command string itself. */
							uxParameterNumber,		/* Return the next parameter. */
							&xParameterStringLength	/* Store the parameter string length. */
						);

		/* Sanity check something was returned. */
		configASSERT( pcParameter );

		/* Return the parameter string. */
		memset( pcWriteBuffer, 0x00, xWriteBufferLen );
    val = strtol(pcParameter, NULL, 10);
    //uxParameterNumber++;

		if( uxParameterNumber == 1U )
		{
      memset(buffer2, 0, sizeof(buffer));
      MDrv_BDMA_Copy((U32)buffer, (U32)val, sizeof(buffer), E_BDMA_CPtoHK);
      MDrv_BDMA_Copy((U32)val, (U32)buffer2, sizeof(buffer), E_BDMA_HKtoCP);

      CLI_MSG("prvBDMATestIMIToMIUCommand result 0x%x = 0x%x 0x%x\r\n", val, buffer2[0], buffer2[15]);

			xReturn = pdFALSE;
			uxParameterNumber = 0;
		}
 		else
		{
			xReturn = pdTRUE;
			uxParameterNumber++;
		}
	}

	return xReturn;
}


static const CLI_Command_Definition_t xCliTestBDMAIMIToMIUSelf =
{
	"bdma-miu", /* The command string to type. */
	"\r\nbdma-miu:\r\n BDMA by self\r\n",
	prvBDMATestIMIToMIUCommand, /* The function to run. */
	1 /* No parameters are expected. */
};
#endif

#if defined(__AUDIO_TEST_PM__)
static BaseType_t prvSuspendCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
  ( void ) pcWriteBuffer;
	( void ) pcCommandString;
	( void ) xWriteBufferLen;

  Mhal_PM_Suspend();
  return pdFALSE;;
}


static const CLI_Command_Definition_t xCliSuspend =
{
	"suspend", /* The command string to type. */
	"\r\nsuspend:\r\n  Suspend\r\n",
	prvSuspendCommand, /* The function to run. */
	0 /* No parameters are expected. */
};

static BaseType_t prvResumeCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
  ( void ) pcWriteBuffer;
	( void ) pcCommandString;
	( void ) xWriteBufferLen;

  Mhal_PM_Resume();
  //MDrv_CPU_SetFreq(E_CPU_FREQ_LOW);
  return pdFALSE;;
}


static const CLI_Command_Definition_t xCliResume =
{
	"resume", /* The command string to type. */
	"\r\nresume:\r\n  Resume\r\n",
	prvResumeCommand, /* The function to run. */
	0 /* No parameters are expected. */
};
#endif


void vRegisterCLICommandsAudioTest( void )
{
	/* Register all the command line commands defined immediately above. */
#if defined(__AUDIO_TEST_SYS__)
  FreeRTOS_CLIRegisterCommand( &xCliInt );
  FreeRTOS_CLIRegisterCommand( &xCliPMGpioIn );
  FreeRTOS_CLIRegisterCommand( &xCliPMGpioOut );
  FreeRTOS_CLIRegisterCommand( &xCliSelfReset );
  FreeRTOS_CLIRegisterCommand( &xCliUartMux );
  FreeRTOS_CLIRegisterCommand( &xCliTestBDMAIMIToMIUSelf );
#endif
#if defined(__AUDIO_TEST_PM__)
  FreeRTOS_CLIRegisterCommand( &xCliSuspend );
  FreeRTOS_CLIRegisterCommand( &xCliResume );
#endif
  FreeRTOS_CLIRegisterCommand( &xCliDmicConfig );
  FreeRTOS_CLIRegisterCommand( &xCliDmicGain );
  FreeRTOS_CLIRegisterCommand( &xCliDmaWidth );
#if defined(CONFIG_VOC_MIC_MAPPING)
  FreeRTOS_CLIRegisterCommand( &xCliMicMapConfig);
#endif
#if defined(__AUDIO_TEST_VAD__) && (defined(CONFIG_VOC_SW_VAD) || defined(CONFIG_AUD_VAD))
  FreeRTOS_CLIRegisterCommand( &xCliVadStop );
  FreeRTOS_CLIRegisterCommand( &xCliVadStart );
#endif
#if defined(__AUDIO_TEST_HPF__)
  FreeRTOS_CLIRegisterCommand( &xCliHpfStop );
  FreeRTOS_CLIRegisterCommand( &xCliHpfStart );
#endif
  FreeRTOS_CLIRegisterCommand( &xCliStartVP );
  FreeRTOS_CLIRegisterCommand( &xCliStopVP );
  FreeRTOS_CLIRegisterCommand( &xCliStartVQ );
  FreeRTOS_CLIRegisterCommand( &xCliStopVQ );
  FreeRTOS_CLIRegisterCommand( &xCliStop );
#if defined(CONFIG_AUD_I2S)
  FreeRTOS_CLIRegisterCommand( &xCliEnableI2S );
  FreeRTOS_CLIRegisterCommand( &xCliDisableI2S );
#endif
#if defined(CONFIG_AUD_AEC)
  FreeRTOS_CLIRegisterCommand( &xCliEnableAEC );
  FreeRTOS_CLIRegisterCommand( &xCliDisableAEC );
#endif
#if defined(__AUDIO_TEST_REC__)
	FreeRTOS_CLIRegisterCommand( &xCliTestRec );
#endif

#if defined(__AUDIO_TEST_BDMA__)
  FreeRTOS_CLIRegisterCommand( &xCliTestBDMA );
  FreeRTOS_CLIRegisterCommand( &xCliTestBDMAInt );
#endif

#if defined(__AUDIO_TEST_BDMA_IMITOMIU__)
  FreeRTOS_CLIRegisterCommand( &xCliTestBDMAIMIToMIU );
#endif

#if defined(__AUDIO_TEST_CYBER_VQ__) || defined(__AUDIO_TEST_NUANCE_VQ__)
  FreeRTOS_CLIRegisterCommand( &xCliTestVQ );
#endif

#if defined(__AUDIO_TEST_CYBER_VQ2__)
  FreeRTOS_CLIRegisterCommand( &xCliCyberTime );
#endif


}


  /*-----------------------------------------------------------*/

