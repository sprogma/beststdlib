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



#define ALIGN_BEFORE_RUN
#define USE_GATHER_INSTRUCTION_IN_STRTOK



/*
    to see realizated function use
    gc ms_string.h | sls "MSLIB_EXPORT.*?ms"
*/

int ms_puts(const char *str);
size_t ms_getline(char **lineptr, size_t *n, FILE *stream);
char *ms_fgets(char *str, int count, FILE *stream);


const char *ms_strstr(const char *str, const char *str_search);




#define ALIGN_PAD(buf) (8 - (size_t)(buf) % 8)


MSLIB_EXPORT void ms_memclear(char *buf, size_t size)
#if defined(ADD_SOURCE_CODE) || defined(FORCE_INLINE)
{
    #ifdef __AVX2__
        #ifdef ALIGN_BEFORE_RUN
            const char *buf_aligned = buf + ALIGN_PAD(buf);
            while (buf < buf_aligned && *buf)
            {
                *buf++ = 0;
                size--;
            }
        #endif
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
        #ifdef ALIGN_BEFORE_RUN
            const char *dest_aligned = str + ALIGN_PAD(str);
            while (str < dest_aligned && *str)
            {
                str++;
            }
        #endif
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
        #ifdef ALIGN_BEFORE_RUN
            const char *dest_aligned = dest + ALIGN_PAD(dest);
            while (dest < dest_aligned && *src)
            {
                *dest++ = *src++;
            }
        #endif
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
        #ifdef ALIGN_BEFORE_RUN
            const char *dest_aligned = dest + ALIGN_PAD(dest);
            while (dest < dest_aligned && size)
            {
                *dest++ = *src++;
                --size;
            }
        #endif
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
        #ifdef ALIGN_BEFORE_RUN
            const char *dest_aligned = dest + ALIGN_PAD(dest);
            while (dest < dest_aligned && *src && size)
            {
                *dest++ = *src++;
                --size;
            }
        #endif
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
        #ifdef ALIGN_BEFORE_RUN
            const char *str_aligned = str + ALIGN_PAD(str);
            while (str < str_aligned && *str && *str != ch)
            {
                str++;
            }
        #endif
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
    #ifdef __WIN32
    #endif
    int size = ms_strlen(str) + 1;
    char *new_string = malloc(size);
    ms_memcpy(new_string, str, size);
    return new_string;
}
#else
;
#endif


MSLIB_EXPORT char *ms_strtok(char *str, const char *delimiters)
#if defined(ADD_SOURCE_CODE) || defined(FORCE_INLINE)
{    
    static char *current_pos = NULL;
    static const char *prev_delimiters = NULL;
    #if defined(__AVX2__) && defined(USE_GATHER_INSTRUCTION_IN_STRTOK)
        static uint32_t delimiters_table[256];
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
    else if (current_pos == NULL || *current_pos == '\0') 
    {
        return NULL;
    }

    #if defined(__AVX2__) && defined(USE_GATHER_INSTRUCTION_IN_STRTOK)
        uint32_t msk0, msk1;
        while (1)
        {
            __m256i ymm0 = _mm256_loadu_si256((__m256i *)(current_pos +  0));
            ymm0 = _mm256_cvtepi8_epi32(ymm0);
            __m256i ymm1 = _mm256_gather_epi32(ymm0);
            msk0 = _mm256_movemask_epi8(_mm256_cmpeq_epi8(ymm0));
            msk1 = _mm256_movemask_epi8(_mm256_cmpeq_epi8(ymm1));
            if (msk0 | msk1)
            {
                break;
            }
            current_pos += 64;
        }
        current_pos += _tzcnt_u64((size_t)msk0 | ((size_t)msk1 << 32ull));
    #else
        while (*current_pos != '\0' && delimiters_table[(unsigned char)*current_pos]) 
        {
            current_pos++;
        }
    #endif

    // If we've reached the end of the string after skipping delimiters
    if (*current_pos == '\0') 
    {
        return NULL;
    }

    char *token_start = current_pos;

    // Find the end of the token
    while (*current_pos != '\0' && !delimiters_table[(unsigned char)*current_pos]) 
    {
        current_pos++;
    }

    if (*current_pos != '\0') 
    {
        *current_pos++ = '\0';
    }

    return token_start;
}
#else
;
#endif


#ifdef __cplusplus
}
#endif

#endif
