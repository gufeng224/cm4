#ifndef _API_VOC_H
#define _API_VOC_H

//#define __SW_VAD_TEST__

BOOL MApi_VOC_VADIsEnable(void);
BOOL MApi_VOC_VADIsTimeout(void);
void MApi_VOC_VADStartTimer(void);
void MApi_VOC_VADStopTimer(void);
void MApi_VOC_VADResetTimer(void);
BOOL MApi_VOC_VADPort(S16 *ps16Buffer, U32 u32Count, U32 *pu32Result);
BOOL MApi_VOC_VADTunePort(U8 *pu8Buffer, U32 u32Len);
BOOL MApi_VOC_VQPort(S16 *ps16Buffer, U32 u32Count);

void Mapi_VOC_VPConfig(U8 u8Scale);
BOOL MApi_VOC_VPPort(S16 *ps16Buffer, U32 *pu32Count);
BOOL MApi_VOC_VPTunePort(U8 *pu8Buffer, U32 u32Len);
int MApi_VOC_VPGetAnglePort(void);

void MApi_VOC_VDInit(void);
BOOL MApi_VOC_VDPort(S16 *ps16Buffer, U32 u32Count);
void MApi_VOC_VDConfig(U32 u32BufferAddr, U32 u32BufferSize);
void MApi_VOC_VDConfigExt(U32 u32BufferAddr, U32 u32BufferSize, U16 u16MaxToNotify);
void MApi_VOC_VDReset(void);
void MApi_VOC_VDNotify(U32 u32Count);

void MApi_VOC_NotifyPeriod(U32 u32Count);
void MApi_VOC_NotifyStartTime(void);
void MApi_VOC_NotifyStatus(U8 u8Status);

BOOL MApi_VOC_CUSPort0(S16 *ps16Buffer, U32 *pu32Count);

BOOL MApi_VOC_MicMapConfig(U8 u8DestMicNo, U8 u8SrcMicNo);
BOOL MApi_VOC_MicMap(S16 *ps16Buffer, U32 *pu32Count);

#if defined(CONFIG_VOC_SWHPF)
void MApi_VOC_HpfReset(void);
BOOL MApi_VOC_Hpf(void* pvBuffer, U16 u16Size);
BOOL MApi_VOC_HpfConfig(S8 s8Coef);
BOOL MApi_VOC_HpfEnable(U8 u8StageEn);
#endif

#ifndef CONFIG_AUD_HWGAIN
void MApi_VOC_SetMicSwGain(U8 nStep);
#endif
void MApi_VOC_SetSwGain(U8 nStep);
BOOL MApi_VOC_SwGain(void* pvBuffer, U16 u16Size);
void MApi_VOC_SwGainReset(void);


BOOL MApi_VOC_ReadPcmBin(S16 *ps16Buffer, U32 *pu32Count);

void  MApi_VOC_SetVADGain(short dB);

#endif

BOOL MApi_VOC_SrcInit(void);
BOOL MApi_VOC_SrcProcess(S16 *ps16Buffer, U32 *pu32Count);
BOOL MApi_VOC_SrcInit48k(void);
BOOL MApi_VOC_SrcProcess48k(S16 *ps16Buffer, U32 *pu32Count);

