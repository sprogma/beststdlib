#ifndef MSLIB_STRING_STRDUP
#define MSLIB_STRING_STRDUP


#include "ms_string_common.h"

#include "ms_string_strlen.h"
#include "ms_string_memcpy.h"



#ifdef __cplusplus
extern "C"
{
#endif




MSLIB_EXPORT char *ms_strdup(const char *str)
#if defined(ADD_SOURCE_CODE) || defined(FORCE_INLINE)
{
    if (str == NULL)
    {
        return NULL;
    }
    int size = ms_strlen(str) + 1;
    char *new_string = malloc(size);
    ms_memcpy(new_string, str, size);
    return new_string;
}
#else
;
#endif




#ifdef __cplusplus
}
#endif



#endif
