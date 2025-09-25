#ifndef MSLIB_STRING_STRNCAT
#define MSLIB_STRING_STRNCAT


#include "ms_string_common_headers.h"


#include "ms_string_strend.h"
#include "ms_string_strncpy.h"


#ifdef __cplusplus
extern "C"
{
#endif





MSLIB_EXPORT char *ms_strncat(char * __restrict__ dest, const char * __restrict__ src, size_t count)
#if defined(ADD_SOURCE_CODE) || defined(FORCE_INLINE)
{
    if (dest == NULL || src == NULL)
    {
        return NULL;
    }
    char *ptr = ms_strend(dest);
    ms_strncpy(ptr, src, count);
    ptr[count] = 0;
    return dest;
}
#else
;
#endif





#ifdef __cplusplus
}
#endif



#endif
