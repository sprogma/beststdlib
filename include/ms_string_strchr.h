#ifndef MSLIB_STRING_STRCHR
#define MSLIB_STRING_STRCHR


#include "ms_string_common_headers.h"

#include "ms_string_strchr_ptr.h"


#ifdef __cplusplus
extern "C"
{
#endif




MSLIB_EXPORT char *ms_strchr(const char *str, char ch)
#if defined(ADD_SOURCE_CODE) || defined(FORCE_INLINE)
{
    if (str == NULL)
    {
        return NULL;
    }

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
