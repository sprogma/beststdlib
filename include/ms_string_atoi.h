#ifndef MSLIB_STRING_ATOI
#define MSLIB_STRING_ATOI


#include "ms_string_common_headers.h"

#include "ms_string_next_nonwhitespace.h"



#ifdef __cplusplus
extern "C"
{
#endif


MSLIB_EXPORT int ms_atoi(char *number)
#if defined(ADD_SOURCE_CODE) || defined(FORCE_INLINE)
{
    if (number == NULL)
    {
        return 0;
    }
    
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




#ifdef __cplusplus
}
#endif



#endif
