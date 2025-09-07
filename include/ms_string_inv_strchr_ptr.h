#ifndef MSLIB_STRING_INV_STRCHR_PTR
#define MSLIB_STRING_INV_STRCHR_PTR


#include "ms_string_common.h"



#ifdef __cplusplus
extern "C"
{
#endif




/*
    inv_strchr idea:
        search for next not ch symbol - same as strchr_ptr,
        but invert character mask before checks.

        ! return pointer on char, and pointer on end of string if not found
*/
#ifdef __AVX2__
MSLIB_EXPORT char *ms_inv_strchr_ptr_avx2_d32(const char *str, int ch)
#if defined(ADD_SOURCE_CODE) || defined(FORCE_INLINE)
{
    if (str == NULL)
    {
        return NULL;
    }
    
    const char *str_aligned = str + ALIGN_PAD32(str);

    while (str < str_aligned && *str && *str == ch)
    {
        str++;
    }

    if (*str == 0 || *str != ch)
    {
        return (char *)str;
    }

    __m256i ymm0 = _mm256_set1_epi8(ch);

    while (1)
    {
        __m256i ymm1 = _mm256_loadu_si256((__m256i *)(str +  0));
        uint32_t msk0 = ~_mm256_movemask_epi8(_mm256_cmpeq_epi8(ymm1, ymm0));
        uint32_t msk1 = _mm256_movemask_epi8(_mm256_cmpeq_epi8(ymm1, _mm256_setzero_si256()));
        if (msk0 | msk1)
        {
            str += _tzcnt_u32(msk0 | msk1);
            return (char *)str;
        }
        str += 32;
    }
}
#else
;
#endif


MSLIB_EXPORT char *ms_inv_strchr_ptr_avx2_d64(const char *str, int ch)
#if defined(ADD_SOURCE_CODE) || defined(FORCE_INLINE)
{
    if (str == NULL)
    {
        return NULL;
    }

    const char *str_aligned = str + ALIGN_PAD64(str);

    while (str < str_aligned && *str && *str == ch)
    {
        str++;
    }

    if (*str == 0 || *str != ch)
    {
        return (char *)str;
    }

    __m256i ymm0 = _mm256_set1_epi8(ch);

    while (1)
    {
        __m256i ymm1 = _mm256_loadu_si256((__m256i *)(str +  0));
        __m256i ymm2 = _mm256_loadu_si256((__m256i *)(str + 32));
        uint32_t msk0 = ~_mm256_movemask_epi8(_mm256_cmpeq_epi8(ymm1, ymm0));
        uint32_t msk1 = ~_mm256_movemask_epi8(_mm256_cmpeq_epi8(ymm2, ymm0));
        uint32_t msk2 = _mm256_movemask_epi8(_mm256_cmpeq_epi8(ymm1, _mm256_setzero_si256()));
        uint32_t msk3 = _mm256_movemask_epi8(_mm256_cmpeq_epi8(ymm2, _mm256_setzero_si256()));
        if (msk0 | msk1 | msk2 | msk3)
        {
            str += _tzcnt_u64((size_t)(msk0 | msk2) | ((size_t)(msk1 | msk3) << 32ull));
            return (char *)str;
        }
        str += 64;
    }
}
#else
;
#endif
#endif


MSLIB_EXPORT char *ms_inv_strchr_ptr_x64(const char *str, int ch)
#if defined(ADD_SOURCE_CODE) || defined(FORCE_INLINE)
{
    if (str == NULL)
    {
        return NULL;
    }

    while (*str && *str == ch)
    {
        str++;
    }
    return (char *)str;
}
#else
;
#endif




#ifdef __AVX2__
    #ifdef USE_32_STEP_STRCHR
        #define ms_inv_strchr_ptr(str, ch) ms_inv_strchr_ptr_avx2_d32(str, ch)
    #else
        #define ms_inv_strchr_ptr(str, ch) ms_inv_strchr_ptr_avx2_d64(str, ch)
    #endif
#else
    #define ms_inv_strchr_ptr(str, ch) ms_inv_strchr_ptr_x64(str, ch)
#endif



#ifdef __cplusplus
}
#endif



#endif
