#ifndef MSLIB_STRING_PARTIAL_IMPLEMENTED
#define MSLIB_STRING_PARTIAL_IMPLEMENTED


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
    strstr
*/
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





// TODO: make getline function.
// MSLIB_EXPORT size_t ms_getline(char ** __restrict__ lineptr, size_t * __restrict__ count, FILE * __restrict__ file)
// #if defined(ADD_SOURCE_CODE) || defined(FORCE_INLINE)
// {
//     size_t alloc = 0;
//     if (*count <= 1)
//     {
//         free(*lineptr);
//         alloc = 16;
//         *lineptr = malloc(alloc);
//         *count = alloc;
//     }
//     size_t read = 0, add_read = 1;
//     while (add_read)
//     {
//         printf("Waiting...\n");
//         add_read = ms_size_fgets(*lineptr + read, *count - read, file);
//         printf("Read %d\n", (int)add_read);
//         read += add_read;
//         if (read + 1 == *count)
//         {
//             alloc <<= 1;
//             *lineptr = realloc(*lineptr, alloc);
//             *count = alloc;
//         }
//         else
//         {
//             break;
//         }
//     }
//     return *count;
// }
// #else
// ;
// #endif



#ifdef __cplusplus
}
#endif



#endif
