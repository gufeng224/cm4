#ifndef __RKIMPL_KWSVQ_H__
#define __RKIMPL_KWSVQ_H__
#include "mosWrapper.h"
#include "kws.h"
//#include "circular_buffer.h"

#define VQ_RKIMPL_PCM_BUF_MS (240) //240msec
#define VQ_RKIMPL_SAMPLE_RATE (16000)
#define VQ_RKIMPL_PCM_BUF_COUNT (VQ_RKIMPL_SAMPLE_RATE * VQ_RKIMPL_PCM_BUF_MS)/1000
#define VQ_RKIMPL_PCM_MULTI   20
static inline BOOL MApi_VOC_VQPort_RKImpl(S16 *ps16Buffer, U32 u32Count) {
  static U32 initedFlag = 0;
  static void* handle = NULL;
  static float raw_buffer[VQ_RKIMPL_PCM_BUF_COUNT];
  static float * in_ptr[8];
  int status;
  int i;

  if (initedFlag == 0) {
    float thresholdSetting[3] = { 0.92f, 1.0f, 1.0f };
	unsigned char channel_num = 1;
    int min_gap = 7;
    initedFlag = 1;
    handle = kws_create("123", channel_num, thresholdSetting, min_gap);
  }
   in_ptr[0] = raw_buffer;

  for (i = 0; i < u32Count; ++i) {
    raw_buffer[i] = ps16Buffer[i]*VQ_RKIMPL_PCM_MULTI;
  }

  status = kws_status(handle, (const float** )in_ptr, u32Count,NULL); 
  if (status == 1)
    return TRUE;

  return FALSE;
}

#endif // __RKIMPL_KWSVQ_H__

