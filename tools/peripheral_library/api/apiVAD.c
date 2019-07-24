/* Standard includes. */
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

#define VQ_MSG(fmt, args...) MOS_DBG_PRINT(MOS_DBG_LEVEL_VQ, "[VQ] " fmt, ##args)
#define VQ_ERROR(fmt, args...) MOS_DBG_ERROR("[VQ ERR] " fmt, ##args);

//#define VAD_DCT4
#if defined(CONFIG_VOC_SW_VAD)
#if defined(VAD_DCT4)
/* vad process */
/* func */
static float* lin_filter(float* c, int l, int r, float* p, int n);
static void   mul_scaler(float* x, int l, float  s);
static void   mul_matrix(float* m, int r, int c, float* x, float* y);
static void   estim_snrv(float* v, int l, float* o);
static float  calc_dist2(float* x, int l, float* y);
/* data */
#define BIQ_IIR     (3)
#define BIQ_FIR     (3)
#define DCT4_SZ     (16)
#define EXTRA       (DCT4_SZ-1+(BIQ_FIR-1))
#define BIAS        (EXTRA-(BIQ_FIR-1))
static float biqcof[][5] = {
{-0.8905, 1.8787, 0.8399,-1.6797, 0.8399},
};
/*
static float gain[] = {
  0.8399, 1.0000,
};
*/
/*
hamming window:
 ham_win = 0.54-0.46*cos(2PI/(N-1))
dct4:
 dct4 = sqrt(2/N)*cos((PI/N)*(n+0.5)*(k+0.5))
 */
static float ham_dct4_16[16][16] = {
{ 0.0282502, 0.0418864, 0.0796348, 0.1324395, 0.1879563, 0.2335047, 0.2590288, 0.2593325, 0.2350453, 0.1921088, 0.1399573, 0.0888967, 0.0473876, 0.0199475, 0.0062133, 0.0013878,},
{ 0.0279781, 0.0382792, 0.0608284, 0.0723147, 0.0505201,-0.0133580,-0.1086446,-0.2084944,-0.2811222,-0.3036413,-0.2719082,-0.2016877,-0.1206498,-0.0551317,-0.0181047,-0.0041502,},
{ 0.0274366, 0.0313754, 0.0276570,-0.0206394,-0.1238571,-0.2460985,-0.3221045,-0.3002045,-0.1799357,-0.0158240, 0.1163959, 0.1670019, 0.1391395, 0.0772961, 0.0284370, 0.0068725,},
{ 0.0266309, 0.0217696,-0.0120458,-0.1042237,-0.2076683,-0.2186621,-0.0783594, 0.1496440, 0.3163957, 0.3128282, 0.1621709,-0.0102021,-0.0944628,-0.0812065,-0.0363203,-0.0095287,},
{ 0.0255687, 0.0102889,-0.0489040,-0.1404926,-0.1396297, 0.0399453, 0.2766114, 0.3295399, 0.1179113,-0.1657945,-0.2692896,-0.1540576, 0.0069020, 0.0659394, 0.0410757, 0.0120931,},
{ 0.0242602,-0.0020778,-0.0742131,-0.1129808, 0.0305080, 0.2563223, 0.2389515,-0.0850429,-0.3395104,-0.2165730, 0.0917136, 0.2056684, 0.0837922,-0.0351002,-0.0422938,-0.0145410,},
{ 0.0227181,-0.0142655,-0.0819962,-0.0341781, 0.1783379, 0.2017137,-0.1378835,-0.3462112,-0.0513556, 0.2915302, 0.1828226,-0.1068916,-0.1364465,-0.0040282, 0.0398695, 0.0168489,},
{ 0.0209573,-0.0252247,-0.0704153, 0.0601407, 0.1957647,-0.0661480,-0.3190025, 0.0171737, 0.3495778, 0.0473195,-0.2640775,-0.0700457, 0.1271569, 0.0422053,-0.0340116,-0.0189946,},
{ 0.0189946,-0.0340116,-0.0422053, 0.1271569, 0.0700457,-0.2640775,-0.0473195, 0.3495778,-0.0171737,-0.3190025, 0.0661480, 0.1957647,-0.0601407,-0.0704153, 0.0252247, 0.0209573,},
{ 0.0168489,-0.0398695,-0.0040282, 0.1364465,-0.1068916,-0.1828226, 0.2915302, 0.0513556,-0.3462112, 0.1378835, 0.2017137,-0.1783379,-0.0341781, 0.0819962,-0.0142655,-0.0227181,},
{ 0.0145410,-0.0422938, 0.0351002, 0.0837922,-0.2056684, 0.0917136, 0.2165730,-0.3395104, 0.0850429, 0.2389515,-0.2563223, 0.0305080, 0.1129808,-0.0742131, 0.0020778, 0.0242602,},
{ 0.0120931,-0.0410757, 0.0659394,-0.0069020,-0.1540576, 0.2692896,-0.1657945,-0.1179113, 0.3295399,-0.2766114, 0.0399453, 0.1396297,-0.1404926, 0.0489040, 0.0102889,-0.0255687,},
{ 0.0095287,-0.0363203, 0.0812065,-0.0944628, 0.0102021, 0.1621709,-0.3128282, 0.3163957,-0.1496440,-0.0783594, 0.2186621,-0.2076683, 0.1042237,-0.0120458,-0.0217696, 0.0266309,},
{ 0.0068725,-0.0284370, 0.0772961,-0.1391395, 0.1670019,-0.1163959,-0.0158240, 0.1799357,-0.3002045, 0.3221045,-0.2460985, 0.1238571,-0.0206394,-0.0276570, 0.0313754,-0.0274366,},
{ 0.0041502,-0.0181047, 0.0551317,-0.1206498, 0.2016877,-0.2719082, 0.3036413,-0.2811222, 0.2084944,-0.1086446, 0.0133580, 0.0505201,-0.0723147, 0.0608284,-0.0382792, 0.0279781,},
{ 0.0013878,-0.0062133, 0.0199475,-0.0473876, 0.0888967,-0.1399573, 0.1921088,-0.2350453, 0.2593325,-0.2590288, 0.2335047,-0.1879563, 0.1324395,-0.0796348, 0.0418864,-0.0282502,},
};
static float cluscent[2][16] = {
{  0.1114, -1.5807, -4.0044, -6.9048, -7.9124, -8.0654, -8.1201, -8.2245, -8.3081, -8.4069, -8.4254, -8.4192, -8.4139, -8.4480, -8.5033, -8.5593,},
{ -1.0735, -1.6652, -3.2114, -5.4024, -7.8142, -9.0126, -9.4507,-10.0433,-10.8516,-11.5526,-11.9593,-11.9424,-11.9447,-11.9462,-12.0242,-12.1012,},
};

static float log2_tab64[] = {
0.0000000000000000,
0.0223678126931190,
0.0443941205739975,
0.0660891905426979,
0.0874628424644470,
0.1085244566202164,
0.1292830109596252,
0.1497471183538437,
0.1699250042438507,
0.1898245662450790,
0.2094533592462540,
0.2288186848163605,
0.2479275166988373,
0.2667865455150604,
0.2854022085666656,
0.3037807345390320,
0.3219280838966370,
0.3398500084877014,
0.3575519919395447,
0.3750394284725189,
0.3923174142837524,
0.4093909263610840,
0.4262647628784180,
0.4429434835910797,
0.4594316184520721,
0.4757334291934967,
0.4918530881404877,
0.5077946186065674,
0.5235619544982910,
0.5391588211059570,
0.5545888543128967,
0.5698556303977966,
0.5849624872207642,
0.5999128222465515,
0.6147098541259766,
0.6293566226959229,
0.6438561677932739,
0.6582114696502686,
0.6724253296852112,
0.6865005493164062,
0.7004396915435791,
0.7142454981803894,
0.7279204726219177,
0.7414669990539551,
0.7548875212669373,
0.7681843042373657,
0.7813597321510315,
0.7944158911705017,
0.8073549270629883,
0.8201789855957031,
0.8328900337219238,
0.8454900383949280,
0.8579809665679932,
0.8703647255897522,
0.8826430439949036,
0.8948177695274353,
0.9068905711174011,
0.9188632369041443,
0.9307373166084290,
0.9425144791603088,
0.9541963338851929,
0.9657843112945557,
0.9772799015045166,
0.9886847138404846,
1.0000000000000000,
};

#define __VAD_GAIN__
#ifdef __VAD_GAIN__
const float pos_gain[]={
1.0,
1.122018454,
1.258925412,
1.412537545,
1.584893192,
1.77827941,
};


const float neg_gain[]={
1.0,
0.891250938,
0.794328235,
0.707945784,
0.630957344,
0.562341325,
};


#endif
static float log2b(float x) {
    float v = 1.0f + x;
    int p = *((int*)&v);
    float r = log2_tab64[(p>>17)&0x3F];
    r += (float)(((p>>23)&255)-127);
    return r;
}

static float* lin_filter(float* c, int l, int r, float* p, int n) {
    float* e = p + n;
    while (p < e) {
        float* t = c, m = 0.0f;
        float* s = p - r;
        int k = l;
        while (k-- > 0) {
            m += *t++ * *s++;
        }
        *p++ = m;
    }
    return p-n-r;
}

static void   mul_scaler(float* x, int l, float s) {
    while (l-- > 0) {
        *x = *x * s;
        x++;
    }
}

static void   mul_matrix(float* m, int r, int c, float* x, float* y) {
    float* e = m + r;
    while (c-- > 0) {
        float* s = x, v = 0.0f;
        do {
            v += *m++ * *s++;
        } while (++m < e);
        *y++ = v;
        e += r;
    }
}

static void   estim_snrv(float* v, int l, float* o) {
    float sum = 0.0f, *s = v, *d = o;
    int i = l;
    while (i-- > 0) {
        float q = *s * *s++;
        *d++ = q;
        sum += q;
    }
    i = l;
    while (i-- > 0) {
        float q = *o;
        *o++ = log2b(q) - log2b(sum-q);
    }
}

static float calc_dist2(float* x, int l, float*y) {
    float d2 = 0.0f;
    while (l-- > 0) {
        float d = *x++ - *y++;
        d2 += d * d;
    }
    return d2;
}

static int vad_init_flag = 0;
static int ch;
static short* pcms;
static int count;
static float* buff = NULL;

#ifdef __VAD_GAIN__

//default 26dB
static float fMinorGain; //1.0; 1dB
static unsigned short usGainLevel; //0; 6dB
static short sTargetGain = 27;

static void  set_gain(short dB)
{
    if(dB>0)
    {
        fMinorGain = pos_gain[dB%6];
        usGainLevel = dB/6;
    }
    else if(dB<0)
    {
        fMinorGain = neg_gain[(-1*dB)%6];
        usGainLevel = (-1*dB)/6;
    }
    else
    {
        fMinorGain = 1.0;
        usGainLevel = 0;
    }
    sTargetGain = dB;
}

void  MApi_VOC_SetVADGain(short dB)
{
    set_gain(dB);
    VQ_ERROR( "MApi_VOC_SetVADGain: set_gain = %d\n", dB);
}

//only for 16bit input
static float gain_process(short sample)
{
    float fRet;
    if(sTargetGain==0)
    {
        fRet = (float)sample;
    }
    else if(sTargetGain>0)
    {
        S32 tmp;
        tmp = (S32)sample<<usGainLevel;

        fRet = (float)tmp * fMinorGain;
        if(fRet>32767.0)
        {
            fRet = 32767.0;
        }
        else if(fRet<-32768.0)
        {
            fRet = -32768.0;
        }
    }
    else
    {
        sample = sample>>usGainLevel;

        fRet = (float)sample * fMinorGain;
    }
    return fRet;
}

#endif

int vad_process(S16 *ps16Buffer, U32 u32Count)
{
  int i;
  //int rdsz = 0;
  float vec[16];
  float d0, d1;
  //float* buff;

  float* cur = buff + EXTRA;

  pcms = ps16Buffer;
  count = u32Count/ch;
  /*[1] convert pcm data to float samples */
  for (i = 0; i < count; i++) {
#ifdef __VAD_GAIN__
      *cur++ = gain_process(pcms[i*ch+0]);
#else
      *cur++ = (float)pcms[i*ch+0];
#endif
  }
  /*[2] biquade filter */
  cur = buff + BIAS;
  //mul_scaler(cur, count, gain[0]);
  lin_filter(biqcof[0], BIQ_FIR+BIQ_IIR-1, BIQ_IIR-1, cur, count);
  cur-= DCT4_SZ-1;
/*
  if (dbgf && dbgp) {
      for (i = 0; i < count; i++) {
          dbgp[i] = (int16_t)cur[i];
      }
      wave_write(dbgf, dbgp, count);
  }
  */
  for (i = 0; i < count; i+=4, cur+=4) {
  /*[3] window & dct4 */
      mul_matrix(&ham_dct4_16[0][0], 16, 16, cur, vec);

  /*[4] compute SNR */
      estim_snrv(vec, 16, vec);
  /*[5] determine voice or not */
      d0 = calc_dist2(vec, 16, cluscent[0]);
      d1 = calc_dist2(vec, 16, cluscent[1]);
      if (d1 < d0)
      {
          //VQ_MSG("vad: %f<->%f\n",d0,d1);
          memset(buff, 0, sizeof(float)*(count+EXTRA));
          return 1;
      }
  }
  /*[?] move tail samples to the beginning */
  memcpy(buff, cur, EXTRA*sizeof(float));
  /*[-] vad process */

  return 0;
}

BOOL MApi_VOC_VADPort(S16 *ps16Buffer, U32 u32Count, U32 *pu32Result)
{
  if (vad_init_flag == 0)
  {
#ifdef __VAD_GAIN__
/*
    fMinorGain = pos_gain[0];//1.0; 1dB
    usGainLevel = 5;//0; 6dB
    sTargetGain = 30;
*/
    set_gain(sTargetGain);
#endif

    ch = MDrv_AUD_GetMicNo();
    count = u32Count/ch;

    buff = malloc(sizeof(float)*(count+EXTRA));
    if (!buff)
    {
      VQ_ERROR("MApi_VOC_VADPort: Init malloc failed\r\n");
      return FALSE;
    }
    VQ_MSG("MApi_VOC_VADPort: malloc(%d) = 0x%x\r\n", sizeof(float)*(count+EXTRA), buff);
    memset(buff, 0, sizeof(float)*(count+EXTRA));

    vad_init_flag = 1;
  }

  if (vad_init_flag)
  {
    *pu32Result = vad_process(ps16Buffer, u32Count);

    return TRUE;

  }

  return FALSE;
}

BOOL MApi_VOC_VADTunePort(U8 *pu8Buffer, U32 u32Len)
{
  (void) pu8Buffer;
  (void) u32Len;
#ifdef __VAD_GAIN__
  S16 s16Gain;
  memcpy(&s16Gain, pu8Buffer, sizeof(S16));
  set_gain(s16Gain);
  VQ_MSG( "MApi_VOC_VADTunePort: set_gain = %d\n", s16Gain);
#endif
  return TRUE;
}

#else
// VAD@DNN
#include "crnn_wrapper.h"
static uint8_t _workzone[4096];
BOOL MApi_VOC_VADPort(S16 *ps16Buffer, U32 u32Count) {
    static void* _vadp = NULL;
    static U32 _num_ch = 0;
    int16_t voice_pcms[VADCRNN_FRAME];
    int16_t *from;
    uint32_t i, j;
    if (!_vadp) {
        _vadp = _workzone;
        if (0 != vadcrnn_init(_vadp, 4096)) {
            _vadp = NULL;
            return FALSE;
        }
        _num_ch = MDrv_AUD_GetMicNo();
    }
    from = ps16Buffer;
    for (i = 0; i < u32Count; i += VADCRNN_FRAME) {
        for (j = 0; j < VADCRNN_FRAME; j++, from += _num_ch) {
            voice_pcms[j] = *from;
        }
        if (vadcrnn_proc(_vadp, voice_pcms)) {
            _vadp = NULL;
            return TRUE;
        }
    }
    return FALSE;
}
#endif//VAD_DCT4
#endif
