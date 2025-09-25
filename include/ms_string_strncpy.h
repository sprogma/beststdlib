#ifndef MSLIB_STRING_STRNCPY
#define MSLIB_STRING_STRNCPY


#include "ms_string_common_headers.h"

#include "ms_string_memclear.h"


#ifdef __cplusplus
extern "C"
{
#endif


/*
    idea: as strcpy, but also use count.
        (merge strcpy and memcpy ideas)
*/
#ifdef __AVX2__
MSLIB_EXPORT char *ms_strncpy_avx2_d32(char * __restrict__ dest, const char * __restrict__ src, size_t size)
#if defined(ADD_SOURCE_CODE) || defined(FORCE_INLINE)
{
    if (dest == NULL || src == NULL)
    {
        return NULL;
    }

    char *dest_start = dest;

    const char *src_aligned = src + ALIGN_PAD32(src);

    while (src < src_aligned && *src && size)
    {
    //TODO: Add here duff's device ?
        *dest++ = *src++;
        --size;
    }

    if (*src == 0 || size == 0)
    {
        return dest_start;
    }

    /* align by 32 */
    size_t aligned_size = size & (~0x1F);

    while (aligned_size != 0)
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
        aligned_size -= 32;
    }

    size &= 0x1F;

    while (*src && size)
    {
    //TODO: Add here duff's device ?
        *dest++ = *src++;
        --size;
    }

    if (size)
    {
        ms_memclear(dest, size);
    }
    return dest_start;
}
#else
;
#endif



MSLIB_EXPORT char *ms_strncpy_avx_d64(char * __restrict__ dest, const char * __restrict__ src, size_t size)
#if defined(ADD_SOURCE_CODE) || defined(FORCE_INLINE)
{
    if (dest == NULL || src == NULL)
    {
        return NULL;
    }

    char *dest_start = dest;

    const char *src_aligned = src + ALIGN_PAD64(src);

    while (src < src_aligned && *src && size)
    {
    //TODO: Add here duff's device ?
        *dest++ = *src++;
        --size;
    }

    if (*src == 0 || size == 0)
    {
        return dest_start;
    }

    size_t aligned_size = size & (~0x3F);
    while (aligned_size != 0)
    {
        __m256i ymm0 = _mm256_loadu_si256((__m256i *)(src +  0));
        __m256i ymm1 = _mm256_loadu_si256((__m256i *)(src + 32));
        uint32_t msk0 = _mm256_movemask_epi8(_mm256_cmpeq_epi8(ymm0, _mm256_setzero_si256()));
        uint32_t msk1 = _mm256_movemask_epi8(_mm256_cmpeq_epi8(ymm1, _mm256_setzero_si256()));
        if (msk0 | msk1)
        {
            break;
        }
        _mm256_storeu_si256((__m256i *)(dest +  0), ymm0);
        _mm256_storeu_si256((__m256i *)(dest + 32), ymm1);
        dest += 64;
        src  += 64;
        aligned_size -= 64;
    }
    size &= 0x3F;

    while (*src && size)
    {
    //TODO: Add here duff's device ?
        *dest++ = *src++;
        --size;
    }

    if (size)
    {
        ms_memclear(dest, size);
    }

    return dest_start;
}
#else
;
#endif
#endif



MSLIB_EXPORT char *ms_strncpy_x64(char * __restrict__ dest, const char * __restrict__ src, size_t size)
#if defined(ADD_SOURCE_CODE) || defined(FORCE_INLINE)
{
    if (dest == NULL || src == NULL)
    {
        return NULL;
    }

    char *dest_start = dest;

    while (*src && size)
    {
    //TODO: Add here duff's device ?
        *dest++ = *src++;
        --size;
    }

    if (size)
    {
        ms_memclear(dest, size);
    }

    return dest_start;
}
#else
;
#endif



#ifdef __AVX2__
    #ifdef USE_32_STEP_STRNCPY
        #define ms_strncpy(dest, src, size) ms_strncpy_avx2_d32(dest, src, size)
    #else
        #define ms_strncpy(dest, src, size) ms_strncpy_avx2_d64(dest, src, size)
    #endif
#else
    #define ms_strncpy(dest, src, size) ms_strncpy_x64(dest, src, size)
#endif




#ifdef __cplusplus
}
#endif



#endif
