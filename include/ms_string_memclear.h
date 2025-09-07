#ifndef MSLIB_STRING_MEMCLEAR
#define MSLIB_STRING_MEMCLEAR


#include "ms_string_common.h"


#ifdef __cplusplus
extern "C"
{
#endif


/*
    memclear idea:

        align pointer

        vectorized put zero in memory (step 32/64)

        continue simple loop to set bytes.
*/

#ifdef __AVX2__
MSLIB_EXPORT void ms_memclear_avx2_d32(char *buf, size_t size)
#if defined(ADD_SOURCE_CODE) || defined(FORCE_INLINE)
{
    const char *buf_aligned = buf + ALIGN_PAD32(buf);
    while (buf < buf_aligned && *buf)
    {
    //TODO: Add here duff's device ?
        *buf++ = 0;
        size--;
    }

    /* align by 32 */
    size_t aligned_size = size & (~0x1F);

    while (aligned_size != 0)
    {
        _mm256_storeu_si256((__m256i *)(buf +  0), _mm256_setzero_si256());
        buf += 32;
        aligned_size -= 32;
    }

    size &= 0x1F;

    while (size)
    {
    //TODO: Add here duff's device ?
        *buf++ = 0;
        size--;
    }
}
#else
;
#endif


MSLIB_EXPORT void ms_memclear_avx2_d64(char *buf, size_t size)
#if defined(ADD_SOURCE_CODE) || defined(FORCE_INLINE)
{
    const char *buf_aligned = buf + ALIGN_PAD64(buf);
    while (buf < buf_aligned && *buf)
    {
    //TODO: Add here duff's device ?
        *buf++ = 0;
        size--;
    }

    /* align by 64 */
    size_t aligned_size = size & (~0x3F);

    while (aligned_size != 0)
    {
        _mm256_storeu_si256((__m256i *)(buf +  0), _mm256_setzero_si256());
        _mm256_storeu_si256((__m256i *)(buf + 32), _mm256_setzero_si256());
        buf += 64;
        aligned_size -= 64;
    }

    size &= 0x3F;

    while (size)
    {
    //TODO: Add here duff's device ?
        *buf++ = 0;
        size--;
    }
}
#else
;
#endif
#endif


MSLIB_EXPORT void ms_memclear_x64(char *buf, size_t size)
#if defined(ADD_SOURCE_CODE) || defined(FORCE_INLINE)
{
    while (size)
    {
    //TODO: Add here duff's device ?
        *buf++ = 0;
        size--;
    }
}
#else
;
#endif


#ifdef __AVX2__
    #ifdef USE_32_STEP_MEMCLEAR
        #define ms_memclear(buf, size) ms_memclear_avx2_d32(buf, size)
    #else
        #define ms_memclear(buf, size) ms_memclear_avx2_d64(buf, size)
    #endif
#else
    #define ms_memclear(buf, size) ms_memclear_x64(buf, size)
#endif


#ifdef __cplusplus
}
#endif



#endif