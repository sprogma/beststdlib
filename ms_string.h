#ifndef MSLIB_STRING
#define MSLIB_STRING

#include "inttypes.h"
#include "stdio.h"
#include "immintrin.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define ADD_SOURCE_CODE

#ifdef FORCE_INLINE
    #define MSLIB_EXPORT __attribute__((always_inline)) __inline__
#else
    #define MSLIB_EXPORT
#endif



#define USE_GATHER_INSTRUCTION_IN_STRTOK
#define USE_MANY_LOADING_IN_STRSTR_32



/*
    to see realizated function use
    gc ms_string.h | sls "MSLIB_EXPORT.*?ms"
*/

int ms_puts(const char *str);
size_t ms_getline(char **lineptr, size_t *n, FILE *stream);
char *ms_fgets(char *str, int count, FILE *stream);



#define ALIGN_PAD(buf) (8 - (size_t)(buf) % 8)


MSLIB_EXPORT void ms_memclear(char *buf, size_t size)
#if defined(ADD_SOURCE_CODE) || defined(FORCE_INLINE)
{
    #ifdef __AVX2__
        const char *buf_aligned = buf + ALIGN_PAD(buf);
        while (buf < buf_aligned && *buf)
        {
            *buf++ = 0;
            size--;
        }
        size_t aligned_size = size & (~0x3F);
        while (aligned_size != 0)
        {
            _mm256_storeu_si256((__m256i *)(buf +  0), _mm256_setzero_si256());
            _mm256_storeu_si256((__m256i *)(buf + 32), _mm256_setzero_si256());
            buf += 64;
            aligned_size -= 64;
        }
        size &= 0x3F;
    #endif
    while (size)
    {
        *buf++ = 0;
        size--;
    }
}
#else
;
#endif


MSLIB_EXPORT char *ms_strend(const char *str)
#if defined(ADD_SOURCE_CODE) || defined(FORCE_INLINE)
{
    #ifdef __AVX2__
        const char *dest_aligned = str + ALIGN_PAD(str);
        while (str < dest_aligned && *str)
        {
            str++;
        }
        uint32_t msk0, msk1;
        while (1)
        {
            __m256i ymm0 = _mm256_loadu_si256((__m256i *)(str +  0));
            __m256i ymm1 = _mm256_loadu_si256((__m256i *)(str + 32));
            msk0 = _mm256_movemask_epi8(_mm256_cmpeq_epi8(ymm0, _mm256_setzero_si256()));
            msk1 = _mm256_movemask_epi8(_mm256_cmpeq_epi8(ymm1, _mm256_setzero_si256()));
            if (msk0 | msk1)
            {
                break;
            }
            str += 64;
        }
        return (char *)str + _tzcnt_u64((size_t)msk0 | ((size_t)msk1 << 32ull));
    #else
        while (*str)
        {
            str++;
        }
        return (char *)str;
    #endif
}
#else
;
#endif



MSLIB_EXPORT char *ms_strcpy(char *dest, const char *src)
#if defined(ADD_SOURCE_CODE) || defined(FORCE_INLINE)
{
    char *dest_start = dest;
    #ifdef __AVX2__
        const char *dest_aligned = dest + ALIGN_PAD(dest);
        while (dest < dest_aligned && *src)
        {
            *dest++ = *src++;
        }
        while (1)
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
        }
    #endif
    while (*src)
    {
        *dest++ = *src++;
    }
    *dest++ = 0;
    return dest_start;
}
#else
;
#endif



MSLIB_EXPORT char *ms_memcpy(char *dest, const char *src, size_t size)
#if defined(ADD_SOURCE_CODE) || defined(FORCE_INLINE)
{
    char *dest_start = dest;
    #ifdef __AVX2__
        const char *dest_aligned = dest + ALIGN_PAD(dest);
        while (dest < dest_aligned && size)
        {
            *dest++ = *src++;
            --size;
        }
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
    #endif
    while (size)
    {
        *dest++ = *src++;
        --size;
    }
    return dest_start;
}
#else
;
#endif



MSLIB_EXPORT char *ms_strncpy(char *dest, const char *src, size_t size)
#if defined(ADD_SOURCE_CODE) || defined(FORCE_INLINE)
{
    char *dest_start = dest;
    #ifdef __AVX2__
        const char *dest_aligned = dest + ALIGN_PAD(dest);
        while (dest < dest_aligned && *src && size)
        {
            *dest++ = *src++;
            --size;
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
    #endif
    while (*src && size)
    {
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



MSLIB_EXPORT char *ms_strcat(char *dest, const char *src)
#if defined(ADD_SOURCE_CODE) || defined(FORCE_INLINE)
{
    ms_strcpy(ms_strend(dest), src);
    return dest;
}
#else
;
#endif



MSLIB_EXPORT char *ms_strncat(char *dest, const char *src, size_t count)
#if defined(ADD_SOURCE_CODE) || defined(FORCE_INLINE)
{
    ms_strncpy(ms_strend(dest), src, count);
    return dest;
}
#else
;
#endif




MSLIB_EXPORT char *ms_strchr(const char *str, int ch)
#if defined(ADD_SOURCE_CODE) || defined(FORCE_INLINE)
{
    #ifdef __AVX2__
        const char *str_aligned = str + ALIGN_PAD(str);
        while (str < str_aligned && *str && *str != ch)
        {
            str++;
        }
        __m256i ymm0 = _mm256_set1_epi8(ch);
        uint32_t msk0, msk1, msk2, msk3;
        while (1)
        {
            __m256i ymm1 = _mm256_loadu_si256((__m256i *)(str +  0));
            __m256i ymm2 = _mm256_loadu_si256((__m256i *)(str + 32));
            msk0 = _mm256_movemask_epi8(_mm256_cmpeq_epi8(ymm1, ymm0));
            msk1 = _mm256_movemask_epi8(_mm256_cmpeq_epi8(ymm2, ymm0));
            msk2 = _mm256_movemask_epi8(_mm256_cmpeq_epi8(ymm1, _mm256_setzero_si256()));
            msk3 = _mm256_movemask_epi8(_mm256_cmpeq_epi8(ymm2, _mm256_setzero_si256()));
            if (msk0 | msk1 | msk2 | msk3)
            {
                break;
            }
            str += 64;
        }
        str += _tzcnt_u64((size_t)(msk0 | msk2) | ((size_t)(msk1 | msk3) << 32ull));
        return (!*str && ch ? NULL : (char *)str);
    #else
        while (*str && *str != ch)
        {
            str++;
        }
        return (!*str && ch ? NULL : (char *)str);
    #endif
}
#else
;
#endif



MSLIB_EXPORT size_t ms_strlen(const char *str)
#if defined(ADD_SOURCE_CODE) || defined(FORCE_INLINE)
{
    return ms_strend(str) - str;
}
#else
;
#endif




MSLIB_EXPORT char *ms_strdup(const char *str)
#if defined(ADD_SOURCE_CODE) || defined(FORCE_INLINE)
{
    int size = ms_strlen(str) + 1;
    char *new_string = malloc(size);
    ms_memcpy(new_string, str, size);
    return new_string;
}
#else
;
#endif


#ifdef FORCE_INLINE
    _Thread_local char *current_pos = NULL;
    _Thread_local const char *prev_delimiters = NULL;
    #if defined(__AVX2__) && defined(USE_GATHER_INSTRUCTION_IN_STRTOK)
        _Thread_local uint32_t delimiters_table[256];
    #endif
#endif
MSLIB_EXPORT char *ms_strtok(char *str, const char *delimiters)
#if defined(ADD_SOURCE_CODE) || defined(FORCE_INLINE)
{    
    #ifndef FORCE_INLINE
        _Thread_local static char *current_pos = NULL;
        _Thread_local static const char *prev_delimiters = NULL;
        #if defined(__AVX2__) && defined(USE_GATHER_INSTRUCTION_IN_STRTOK)
            _Thread_local static uint32_t delimiters_table[256];
        #endif
    #endif
    #if defined(__AVX2__) && defined(USE_GATHER_INSTRUCTION_IN_STRTOK)
        if (prev_delimiters != delimiters)
        {
            ms_memclear((char *)delimiters_table, sizeof(delimiters_table));
            prev_delimiters = delimiters;
            while (*delimiters)
            {
                delimiters_table[(unsigned char)*delimiters] = -1;
                delimiters++;
            }
            delimiters = prev_delimiters;
        }
    #else
        static unsigned char delimiters_table[256];
        if (prev_delimiters != delimiters)
        {
            ms_memclear((char *)delimiters_table, sizeof(delimiters_table));
            prev_delimiters = delimiters;
            while (*delimiters)
            {
                delimiters_table[(unsigned char)*delimiters] = -1;
                delimiters++;
            }
            delimiters = prev_delimiters;
        }
    #endif
    

    if (str != NULL) 
    {
        current_pos = str;
    } 
    else if (current_pos == NULL || *current_pos == 0) 
    {
        return NULL;
    }

    /* parsing delimiters, often short string i think? */
    #if defined(__AVX2__) && defined(USE_GATHER_INSTRUCTION_IN_STRTOK)
    {
        // TODO: unroll line to 64 byte fetch / parallel compuctation?
        uint32_t mskz0, mskd0;
        while (1)
        {
            __m128i xmm0 = _mm_loadu_si128((__m128i *)(current_pos));
            __m256i ymm0 = _mm256_cvtepu8_epi32(xmm0);
            __m256i ymm1 = _mm256_i32gather_epi32(delimiters_table, ymm0, 4);
            mskz0 = _mm256_movemask_epi8(_mm256_cmpeq_epi32(ymm0, _mm256_setzero_si256()));
            mskd0 = ~_mm256_movemask_epi8(ymm1);
            if (mskd0 | mskz0)
            {
                break;
            }
            current_pos += 8;
        }
        current_pos += _tzcnt_u32(mskz0 | mskd0) / 4;
    }
    #else
        while (*current_pos != 0 && delimiters_table[(unsigned char)*current_pos]) 
        {
            current_pos++;
        }
    #endif

    // If we've reached the end of the string after skipping delimiters
    if (*current_pos == 0) 
    {
        return NULL;
    }

    char *token_start = current_pos;

    /* parsing token, often longer string i think? */
    #if defined(__AVX2__) && defined(USE_GATHER_INSTRUCTION_IN_STRTOK)
    {
        // TODO: unroll line to 64 byte fetch / parallel compuctation?
        uint32_t mskz0, mskd0;
        while (1)
        {
            __m128i xmm0 = _mm_loadu_si128((__m128i *)(current_pos));
            __m256i ymm0 = _mm256_cvtepu8_epi32(xmm0);
            __m256i ymm1 = _mm256_i32gather_epi32(delimiters_table, ymm0, 4);
            mskz0 = _mm256_movemask_epi8(_mm256_cmpeq_epi32(ymm0, _mm256_setzero_si256()));
            mskd0 = _mm256_movemask_epi8(ymm1);
            if (mskd0 | mskz0)
            {
                break;
            }
            current_pos += 8;
        }
        current_pos += _tzcnt_u32(mskz0 | mskd0) / 4;
    }
    #else
        while (*current_pos != 0 && !delimiters_table[(unsigned char)*current_pos]) 
        {
            current_pos++;
        }
    #endif

    // TODO: See, does this check not slow down code?
    if (*current_pos != 0) 
    {
        *current_pos++ = 0;
    }

    return token_start;
}
#else
;
#endif


/* internal functions */

#ifdef __AVX2__
MSLIB_EXPORT const char *_ms_strstr_search_32_chars(const char *str, __m256i search, size_t length)
{
    /* load two aligned blocks : current and next */
    const char *aligned = (const char *)((uintptr_t)str & 0x1F);
    __m256i ymm0, ymm1;

    #ifdef USE_MANY_LOADING_IN_STRSTR_32    
        const __m256i reverse_shuffle = _mm256_set_epi8(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15);
        const __m256i blend_mask = _mm256_setr_epi8(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xFF);
        #define SHIFTL(ymm) \
            ymm = _mm256_alignr_epi8(_mm256_permute2x128_si256(ymm, ymm, _MM_SHUFFLE(2, 0, 0, 1)), ymm, 1)
        #define SHIFTR(ymm) \
            ymm = _mm256_alignr_epi8(ymm, _mm256_permute2x128_si256(ymm, ymm, _MM_SHUFFLE(0, 0, 2, 0)), 15)
        #define REVERSE(ymm) \
            ymm = _mm256_permute4x64_epi64(_mm256_shuffle_epi8(ymm, reverse_shuffle), _MM_SHUFFLE(1, 0, 3, 2));
            
        ymm0 = _mm256_loadu_si256((__m256i *)(str +  0));
        ymm1 = _mm256_loadu_si256((__m256i *)(str + 32));
        REVERSE(ymm1);

        // printf(">%s : %s\n", str);
        SHIFTL(ymm0);
        ymm0 = _mm256_blendv_epi8(ymm0, ymm1, blend_mask);
        // _mm256_storeu_si256((__m256i *)str, ymm0);
        SHIFTR(ymm1);
        // printf(">%s\n", str);
    #else
    #endif

    char str_search_buf[33] = {}, *str_search = str_search_buf;
    _mm256_storeu_si256((__m256i *)str_search, search);

    /* naive algo */
    while (*str != 0)
    {
        /* match */
        const char *match = str;
        str_search = str_search_buf;
        while (*str_search && *match && *match == *str_search)
        {
            match++;
            str_search++;
        }
        if (*str_search == 0)
        {
            return str;
        }
        str++;
    }
    
    return NULL;
}
#endif

MSLIB_EXPORT const char *ms_strstr(const char *str, const char *str_search)
#if defined(ADD_SOURCE_CODE) || defined(FORCE_INLINE)
{
    if (*str_search == 0)
    {
        return str;
    }
    if (str_search[1] == 0)
    {
        return ms_strchr(str, *str_search);
    }
    /* length of str_search <= 32 */
    #ifdef __AVX2__
    {
        /* check str_search 32 bits: */
        char buf[32] = {};
        size_t search_length = 0;
        while (search_length <= 32 && str_search[search_length])
        {
            buf[search_length] = str_search[search_length];
            search_length++;
        }
        __m256i search_ymm = _mm256_loadu_si256((__m256i *)buf);
        if (search_length <= 32)
        {
            return _ms_strstr_search_32_chars(str, search_ymm, search_length);
        }
    }
    #endif
    /* naive algo */
    const char *str_search_start = str_search;
    while (*str != 0)
    {
        /* match */
        str_search = str_search_start;
        const char *match = str;
        while (*str_search && *match && *match == *str_search)
        {
            match++;
            str_search++;
        }
        if (*str_search == 0)
        {
            return str;
        }
        str++;
    }
    
    return NULL;
}
#else
;
#endif


#ifdef __cplusplus
}
#endif

#endif
