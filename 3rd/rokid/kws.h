
#ifndef __KWS_H__
#define __KWS_H__
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif
	

#if defined(_MSC_VER)
    //  Microsoft
    #define EXPORT __declspec(dllexport)
    #define IMPORT __declspec(dllimport)
#elif defined(__GNUC__) || defined(__clang__)
    //  GCC
    #define EXPORT __attribute__((visibility("default")))
    #define IMPORT
#else
    //  do nothing and hope for the best?
    #define EXPORT
    #define IMPORT
    #pragma warning Unknown dynamic link import/export semantics.
#endif


typedef void*  kws_task ;
EXPORT kws_task kws_create(const char* model_name, int batch, float* thresholds, int min_gap);
EXPORT void kws_destroy(kws_task task);
EXPORT int kws_status(kws_task task, const float** audio, int in_length, int* flags);
EXPORT void kws_clear(kws_task task);

typedef void*  kws_model_task ;
EXPORT kws_model_task kws_model_create(const char* pNnetPath, int iCn);
EXPORT int kws_model_free(kws_model_task task);
EXPORT int kws_model_get_score(kws_model_task task, const float** pWavBuff, int iWavLen, float* pScore, int *iOutputLen);
EXPORT void kws_model_clear(kws_model_task task);
#ifdef __cplusplus
}
#endif
		
		
		
#endif // __KWS_H__

