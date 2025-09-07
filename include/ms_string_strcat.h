#ifndef MSLIB_STRING_STRCAT
#define MSLIB_STRING_STRCAT


#include "ms_string_common.h"


#include "ms_string_strend.h"
#include "ms_string_strcpy.h"


#ifdef __cplusplus
extern "C"
{
#endif





MSLIB_EXPORT char *ms_strcat(char * __restrict__ dest, const char * __restrict__ src)
#if defined(ADD_SOURCE_CODE) || defined(FORCE_INLINE)
{
    if (dest == NULL || src == NULL)
    {
        return NULL;
    }

    ms_strcpy(ms_strend(dest), src);
    return dest;
}
#else
;
#endif





#ifdef __cplusplus
}
#endif



#endif
