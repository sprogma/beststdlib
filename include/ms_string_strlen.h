#ifndef MSLIB_STRING_STRLEN
#define MSLIB_STRING_STRLEN


#include "ms_string_common.h"

#include "ms_string_strend.h"



#ifdef __cplusplus
extern "C"
{
#endif





MSLIB_EXPORT size_t ms_strlen(const char *str)
#if defined(ADD_SOURCE_CODE) || defined(FORCE_INLINE)
{
    if (str == NULL)
    {
        return 0;
    }
    return ms_strend(str) - str;
}
#else
;
#endif



#ifdef __cplusplus
}
#endif



#endif
