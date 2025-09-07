#ifndef MSLIB_STRING_COMMON
#define MSLIB_STRING_COMMON


#include "inttypes.h"
#include "errno.h"
#include "limits.h"
#include "stdio.h"
#include "immintrin.h"


#define ADD_SOURCE_CODE
// #define STRTOK_DYNAMIC_DELIMITERS
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
        [ ] getline
*/


/*
    to see realizated function use
    gc ms_string.h | sls "MSLIB_EXPORT.*?ms"
*/


#define ALIGN_PAD8(buf)  (( 8 - (size_t)(buf) %  8) % 8 )
#define ALIGN_PAD32(buf) ((32 - (size_t)(buf) % 32) % 32)
#define ALIGN_PAD64(buf) ((64 - (size_t)(buf) % 64) % 64)

#ifdef FORCE_INLINE
    #define MSLIB_EXPORT static __attribute__((always_inline)) __inline__
#else
    #define MSLIB_EXPORT static inline
#endif

#endif
