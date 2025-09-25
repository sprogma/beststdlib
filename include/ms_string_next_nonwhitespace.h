#ifndef MSLIB_STRING_NEXT_NONWHITESPACE
#define MSLIB_STRING_NEXT_NONWHITESPACE


#include "ms_string_common_headers.h"


#ifdef __cplusplus
extern "C"
{
#endif


/*
    nonwhitespace idea:

        search for next not whitespace letter.

        load 32/64 bytes + check on not space (>32) and on null (==0)

        gather bytes mask

        break if found + return index of first 1 in mask
*/
#ifdef __AVX2__
MSLIB_EXPORT char *next_nonwhitespace_avx2_d32(const char *str)
#if defined(ADD_SOURCE_CODE) || defined(FORCE_INLINE)
{
    if (str == NULL)
    {
        return NULL;
    }

    const char *dest_aligned = str + ALIGN_PAD32(str);

    /* 32 is ' ' code, and all isspace chars is less than 32,
       use this to make isspace() by one comparsion */
    while (str < dest_aligned && *str && *str <= ' ')
    {
        str++;
    }

    if (*str == 0 || *str > ' ')
    {
        return (char *)str;
    }

    __m256i ymm_whitespace = _mm256_set1_epi8(' ');
    while (1)
    {
        __m256i ymm0 = _mm256_loadu_si256((__m256i *)(str +  0));
        uint32_t msk0  = _mm256_movemask_epi8(_mm256_cmpgt_epi8(ymm0, ymm_whitespace));
        uint32_t mskz0 = _mm256_movemask_epi8(_mm256_cmpeq_epi8(ymm0, _mm256_setzero_si256()));
        if (msk0 | mskz0)
        {
            return (char *)str + _tzcnt_u32((size_t)(msk0 | mskz0));
        }
        str += 32;
    }
}
#else
;
#endif


MSLIB_EXPORT char *next_nonwhitespace_avx2_d64(const char *str)
#if defined(ADD_SOURCE_CODE) || defined(FORCE_INLINE)
{
    if (str == NULL)
    {
        return NULL;
    }

    const char *dest_aligned = str + ALIGN_PAD64(str);

    /* 32 is ' ' code, and all isspace chars is less than 32,
       use this to make isspace() by one comparsion */
    while (str < dest_aligned && *str && *str <= ' ')
    {
        str++;
    }

    if (*str == 0 || *str > ' ')
    {
        return (char *)str;
    }

    __m256i ymm_whitespace = _mm256_set1_epi8(' ');

    while (1)
    {
        __m256i ymm0 = _mm256_loadu_si256((__m256i *)(str +  0));
        __m256i ymm1 = _mm256_loadu_si256((__m256i *)(str + 32)); // TODO: does this actually impact perfomance?
        uint32_t msk0 = _mm256_movemask_epi8(_mm256_cmpgt_epi8(ymm0, ymm_whitespace));
        uint32_t msk1 = _mm256_movemask_epi8(_mm256_cmpgt_epi8(ymm1, ymm_whitespace));
        uint32_t mskz0 = _mm256_movemask_epi8(_mm256_cmpeq_epi8(ymm0, _mm256_setzero_si256()));
        uint32_t mskz1 = _mm256_movemask_epi8(_mm256_cmpeq_epi8(ymm1, _mm256_setzero_si256()));
        if (msk0 | msk1 | mskz0 | mskz1)
        {
            return (char *)str + _tzcnt_u64((size_t)(msk0 | mskz0) | ((size_t)(msk1 | mskz1) << 32ull));
        }
        str += 64;
    }
}
#else
;
#endif
#endif


MSLIB_EXPORT char *next_nonwhitespace_x64(const char *str)
#if defined(ADD_SOURCE_CODE) || defined(FORCE_INLINE)
{
    if (str == NULL)
    {
        return NULL;
    }

    while (*str && *str <= ' ')
    {
        str++;
    }

    return (char *)str;
}
#else
;
#endif



#ifdef __AVX2__
    #ifdef USE_32_STEP_NEXT_NONWHITESPACE
        #define next_nonwhitespace(str) next_nonwhitespace_avx2_d32(str)
    #else
        #define next_nonwhitespace(str) next_nonwhitespace_avx2_d64(str)
    #endif
#else
    #define next_nonwhitespace(str) next_nonwhitespace_x64(str)
#endif




#ifdef __cplusplus
}
#endif



#endif
