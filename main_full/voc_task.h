#ifndef _VOC_TASK_H
#define _VOC_TASK_H

#include "kwsVQ.h"

typedef enum
{
  E_FUNC_NONE = 0,
  E_FUNC_VQ,
  E_FUNC_VD,
  E_FUNC_VP,
  E_FUNC_VS,
  E_FUNC_CUS0,
  E_FUNC_TEST,
  E_FUNC_VP_TEST,
  E_FUNC_NUM,
} VOC_FUNC_e;

typedef enum
{
  E_VQ_MODE_NORMAL = 0,
  E_VQ_MODE_LOOP,
  E_VQ_MODE_PM,
  E_VQ_MODE_VAD,
  E_VQ_MODE_DWW,
  E_VQ_MODE_TEST,
  E_VQ_MODE_MAX,
} VOC_VQMode_e;

typedef enum
{
  E_STATUS_XRUN = 0,
  E_STATUS_MAX,
} VOC_Status_e;

typedef BaseType_t (*VOC_PROC_CUS_FUNC_t)(short *, uint32_t);
void VocTaskRegisterCusFunc(VOC_PROC_CUS_FUNC_t pdfFunc);
void VocTaskStart(void);
void VocTaskReset(void);
void VocTaskEnableFunc(BOOL bEnable, VOC_FUNC_e eFunc);
BOOL VocTaskIsEnableFunc(VOC_FUNC_e eFunc);
void VocTaskVQPost(void);
void VocTaskVQConfig(VOC_VQMode_e eMode);

void main_audio(void);

#endif

