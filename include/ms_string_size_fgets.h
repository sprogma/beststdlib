#ifndef MSLIB_STRING_SIZE_FGETS
#define MSLIB_STRING_SIZE_FGETS


#include "ms_string_common_headers.h"


#ifdef __cplusplus
extern "C"
{
#endif



MSLIB_EXPORT size_t ms_size_fgets(char * __restrict__ str, size_t count, FILE * __restrict__ file)
#if defined(ADD_SOURCE_CODE) || defined(FORCE_INLINE)
{
    if (!count || str == NULL) return 0;
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




#ifdef __cplusplus
}
#endif



#endif
