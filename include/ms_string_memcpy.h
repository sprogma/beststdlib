#ifndef MSLIB_STRING_MEMCPY
#define MSLIB_STRING_MEMCPY


#include "ms_string_common.h"


#ifdef __cplusplus
extern "C"
{
#endif



/*
    ms_memcpy idea:

        align destination
        (unaligned write is slower than an unaligned read?)

        copy data

        copy rest in simple cycle
*/
#ifdef __AVX2__
MSLIB_EXPORT char *ms_memcpy_avx2_d32(char * __restrict__ dest, const char * __restrict__ src, size_t size)
#if defined(ADD_SOURCE_CODE) || defined(FORCE_INLINE)
{
    char *dest_start = dest;

    const char *dest_aligned = dest + ALIGN_PAD32(dest);

    while (dest < dest_aligned && size)
    {
    //TODO: Add here long long copy ?
        *dest++ = *src++;
        --size;
    }

    if (size == 0)
    {
        return dest_start;
    }

    /* align by 32 */
    size_t aligned_size = size & (~0x1F);

    while (aligned_size != 0)
    {
        __m256i ymm0 = _mm256_loadu_si256((__m256i *)src);
        _mm256_storeu_si256((__m256i *)dest, ymm0);
        dest += 32;
        src  += 32;
        aligned_size -= 32;
    }

    size &= 0x1F;

    while (size)
    {
    //TODO: Add here long long copy ?
        *dest++ = *src++;
        --size;
    }

    return dest_start;
}
#else
;
#endif




MSLIB_EXPORT char *ms_memcpy_avx2_d64(char * __restrict__ dest, const char * __restrict__ src, size_t size)
#if defined(ADD_SOURCE_CODE) || defined(FORCE_INLINE)
{
    char *dest_start = dest;

    const char *dest_aligned = dest + ALIGN_PAD64(dest);

    while (dest < dest_aligned && size)
    {
    //TODO: Add here long long copy ?
        *dest++ = *src++;
        --size;
    }

    if (size == 0)
    {
        return dest_start;
    }

    /* align by 64 */
    size_t aligned_size = size & (~0x3F);

    while (aligned_size != 0)
    {
        __m256i ymm0 = _mm256_loadu_si256((__m256i *)(src +  0));
        __m256i ymm1 = _mm256_loadu_si256((__m256i *)(src + 32));
        _mm256_storeu_si256((__m256i *)(dest +  0), ymm0);
        _mm256_storeu_si256((__m256i *)(dest + 32), ymm1);
        dest += 64;
        src  += 64;
        aligned_size -= 64;
    }

    size &= 0x3F;

    while (size)
    {
    //TODO: Add here long long copy ?
        *dest++ = *src++;
        --size;
    }
    return dest_start;
}
#else
;
#endif
#endif


MSLIB_EXPORT char *ms_memcpy_x64(char * __restrict__ dest, const char * __restrict__ src, size_t size)
#if defined(ADD_SOURCE_CODE) || defined(FORCE_INLINE)
{
    char *dest_start = dest;

    while (size)
    {
    //TODO: Add here long long copy ?
        *dest++ = *src++;
        --size;
    }

    return dest_start;
}
#else
;
#endif



#ifdef __AVX2__
    #ifdef USE_32_STEP_MEMCPY
        #define ms_memcpy(dest, src, size) ms_memcpy_avx2_d32(dest, src, size)
    #else
        #define ms_memcpy(dest, src, size) ms_memcpy_avx2_d64(dest, src, size)
    #endif
#else
    #define ms_memcpy(dest, src, size) ms_memcpy_x64(dest, src, size)
#endif





#ifdef __cplusplus
}
#endif



#endif