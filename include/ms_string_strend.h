#ifndef MSLIB_STRING_STREND
#define MSLIB_STRING_STREND


#include "ms_string_common.h"


#ifdef __cplusplus
extern "C"
{
#endif


/*
    strend idea:

        align pointer [check on zero]

        vectorized fetch and check on zero (place -1 there it is.)

        gather bytes mask

        return position of zero using BMI(2).
*/


#ifdef __AVX2__
MSLIB_EXPORT char *ms_strend_avx2_d32(const char *str)
#if defined(ADD_SOURCE_CODE) || defined(FORCE_INLINE)
{
    const char *dest_aligned = str + ALIGN_PAD32(str);

    while (str < dest_aligned && *str)
    {
    //TODO: Add here duff's device ?
        str++;
    }

    if (*str == 0)
    {
        return (char *)str;
    }

    while (1)
    {
        __m256i ymm0 = _mm256_loadu_si256((__m256i *)(str +  0));
        uint32_t msk0 = _mm256_movemask_epi8(_mm256_cmpeq_epi8(ymm0, _mm256_setzero_si256()));
        if (msk0)
        {
            return (char *)str + _tzcnt_u32((size_t)msk0);
        }
        str += 32;
    }
}
#else
;
#endif



MSLIB_EXPORT char *ms_strend_avx2_d64(const char *str)
#if defined(ADD_SOURCE_CODE) || defined(FORCE_INLINE)
{
    const char *dest_aligned = str + ALIGN_PAD64(str);

    while (str < dest_aligned && *str)
    {
    //TODO: Add here duff's device ?
        str++;
    }

    if (*str == 0)
    {
        return (char *)str;
    }

    while (1)
    {
        __m256i ymm0 = _mm256_loadu_si256((__m256i *)(str +  0));
        __m256i ymm1 = _mm256_loadu_si256((__m256i *)(str + 32));
        uint32_t msk0 = _mm256_movemask_epi8(_mm256_cmpeq_epi8(ymm0, _mm256_setzero_si256()));
        uint32_t msk1 = _mm256_movemask_epi8(_mm256_cmpeq_epi8(ymm1, _mm256_setzero_si256()));
        if (msk0 | msk1)
        {
            return (char *)str + _tzcnt_u64((size_t)msk0 | ((size_t)msk1 << 32ull));
        }
        str += 64;
    }
}
#else
;
#endif
#endif



MSLIB_EXPORT char *ms_strend_x64(const char *str)
#if defined(ADD_SOURCE_CODE) || defined(FORCE_INLINE)
{
    while (*str)
    {
    //TODO: Add here duff's device ?
        str++;
    }
    return (char *)str;
}
#else
;
#endif



#ifdef __AVX2__
    #ifdef USE_32_STEP_STREND
        #define ms_strend(str) ms_strend_avx2_d32(str)
    #else
        #define ms_strend(str) ms_strend_avx2_d64(str)
    #endif
#else
    #define ms_strend(str) ms_strend_x64(str)
#endif



#ifdef __cplusplus
}
#endif



#endif