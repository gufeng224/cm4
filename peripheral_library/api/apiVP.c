/* Standard includes. */
#include <string.h>
#include <stdio.h>

/* Kernel includes. */
#include "mosWrapper.h"

/* driver includes. */
#include "drvAudio.h"
#include "drvBDMA.h"
#include "drvMBX.h"
#include "drvPM.h"
#include "drvCPU.h"

#include "voc_task.h"
#include "apiVOC.h"

#define VP_MSG(fmt, args...) MOS_DBG_PRINT(MOS_DBG_LEVEL_VP, "[VP] " fmt, ##args)
#define VP_ERROR(fmt, args...) MOS_DBG_ERROR("[VP ERR] " fmt, ##args);

static U8 _u8VPScale = 1;
static U8 _u8MicMap[8] = {0, 1, 2, 3, 4, 5, 6, 7};
static S16 _s16MicData[8] = {0};

static U8 _u8AudMicSwGain = 0;
static U8 _u8AudSwGain = 0;
//2.14
static const S16 _s16GainTable[6]={
     16384,
     18383,
     20626,
     23143,
     25966,
     29135
};

static U16 _u16GainBitwidth;
static U16 _u16GainMicChn;
static U16 _u16GainProcChn;

__attribute__((weak)) int MApi_VOC_VPGetAnglePort(void)
{
  return 0;
}

__attribute__((weak)) BOOL MApi_VOC_VPTunePort(U8 *pu8Buffer, U32 u32Len)
{
  (void)u32Len;

  _u8VPScale = *pu8Buffer;

  return TRUE;
}

__attribute__((weak)) BOOL MApi_VOC_VPPort(S16 *ps16Buffer, U32 *pu32Count)
{
  U32 i;
  U8 u8MicNo = MDrv_AUD_GetMicNo();
  for(i = 0; i < *pu32Count / u8MicNo; i++) //sterero to mono
  {
    ps16Buffer[i] = ps16Buffer[i * u8MicNo ] * _u8VPScale;
  }

  *pu32Count = *pu32Count / u8MicNo;

  return TRUE;
}

BOOL MApi_VOC_VPNto1(S16 *ps16Buffer, U32 *pu32Count)
{
  U32 i;
  U8 u8MicNo = MDrv_AUD_GetMicNo();
  for(i = 0; i < *pu32Count / u8MicNo; i++) //sterero to mono
  {
    ps16Buffer[i] = ps16Buffer[i * u8MicNo + 1] * _u8VPScale;
  }

  *pu32Count = *pu32Count / u8MicNo;

  return TRUE;
}


BOOL MApi_VOC_VP2to4(S16 *ps16Buffer, U32 *pu32Count)
{
  unsigned int i = 0;

  U8 u8MicNo = MDrv_AUD_GetMicNo();

  for(i = 0; i < *pu32Count / u8MicNo; i++) //sterero to mono
  {
    ps16Buffer[i * u8MicNo + 2] = ps16Buffer[i * u8MicNo] * _u8VPScale;
    ps16Buffer[i * u8MicNo + 3] = ps16Buffer[i * u8MicNo + 1] * _u8VPScale;
  }

  return TRUE;
}

void Mapi_VOC_VPConfig(U8 u8Scale)
{
  _u8VPScale = u8Scale;
  return;
}

__attribute__((weak)) BOOL MApi_VOC_CUSPort0(S16 *ps16Buffer, U32 *pu32Count)
{
  U32 i;
//  U8 u8MicNum = MDrv_AUD_GetChn();
//  U16* u16MicIndex[8] = {0x0100,0x0200,0x0300,0x0400,0x0500,0x0600,0x0700,0x0800};
//  S16 u16MicIndex[u8MicNum] ;
//  S16 u16MicIndex[4] = {0x0006, 0x0002, 0x0005, 0x0001};
  U16 u16MicIndex[4] = {0x0600,0x0200,0x0500,0x0100};
  S16 ps16DataBuffer ;
  U8 u8MicNo;
  U8 u8MicNum = 4;
//  S16 t;

if (MDrv_AUD_GetMicNo() != 4)
	return FALSE;

if (MDrv_AUD_GetBitWidth() != 32)
	return FALSE;


//  for (t = 0 ; t < u8MicNum ; t++)
//   u16MicIndex[t] = (t+1) * 0x100 ;
//  VOC_MSG("MApi_VOC_DA : start \n";
//  for(i = 0; i < *pu32Count / u8MicNum; i++)
//  for(i = 0; i < *pu32Count / 1 ; i+=4)
//  {
//    ps16Buffer[i] = u16MicIndex [0];
//  }

for (u8MicNo = 0; u8MicNo < u8MicNum ; u8MicNo++ )// write ID
{
  for(i = u8MicNo*2; i < *pu32Count / 1 ; i+=(u8MicNum*2))
  {
    ps16Buffer[i] = u16MicIndex [u8MicNo];
  }
}

//for (u8MicNo = 0; u8MicNo < u8MicNum ; u8MicNo++ )// write ID
//{
//  for(i = u8MicNo*2; i < *pu32Count / 1 ; i+=u8MicNum*2)
//  {
//    ps16DataBuffer = ps16Buffer[i+1];
//    ps16Buffer[i] = (ps16DataBuffer >>8) & 0x00ff ;
//    ps16Buffer[i+1] = (ps16DataBuffer<<8) |u16MicIndex [u8MicNo] ;
//  }
//}

  for(i = 0; i < *pu32Count / 1 ; i+=u8MicNum*2) // arrange data
  {
    ps16DataBuffer = ps16Buffer[i]; // 0~1:MIC1 2~3:MIC2 4~5:MIC3 6~7:MIC4
    ps16Buffer[i] = ps16Buffer[i+6];
    ps16Buffer[i+6] = ps16DataBuffer;

    ps16DataBuffer = ps16Buffer[i+1];
    ps16Buffer[i+1] = ps16Buffer[i+7];
    ps16Buffer[i+7] = ps16DataBuffer;
  }

  return TRUE;
}

BOOL MApi_VOC_MicMapConfig(U8 u8DestMicNo, U8 u8SrcMicNo)
{
  if (u8DestMicNo < 8 && u8SrcMicNo < 8)
    _u8MicMap[u8DestMicNo] = u8SrcMicNo;
  else
    return FALSE;

  VP_MSG("MApi_VOC_MicMapConfig: map %d to %d\n", u8SrcMicNo, u8DestMicNo);
  return TRUE;
}

BOOL MApi_VOC_MicMap(S16 *ps16Buffer, U32 *pu32Count)
{
  unsigned int c1, c2, c3;

  U8 u8MicNo = MDrv_AUD_GetMicNo();
//  U8 u8BitWidth = MDrv_AUD_GetBitWidth();

  c3 = 0;
  for( c2 = 0; c2 < *pu32Count/u8MicNo; c2++ )
  {
      for( c1 = 0; c1 < u8MicNo; c1++ )
      {
          _s16MicData[ c1 ] = ps16Buffer[ c3 + c1 ];
      }

      for( c1 = 0; c1 < u8MicNo; c1++ )
      {
          ps16Buffer[ c3 ] = _s16MicData[ _u8MicMap[c1] ];
          c3++;
      }
  }

  return TRUE;
}

#define MAX_S16 32767//0x7FFF
#define MIN_S16 -32768//0x8000

//#define MAX_S24 8388607L
//#define MIN_S24 -8388608L

#define MAX_S32 2147483392LL//2147483647LL , MAX_S24<<8
#define MIN_S32 -2147483648LL

#if defined(CONFIG_VOC_SWHPF)

static S32 _s32PreXX[8][2]={0};
static S32 _s32PreYY[8][2]={0};
static S64 _s64PreXX[8][2]={0};
static S64 _s64PreYY[8][2]={0};

static S16 _s16Coef=5;
static U16 _u16Stage=2;

static U16 _u16ProcChn;
static U16 _u16Channel;
static U16 _u16Bitwidth;

S16 HPF_16(S8* ps8Buffer, U16 u16Size)
{
  S32 s32CurYY;
  S32 s32CurXX;
  S16 s16CurX;
  S16 s16CurY;
  U16 u16Frame;
  U16 u16Chn=0;
  U16 u16Num=0;
  U16 u16Bytes=2;
  U16 u16Iter=0;

  if(u16Size == 0){
    VP_ERROR("[ERROR] Size 0\n");
    return -1;
  }

  if(!ps8Buffer){
    VP_ERROR("[ERROR] NULL Buffer\n");
    return -1;
  }

  if(_u16Channel>8 || _s16Coef>9 || _s16Coef<=-3 || _u16Stage>2){
    VP_ERROR("[ERROR] param not support\n");
    return -1;
  }

 // VOC_ERROR("[HPF_16] size %u, coef %u, chn %u\n",u16Size,u16Coef,u16Channel);

  u16Frame = u16Size/(_u16Channel*u16Bytes);

  while(u16Num<u16Frame){
    for(u16Chn=0;u16Chn<_u16Channel;u16Chn++){

      if(u16Chn >= _u16ProcChn)
        continue;

      s16CurX =*(S16*)(ps8Buffer+(u16Num*_u16Channel+u16Chn)*u16Bytes);
      s32CurYY = (S32)s16CurX<<14; //Q3.29
      for(u16Iter=0;u16Iter<_u16Stage;u16Iter++){
        s32CurXX = s32CurYY;
        s32CurYY = s32CurXX - _s32PreXX[u16Chn][u16Iter] + _s32PreYY[u16Chn][u16Iter] - (_s32PreYY[u16Chn][u16Iter]>>(_s16Coef+3));

        _s32PreXX[u16Chn][u16Iter] = s32CurXX;
        _s32PreYY[u16Chn][u16Iter] = s32CurYY;
      }

      s32CurYY = s32CurYY>>14;

      if(s32CurYY>MAX_S16)
        s16CurY = MAX_S16;
      else if(s32CurYY<MIN_S16)
        s16CurY = MIN_S16;
      else
        s16CurY = (S16)s32CurYY;

      *(S16*)(ps8Buffer+(u16Num*_u16Channel+u16Chn)*u16Bytes) = s16CurY;

    }
    u16Num++;
  }

  return 0;
}


S16 HPF_32(S8* ps8Buffer, U16 u16Size)
{
  S64 s64CurYY;
  S64 s64CurXX;
  S32 s32CurX;
  S32 s32CurY;
  U16 u16Num=0;
  U16 u16Frame;
  U16 u16Chn=0;
  U16 u16Bytes=4;
  U16 u16Iter=0;

  if(u16Size == 0){
    VP_ERROR("[ERROR] Size 0\n");
    return -1;
  }

  if(!ps8Buffer){
    VP_ERROR("[ERROR] NULL Buffer\n");
    return -1;
  }

  if(_u16Channel>8 || _s16Coef>9 || _s16Coef<=-3 || _u16Stage>2){
    VP_ERROR("[ERROR] param not support\n");
    return -1;
  }

  u16Frame = u16Size/(_u16Channel*u16Bytes);

  while(u16Num<u16Frame){
    for(u16Chn=0;u16Chn<_u16Channel;u16Chn++){

      if(u16Chn >= _u16ProcChn)
        continue;

      s32CurX =*(S32*)(ps8Buffer+(u16Num*_u16Channel+u16Chn)*u16Bytes);
      s64CurYY = (S64)s32CurX<<6;
      for(u16Iter=0;u16Iter<_u16Stage;u16Iter++){
        s64CurXX = s64CurYY;
        s64CurYY = s64CurXX - _s64PreXX[u16Chn][u16Iter] + _s64PreYY[u16Chn][u16Iter] - (_s64PreYY[u16Chn][u16Iter]>>(3+_s16Coef));// (((s64PreYY<<12) - (s64PreYY<<(9-u16Coef)))>>12);

        _s64PreXX[u16Chn][u16Iter] = s64CurXX;
        _s64PreYY[u16Chn][u16Iter] = s64CurYY;
      }
      s64CurYY = (s64CurYY>>14)<<8; //LSB should be zero
      if(s64CurYY>MAX_S32)
        s32CurY = MAX_S32;
      else if(s64CurYY<MIN_S32)
        s32CurY = MIN_S32;
      else
        s32CurY = (S32)s64CurYY;
      *(S32*)(ps8Buffer+(u16Num*_u16Channel+u16Chn)*u16Bytes) = s32CurY;

    }
    u16Num++;
  }
  return 0;
}


void MApi_VOC_HpfReset(void)
{
  memset(_s32PreXX, 0, sizeof(_s32PreXX));
  memset(_s32PreYY, 0, sizeof(_s32PreYY));
  memset(_s64PreXX, 0, sizeof(_s64PreXX));
  memset(_s64PreYY, 0, sizeof(_s64PreYY));

  _u16Channel = MDrv_AUD_GetMicNo();
  _u16Bitwidth = MDrv_AUD_GetBitWidth();
  _u16ProcChn = _u16Channel;

#if defined(CONFIG_AUD_AEC) && defined(CONFIG_AUD_REFEXT)
  if(MDrv_AUD_AECIsEnable())
    _u16ProcChn = _u16Channel - 2;
#endif
}

BOOL MApi_VOC_Hpf(void* pvBuffer, U16 u16Size)
{
  S16 s16Ret;

  if(_u16Stage == 0)
    return TRUE;

  if(_u16Bitwidth == 16)
  {
    s16Ret = HPF_16((S8*)pvBuffer,u16Size);
  }
  else if(_u16Bitwidth==24 || _u16Bitwidth==32)//only support 24bit precision 32 bit
  {
    s16Ret = HPF_32((S8*)pvBuffer,u16Size);
  }
  else
  {
    return FALSE;
  }

  if(s16Ret)
    return FALSE;
  else
    return TRUE;
}

BOOL MApi_VOC_HpfConfig(S8 s8Coef)
{
  if(s8Coef<-2 || s8Coef>9)
  {
    VP_ERROR("[ERROR] wrong parameter\n");
    return FALSE;
  }

  _s16Coef = (S16)s8Coef;
  return TRUE;
}

BOOL MApi_VOC_HpfEnable(U8 u8StageEn)
{
  if(u8StageEn>2)
  {
    VP_ERROR("[ERROR] wrong parameter\n");
    return FALSE;
  }

  _u16Stage = u8StageEn;
  return TRUE;
}

#endif

#ifndef CONFIG_AUD_HWGAIN
void MApi_VOC_SetMicSwGain(U8 nStep)
{
  if (nStep >= 8)
    return;
  _u8AudMicSwGain = nStep;
}
#endif

void MApi_VOC_SwGainReset(void)
{
  _u16GainBitwidth = MDrv_AUD_GetBitWidth();
  _u16GainMicChn = MDrv_AUD_GetMicNo();
  _u16GainProcChn = _u16GainMicChn;

#if defined(CONFIG_AUD_AEC)
  if(MDrv_AUD_AECIsEnable())
    _u16GainProcChn = _u16GainMicChn - 2;
#endif
}

//table only support 0~5dB
void MApi_VOC_SetSwGain(U8 nLevel)
{
  if (nLevel >= 6)
    return;
  _u8AudSwGain = nLevel;
}

BOOL MApi_VOC_SwGain(void* pvBuffer, U16 u16Size)
{
  U16 u16Num;
  U16 u16Sample;
  S8* ps8Buffer = (S8*)pvBuffer;

  if(_u8AudMicSwGain==0 && _u8AudSwGain==0)
    return TRUE;

  if(_u16GainBitwidth==16)
  {
    S16 s16Cur;
    S32 s32Tmp;
    u16Sample = u16Size>>1;
    for(u16Num=0;u16Num<u16Sample;u16Num++)
    {
      if((u16Num%_u16GainMicChn) >= _u16GainProcChn)
        continue;

      s16Cur =*(S16*)(ps8Buffer+(u16Num*2));
      s32Tmp = (S32)s16Cur<<_u8AudMicSwGain;
      if(s32Tmp>MAX_S16)
        s16Cur = MAX_S16;
      else if(s32Tmp<MIN_S16)
        s16Cur = MIN_S16;
      else
      {
        if(_u8AudSwGain)
        {
          s32Tmp = (s32Tmp * _s16GainTable[_u8AudSwGain]) >> 14;
          if(s32Tmp>MAX_S16)
            s16Cur = MAX_S16;
          else if(s32Tmp<MIN_S16)
            s16Cur = MIN_S16;
          else
            s16Cur = (S16)s32Tmp;
        }
        else
        {
          s16Cur = (S16)s32Tmp;
        }
      }

      *(S16*)(ps8Buffer+(u16Num*2))=s16Cur;
    }

  }
  else if(_u16GainBitwidth==24 || _u16GainBitwidth==32)//only support 24bit precision 32 bit
  {
    S32 s32Cur;
    S64 s64Tmp;
    u16Sample = u16Size>>2;
    for(u16Num=0;u16Num<u16Sample;u16Num++)
    {
      s32Cur =*(S32*)(ps8Buffer+(u16Num*4));
      s64Tmp = (S64)s32Cur<<_u8AudMicSwGain;
       if(s64Tmp>MAX_S32)
        s32Cur = MAX_S32;
      else if(s64Tmp<MIN_S32)
        s32Cur = MIN_S32;
      else
      {
        if(_u8AudSwGain)
        {
          s64Tmp = (s64Tmp * _s16GainTable[_u8AudSwGain]) >> 14;
          if(s64Tmp>MAX_S32)
            s32Cur = MAX_S32;
          else if(s64Tmp<MIN_S32)
            s32Cur = MIN_S32;
          else
            s32Cur = (S32)s64Tmp;
        }
        else
        {
          s32Cur = (S32)s64Tmp;
        }
      }

      *(S32*)(ps8Buffer+(u16Num*4))=s32Cur;
    }
  }
  else
  {
    return FALSE;
  }

  return TRUE;
}


/*-----------------------------------------------------------*/

