#ifndef MSLIB_STRING_STRCHR
#define MSLIB_STRING_STRCHR


#include "ms_string_common.h"

#include "ms_string_strchr_ptr.h"


#ifdef __cplusplus
extern "C"
{
#endif




MSLIB_EXPORT char *ms_strchr(const char *str, char ch)
#if defined(ADD_SOURCE_CODE) || defined(FORCE_INLINE)
{
    str = ms_strchr_ptr(str, ch);
    return (!*str && ch) ? NULL : (char *)str;
}
#else
;
#endif



#ifdef __cplusplus
}
#endif



#endif