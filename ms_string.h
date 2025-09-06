#ifndef MSLIB_STRING
#define MSLIB_STRING

#include "inttypes.h"
#include "errno.h"
#include "limits.h"
#include "stdio.h"
#include "immintrin.h"

#ifdef __WIN32
    #include "windows.h"
#endif


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
#define USE_32_STEP_STREND
#define USE_32_STEP_NEXT_NONWHITESPACE
#define USE_32_STEP_MEMCLEAR
#define USE_32_STEP_MEMCPY
#define USE_32_STEP_STRCPY
#define USE_32_STEP_STRNCPY
#define USE_32_STEP_STRCHR



/*

    functions database:

        [X] puts
        [X] strchr, strlen
        [X] strcat, strncat
        [X] atoi
        [X] fgets
        [X] strdup / * there is some optimizations, witch requres using ms_free instead of free? * /
        [X] getline
*/


/*
    to see realizated function use
    gc ms_string.h | sls "MSLIB_EXPORT.*?ms"
*/




#define ALIGN_PAD8(buf)  ( 8 - (size_t)(buf) %  8)
#define ALIGN_PAD32(buf) (32 - (size_t)(buf) % 32)
#define ALIGN_PAD64(buf) (64 - (size_t)(buf) % 64)




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
    char *dest_start = dest;
    
    const char *src_aligned = src + ALIGN_PAD32(src);
    
    while (src < src_aligned && *src)
    {
    //TODO: Add here duff's device ?
        *dest++ = *src++;
    }

    if (*src == 0)
    {
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
    char *dest_start = dest;

    const char *src_aligned = src + ALIGN_PAD64(src);
    
    while (src < src_aligned && *src)
    {
    //TODO: Add here duff's device ?
        *dest++ = *src++;
    }

    if (*src == 0)
    {
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





#ifdef __AVX2__
MSLIB_EXPORT char *ms_strncpy_avx2_d32(char * __restrict__ dest, const char * __restrict__ src, size_t size)
#if defined(ADD_SOURCE_CODE) || defined(FORCE_INLINE)
{
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




MSLIB_EXPORT char *ms_strcat(char * __restrict__ dest, const char * __restrict__ src)
#if defined(ADD_SOURCE_CODE) || defined(FORCE_INLINE)
{
    ms_strcpy(ms_strend(dest), src);
    return dest;
}
#else
;
#endif




MSLIB_EXPORT char *ms_strncat(char * __restrict__ dest, const char * __restrict__ src, size_t count)
#if defined(ADD_SOURCE_CODE) || defined(FORCE_INLINE)
{
    ms_strncpy(ms_strend(dest), src, count);
    return dest;
}
#else
;
#endif




/*
    strchar idea:
        
        like strend, but check both zero and given char
*/
#ifdef __AVX2__
MSLIB_EXPORT char *ms_strchr_avx2_d32(const char *str, int ch)
#if defined(ADD_SOURCE_CODE) || defined(FORCE_INLINE)
{
    const char *str_aligned = str + ALIGN_PAD32(str);

    while (str < str_aligned && *str && *str != ch)
    {
        str++;
    }

    if (*str == 0 || *str == ch)
    {
        return (!*str && ch ? NULL : (char *)str);
    }

    __m256i ymm0 = _mm256_set1_epi8(ch);

    while (1)
    {
        __m256i ymm1 = _mm256_loadu_si256((__m256i *)(str +  0));
        uint32_t msk0 = _mm256_movemask_epi8(_mm256_cmpeq_epi8(ymm1, ymm0));
        uint32_t msk1 = _mm256_movemask_epi8(_mm256_cmpeq_epi8(ymm1, _mm256_setzero_si256()));
        if (msk0 | msk1)
        {
            str += _tzcnt_u32(msk0 | msk1);
            return (!*str && ch ? NULL : (char *)str);
        }
        str += 32;
    }
}
#else
;
#endif


MSLIB_EXPORT char *ms_strchr_avx2_d64(const char *str, int ch)
#if defined(ADD_SOURCE_CODE) || defined(FORCE_INLINE)
{
    const char *str_aligned = str + ALIGN_PAD64(str);

    while (str < str_aligned && *str && *str != ch)
    {
        str++;
    }

    if (*str == 0 || *str == ch)
    {
        return (!*str && ch ? NULL : (char *)str);
    }

    __m256i ymm0 = _mm256_set1_epi8(ch);

    while (1)
    {
        __m256i ymm1 = _mm256_loadu_si256((__m256i *)(str +  0));
        __m256i ymm2 = _mm256_loadu_si256((__m256i *)(str + 32));
        uint32_t msk0 = _mm256_movemask_epi8(_mm256_cmpeq_epi8(ymm1, ymm0));
        uint32_t msk1 = _mm256_movemask_epi8(_mm256_cmpeq_epi8(ymm2, ymm0));
        uint32_t msk2 = _mm256_movemask_epi8(_mm256_cmpeq_epi8(ymm1, _mm256_setzero_si256()));
        uint32_t msk3 = _mm256_movemask_epi8(_mm256_cmpeq_epi8(ymm2, _mm256_setzero_si256()));
        if (msk0 | msk1 | msk2 | msk3)
        {
            str += _tzcnt_u64((size_t)(msk0 | msk2) | ((size_t)(msk1 | msk3) << 32ull));
            return (!*str && ch ? NULL : (char *)str);
        }
        str += 64;
    }
}
#else
;
#endif
#endif


MSLIB_EXPORT char *ms_strchr_x64(const char *str, int ch)
#if defined(ADD_SOURCE_CODE) || defined(FORCE_INLINE)
{
    while (*str && *str != ch)
    {
        str++;
    }
    return (!*str && ch ? NULL : (char *)str);
}
#else
;
#endif




#ifdef __AVX2__
    #ifdef USE_32_STEP_STRCHR
        #define ms_strchr(str, ch) ms_strchr_avx2_d32(str, ch)
    #else
        #define ms_strchr(str, ch) ms_strchr_avx2_d64(str, ch)
    #endif
#else
    #define ms_strchr(str, ch) ms_strchr_x64(str, ch)
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



#ifdef WIN32
MSLIB_EXPORT int ms_puts_win(const char *str)
#if defined(ADD_SOURCE_CODE) || defined(FORCE_INLINE)
{
    size_t size = ms_strlen(str);

    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD bytesWritten;
    WriteConsole(hStdout, str, size, &bytesWritten, NULL);
    return bytesWritten != size;
}
#else
;
#endif
#endif


MSLIB_EXPORT int ms_puts_any(const char *str)
#if defined(ADD_SOURCE_CODE) || defined(FORCE_INLINE)
{
    size_t size = ms_strlen(str);

    return fwrite(str, size, 1, stdout) != size;
}
#else
;
#endif


#ifdef WIN32
    #define ms_puts(str) ms_puts_win(str)
#else
    #define ms_puts(str) ms_puts_any(str)
#endif



MSLIB_EXPORT int ms_atoi(char *number)
#if defined(ADD_SOURCE_CODE) || defined(FORCE_INLINE)
{
    char *start = next_nonwhitespace(number);
    int res = 0, neg = 0;
    if (*start == '+')
    {
        start++;
    }
    else if (*start == '-')
    {
        start++;
        neg = 0x10000000;
    }
    while ('0' <= *start && *start <= '9')
    {
        int tmp1, tmp2;
        if (__builtin_expect(__builtin_add_overflow(res << 1, res << 3, &tmp1), 0))
        {
            errno = ERANGE;
            return (neg ? INT_MIN : INT_MAX);
        }
        if (__builtin_expect(__builtin_add_overflow(tmp1, (*start) & 0xF, &tmp2), 0))
        {
            errno = ERANGE;
            return (neg ? INT_MIN : INT_MAX);
        }
        res = tmp2;
        ++start;
    }
    return (neg ? -res : res);
}
#else
;
#endif



MSLIB_EXPORT size_t ms_size_fgets(char * __restrict__ str, size_t count, FILE * __restrict__ file)
#if defined(ADD_SOURCE_CODE) || defined(FORCE_INLINE)
{
    if (!count) return 0;
    char *str_start = str;
    int c;
    while (--count && (c = getc(file)) && c != EOF)
    {
        *str++ = c;
        if (c == '\n')
        {
            *str++ = 0;
            return str - str_start - 1;
        }
    }
    *str++ = 0;
    if (str - str_start == 1)
    {
        return 0;
    }
    return str - str_start - 1;
}
#else
;
#endif



MSLIB_EXPORT char *ms_fgets(char * __restrict__ str, size_t count, FILE * __restrict__ file)
#if defined(ADD_SOURCE_CODE) || defined(FORCE_INLINE)
{
    ms_size_fgets(str, count, file);
    return str;
}
#else
;
#endif


/*
    strtok idea:
        
        1. make lookup table on INTs.
        2. fill it from delimiters with -1
        3. load 32 bytes -> convert 32epi8 -> 8epi32
        4. use vpgatherdd instruction -> search LUT.
        5. act as strchr.
        
        make 2 searches - first to skip delimiters, second - to find next delimiter

*/
#ifdef __AVX2__
#ifdef FORCE_INLINE
    extern _Thread_local char *avx2_gather_current_pos = NULL;
    extern _Thread_local const char *avx2_gather_prev_delimiters = NULL;
    extern _Thread_local uint32_t avx2_gather_delimiters_table[256];
#endif
MSLIB_EXPORT char *ms_strtok_avx2_gather(char * __restrict__ str, const char * __restrict__ delimiters)
#if defined(ADD_SOURCE_CODE) || defined(FORCE_INLINE)
{    
    #ifndef FORCE_INLINE
        _Thread_local static char *avx2_gather_current_pos = NULL;
        _Thread_local static const char *avx2_gather_prev_delimiters = NULL;
        _Thread_local static uint32_t avx2_gather_delimiters_table[256];
    #endif

    if (avx2_gather_prev_delimiters != delimiters)
    {
        ms_memclear((char *)avx2_gather_delimiters_table, sizeof(avx2_gather_delimiters_table));

        avx2_gather_delimiters_table[0] = -1;

        avx2_gather_prev_delimiters = delimiters;
        while (*delimiters)
        {
            avx2_gather_delimiters_table[(unsigned char)*delimiters] = -1;
            delimiters++;
        }
        delimiters = avx2_gather_prev_delimiters;
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

    // TODO: unroll line to 64 byte fetch / parallel compuctation?
    while (1)
    {
        __m128i xmm0 = _mm_cvtsi64_si128(*(__int64 *)(avx2_gather_current_pos));
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

    // it is end
    if (*avx2_gather_current_pos == 0) 
    {
        return NULL;
    }

    char *token_start = avx2_gather_current_pos;

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

    // TODO: unroll line to 64 byte fetch / parallel compuctation?
    while (1)
    {
        __m128i xmm0 = _mm_loadu_si128((__m128i *)(avx2_gather_current_pos));
        __m256i ymm0 = _mm256_cvtepu8_epi32(xmm0);
        __m256i ymm1 = _mm256_i32gather_epi32(avx2_gather_delimiters_table, ymm0, 4);
        uint32_t msk0 = _mm256_movemask_epi8(ymm1);
        if (msk0)
        {
            avx2_gather_current_pos += _tzcnt_u32(msk0) / 4;
            break;
        }
        avx2_gather_current_pos += 8;
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
    _Thread_local const char *x64_prev_delimiters = NULL;
    _Thread_local uint8_t x64_delimiters_table[256];
#endif
MSLIB_EXPORT char *ms_strtok_x64(char * __restrict__ str, const char * __restrict__ delimiters)
#if defined(ADD_SOURCE_CODE) || defined(FORCE_INLINE)
{    
    #ifndef FORCE_INLINE
        _Thread_local static char *x64_current_pos = NULL;
        _Thread_local static const char *x64_prev_delimiters = NULL;
        _Thread_local static uint8_t x64_delimiters_table[256];
    #endif

    if (x64_prev_delimiters != delimiters)
    {
        ms_memclear((char *)x64_delimiters_table, sizeof(x64_delimiters_table));
        x64_prev_delimiters = delimiters;
        while (*delimiters)
        {
            x64_delimiters_table[(unsigned char)*delimiters] = -1;
            delimiters++;
        }
        delimiters = x64_prev_delimiters;
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


/* internal functions */

// #ifdef __AVX2__
// MSLIB_EXPORT const char *_ms_strstr_search_32_chars(const char *str, __m256i search, size_t length)
// {
//     /* load two aligned blocks : current and next */
//     const char *aligned = (const char *)((uintptr_t)str & 0x1F);
//     __m256i ymm0, ymm1;
// 
//     #ifdef USE_MANY_LOADING_IN_STRSTR_32    
//         const __m256i reverse_shuffle = _mm256_set_epi8(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15);
//         const __m256i blend_mask = _mm256_setr_epi8(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xFF);
//         #define SHIFTL(ymm) \
//             ymm = _mm256_alignr_epi8(_mm256_permute2x128_si256(ymm, ymm, _MM_SHUFFLE(2, 0, 0, 1)), ymm, 1)
//         #define SHIFTR(ymm) \
//             ymm = _mm256_alignr_epi8(ymm, _mm256_permute2x128_si256(ymm, ymm, _MM_SHUFFLE(0, 0, 2, 0)), 15)
//         #define REVERSE(ymm) \
//             ymm = _mm256_permute4x64_epi64(_mm256_shuffle_epi8(ymm, reverse_shuffle), _MM_SHUFFLE(1, 0, 3, 2));
//             
//         ymm0 = _mm256_loadu_si256((__m256i *)(str +  0));
//         ymm1 = _mm256_loadu_si256((__m256i *)(str + 32));
//         REVERSE(ymm1);
// 
//         // printf(">%s : %s\n", str);
//         SHIFTL(ymm0);
//         ymm0 = _mm256_blendv_epi8(ymm0, ymm1, blend_mask);
//         // _mm256_storeu_si256((__m256i *)str, ymm0);
//         SHIFTR(ymm1);
//         // printf(">%s\n", str);
//     #else
//     #endif
// 
//     char str_search_buf[33] = {}, *str_search = str_search_buf;
//     _mm256_storeu_si256((__m256i *)str_search, search);
// 
//     /* naive algo */
//     while (*str != 0)
//     {
//         /* match */
//         const char *match = str;
//         str_search = str_search_buf;
//         while (*str_search && *match && *match == *str_search)
//         {
//             match++;
//             str_search++;
//         }
//         if (*str_search == 0)
//         {
//             return str;
//         }
//         str++;
//     }
//     
//     return NULL;
// }
// #endif


MSLIB_EXPORT const char *ms_strstr(const char * __restrict__ str, const char * __restrict__ str_search)
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
    // /* length of str_search <= 32 */
    // #ifdef __AVX2__
    // {
    //     /* check str_search 32 bits: */
    //     char buf[32] = {};
    //     size_t search_length = 0;
    //     while (search_length <= 32 && str_search[search_length])
    //     {
    //         buf[search_length] = str_search[search_length];
    //         search_length++;
    //     }
    //     __m256i search_ymm = _mm256_loadu_si256((__m256i *)buf);
    //     if (search_length <= 32)
    //     {
    //         return _ms_strstr_search_32_chars(str, search_ymm, search_length);
    //     }
    // }
    // #endif
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


MSLIB_EXPORT size_t ms_getline(char ** __restrict__ lineptr, size_t * __restrict__ count, FILE * __restrict__ file)
#if defined(ADD_SOURCE_CODE) || defined(FORCE_INLINE)
{
    size_t alloc = 0;
    if (*count <= 1)
    {
        free(*lineptr);
        alloc = 16;
        *lineptr = malloc(alloc);
        *count = alloc;
    }
    size_t read = 0, add_read = 1;
    while (add_read)
    {
        printf("Waiting...\n");
        add_read = ms_size_fgets(*lineptr + read, *count - read, file);
        printf("Read %d\n", (int)add_read);
        read += add_read;
        if (read + 1 == *count)
        {
            alloc <<= 1;
            *lineptr = realloc(*lineptr, alloc);
            *count = alloc;
        }
        else
        {
            break;
        }
    }
    return *count;
}
#else
;
#endif


#ifdef __cplusplus
}
#endif


#endif
