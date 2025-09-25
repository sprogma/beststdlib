#ifndef MSLIB_STRING_STRTOK
#define MSLIB_STRING_STRTOK


#include "ms_string_common_headers.h"

#ifdef __AVX2__
    #include "ms_string_strchr_ptr.h"
    #include "ms_string_inv_strchr_ptr.h"
#endif

#include "ms_string_memclear.h"

#ifdef __cplusplus
extern "C"
{
#endif


/*
    strtok idea:

        1. make lookup table on INTs.
        2. fill it from delimiters with -1
        3. load 32 bytes -> convert 32epi8 -> 8epi32
        4. use vpgatherdd instruction -> search LUT.
        5. act as strchr.

        make 2 searches - first to skip delimiters, second - to find next delimiter

        if delimiter is one - fall to strchr.

*/
#ifdef __AVX2__
static _Thread_local char *avx2_gather_current_pos = NULL;
static _Thread_local uint32_t avx2_gather_delimiters_table[256];
static _Thread_local uint32_t avx2_gather_ms_strtok_use_strchr = 0;
MSLIB_EXPORT char *ms_strtok_avx2_gather(char * __restrict__ str, const char * __restrict__ delimiters)
#if defined(ADD_SOURCE_CODE) || defined(FORCE_INLINE)
{
    #ifndef STRTOK_DYNAMIC_DELIMITERS
    if (str != NULL)
    #endif
    {
        if (delimiters[1] != 0)
        {
            ms_memclear((char *)avx2_gather_delimiters_table, sizeof(avx2_gather_delimiters_table));

            avx2_gather_delimiters_table[0] = -1;

            const char *save = delimiters;
            while (*delimiters)
            {
                avx2_gather_delimiters_table[(unsigned char)*delimiters] = -1;
                delimiters++;
            }
            delimiters = save;
        }
    }
    if (str != NULL)
    {
        avx2_gather_current_pos = str;
    }
    else if (avx2_gather_current_pos == NULL || *avx2_gather_current_pos == 0)
    {
        // it is end
        return NULL;
    }

    if (delimiters[1] == 0)
    {
        avx2_gather_current_pos = ms_inv_strchr_ptr(avx2_gather_current_pos, *delimiters);
    }
    else
    {
        /* align position */
        {
            const char *aligned_pos = avx2_gather_current_pos + ALIGN_PAD8(avx2_gather_current_pos);

            while (avx2_gather_current_pos < aligned_pos &&
                  *avx2_gather_current_pos != 0 &&
                   avx2_gather_delimiters_table[(unsigned char)*avx2_gather_current_pos])
            {
                avx2_gather_current_pos++;
            }
        }

        if (*avx2_gather_current_pos != 0 && !avx2_gather_delimiters_table[(unsigned char)*avx2_gather_current_pos])
        {
            // TODO: unroll line to 64 byte fetch / parallel compuctation?
            while (1)
            {
                __m128i xmm0 = _mm_cvtsi64_si128(*(uint64_t *)(avx2_gather_current_pos));
                __m256i ymm0 = _mm256_cvtepu8_epi32(xmm0);
                __m256i ymm1 = _mm256_i32gather_epi32(avx2_gather_delimiters_table, ymm0, 4);
                uint32_t mskz0 = _mm256_movemask_epi8(_mm256_cmpeq_epi32(ymm0, _mm256_setzero_si256()));
                uint32_t mskd0 = ~_mm256_movemask_epi8(ymm1);
                if (mskd0 | mskz0)
                {
                    avx2_gather_current_pos += _tzcnt_u32(mskz0 | mskd0) / 4;
                    break;
                }
                avx2_gather_current_pos += 8;
            }
        }
    }

    // it is end
    if (*avx2_gather_current_pos == 0)
    {
        return NULL;
    }

    char *token_start = avx2_gather_current_pos;

    if (delimiters[1] == 0)
    {
        avx2_gather_current_pos = ms_strchr_ptr(avx2_gather_current_pos, *delimiters);
    }
    else
    {
        /* align position */
        {
            const char *aligned_pos = avx2_gather_current_pos + ALIGN_PAD8(avx2_gather_current_pos);

            while (avx2_gather_current_pos < aligned_pos &&
                  *avx2_gather_current_pos != 0 &&
                   !avx2_gather_delimiters_table[(unsigned char)*avx2_gather_current_pos])
            {
                avx2_gather_current_pos++;
            }
        }

        if (*avx2_gather_current_pos != 0 && !avx2_gather_delimiters_table[(unsigned char)*avx2_gather_current_pos])
        {
            // TODO: unroll line to 64 byte fetch / parallel compuctation?
            while (1)
            {
                __m128i xmm0 = _mm_cvtsi64_si128(*(uint64_t *)(avx2_gather_current_pos));
                __m256i ymm0 = _mm256_cvtepu8_epi32(xmm0);
                __m256i ymm1 = _mm256_i32gather_epi32(avx2_gather_delimiters_table, ymm0, 4);
                uint32_t mskz0 = _mm256_movemask_epi8(_mm256_cmpeq_epi32(ymm0, _mm256_setzero_si256()));
                uint32_t mskd0 = _mm256_movemask_epi8(ymm1);
                if (mskd0 | mskz0)
                {
                    avx2_gather_current_pos += _tzcnt_u32(mskz0 | mskd0) / 4;
                    break;
                }
                avx2_gather_current_pos += 8;
            }
        }
    }

    *avx2_gather_current_pos++ = 0;

    return token_start;
}
#else
;
#endif
#endif

#ifdef FORCE_INLINE
    _Thread_local char *x64_current_pos = NULL;
    _Thread_local uint8_t x64_delimiters_table[256];
#endif
MSLIB_EXPORT char *ms_strtok_x64(char * __restrict__ str, const char * __restrict__ delimiters)
#if defined(ADD_SOURCE_CODE) || defined(FORCE_INLINE)
{
    #ifndef FORCE_INLINE
        _Thread_local static char *x64_current_pos = NULL;
        _Thread_local static uint8_t x64_delimiters_table[256];
    #endif

    #ifndef STRTOK_DYNAMIC_DELIMITERS
    if (str != NULL)
    #endif
    {
        ms_memclear((char *)x64_delimiters_table, sizeof(x64_delimiters_table));
        const char *save = delimiters;
        while (*delimiters)
        {
            x64_delimiters_table[(unsigned char)*delimiters] = -1;
            delimiters++;
        }
        delimiters = save;
    }

    if (str != NULL)
    {
        x64_current_pos = str;
    }
    else if (x64_current_pos == NULL || *x64_current_pos == 0)
    {
        return NULL;
    }


    while (*x64_current_pos != 0 && x64_delimiters_table[(unsigned char)*x64_current_pos])
    {
        x64_current_pos++;
    }

    // If we've reached the end of the string after skipping delimiters
    if (*x64_current_pos == 0)
    {
        return NULL;
    }

    char *token_start = x64_current_pos;

    while (*x64_current_pos != 0 && !x64_delimiters_table[(unsigned char)*x64_current_pos])
    {
        x64_current_pos++;
    }

    *x64_current_pos++ = 0;

    return token_start;
}
#else
;
#endif


#if defined(__AVX2__) && defined(USE_GATHER_INSTRUCTION_IN_STRTOK)
    #define ms_strtok(str, delimiters) ms_strtok_avx2_gather(str, delimiters)
#else
    #define ms_strtok(str, delimiters) ms_strtok_x64(str, delimiters)
#endif




#ifdef __cplusplus
}
#endif



#endif
