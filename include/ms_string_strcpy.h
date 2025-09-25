#ifndef MSLIB_STRING_STRCPY
#define MSLIB_STRING_STRCPY


#include "ms_string_common_headers.h"


#ifdef __cplusplus
extern "C"
{
#endif


/*
    strcpy idea:

        align source.

        (unaligned write is slower than an unaligned read?)
        but cannot align dest becouse we search for zero in src.

        search for zero in src, and write data in destination.

        continue with simple loop after founding zero.

*/

#ifdef __AVX2__
MSLIB_EXPORT char *ms_strcpy_avx2_d32(char * __restrict__ dest, const char * __restrict__ src)
#if defined(ADD_SOURCE_CODE) || defined(FORCE_INLINE)
{
    if (dest == NULL || src == NULL)
    {
        return NULL;
    }

    char *dest_start = dest;

    const char *src_aligned = src + ALIGN_PAD32(src);

    while (src < src_aligned && *src)
    {
    //TODO: Add here duff's device ?
        *dest++ = *src++;
    }

    if (*src == 0)
    {
        *dest++ = 0;
        return dest_start;
    }

    while (1)
    {
        __m256i ymm0 = _mm256_loadu_si256((__m256i *)(src +  0));
        uint32_t msk0 = _mm256_movemask_epi8(_mm256_cmpeq_epi8(ymm0, _mm256_setzero_si256()));
        if (msk0)
        {
            break;
        }
        _mm256_storeu_si256((__m256i *)(dest +  0), ymm0);
        dest += 32;
        src  += 32;
    }

    while (*src)
    {
    //TODO: Add here duff's device ?
        *dest++ = *src++;
    }

    *dest++ = 0;

    return dest_start;
}
#else
;
#endif



MSLIB_EXPORT char *ms_strcpy_avx2_d64(char * __restrict__ dest, const char * __restrict__ src)
#if defined(ADD_SOURCE_CODE) || defined(FORCE_INLINE)
{
    if (dest == NULL || src == NULL)
    {
        return NULL;
    }

    char *dest_start = dest;

    const char *src_aligned = src + ALIGN_PAD64(src);

    while (src < src_aligned && *src)
    {
    //TODO: Add here duff's device ?
        *dest++ = *src++;
    }

    if (*src == 0)
    {
        *dest++ = 0;
        return dest_start;
    }

    while (1)
    {
        __m256i ymm0 = _mm256_loadu_si256((__m256i *)(src +  0));
        __m256i ymm1 = _mm256_loadu_si256((__m256i *)(src + 32));
        uint32_t msk0 = _mm256_movemask_epi8(_mm256_cmpeq_epi8(ymm0, _mm256_setzero_si256()));
        uint32_t msk1 = _mm256_movemask_epi8(_mm256_cmpeq_epi8(ymm1, _mm256_setzero_si256()));
        if (msk0 | msk1)
        {
            if (!msk0)
            {
                _mm256_storeu_si256((__m256i *)dest, ymm0);
                dest += 32;
                src  += 32;
            }
            break;
        }
        _mm256_storeu_si256((__m256i *)(dest +  0), ymm0);
        _mm256_storeu_si256((__m256i *)(dest + 32), ymm1);
        dest += 64;
        src  += 64;
    }

    while (*src)
    {
    //TODO: Add here duff's device ?
        *dest++ = *src++;
    }

    *dest++ = 0;

    return dest_start;
}
#else
;
#endif
#endif



MSLIB_EXPORT char *ms_strcpy_x64(char * __restrict__ dest, const char * __restrict__ src)
#if defined(ADD_SOURCE_CODE) || defined(FORCE_INLINE)
{
    if (dest == NULL || src == NULL)
    {
        return NULL;
    }

    char *dest_start = dest;

    while (*src)
    {
    //TODO: Add here duff's device ?
        *dest++ = *src++;
    }

    *dest++ = 0;

    return dest_start;
}
#else
;
#endif



#ifdef __AVX2__
    #ifdef USE_32_STEP_STRCPY
        #define ms_strcpy(dest, src) ms_strcpy_avx2_d32(dest, src)
    #else
        #define ms_strcpy(dest, src) ms_strcpy_avx2_d64(dest, src)
    #endif
#else
    #define ms_strcpy(dest, src) ms_strcpy_x64(dest, src)
#endif




#ifdef __cplusplus
}
#endif



#endif
