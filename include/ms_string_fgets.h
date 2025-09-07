#ifndef MSLIB_STRING_FGETS
#define MSLIB_STRING_FGETS


#include "ms_string_common.h"

#include "ms_string_size_fgets.h"


#ifdef __cplusplus
extern "C"
{
#endif



MSLIB_EXPORT char *ms_fgets(char * __restrict__ str, size_t count, FILE * __restrict__ file)
#if defined(ADD_SOURCE_CODE) || defined(FORCE_INLINE)
{
    if (str != NULL)
    {
        ms_size_fgets(str, count, file);
    }
    return str;
}
#else
;
#endif




#ifdef __cplusplus
}
#endif



#endif
