#ifndef MSLIB_STRING_PUTS
#define MSLIB_STRING_PUTS


#include "ms_string_common.h"

#ifdef WIN32
    #include "windows.h"
#endif

#include "ms_string_strlen.h"



#ifdef __cplusplus
extern "C"
{
#endif



#ifdef WIN32
MSLIB_EXPORT int ms_puts_win(const char *str)
#if defined(ADD_SOURCE_CODE) || defined(FORCE_INLINE)
{
    if (str == NULL)
    {
        return 0;
    }

    size_t size = ms_strlen(str);

    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD bytesWritten;
    WriteConsole(hStdout, str, size, &bytesWritten, NULL);
    return bytesWritten != size;
}
#else
;
#endif
#endif


MSLIB_EXPORT int ms_puts_any(const char *str)
#if defined(ADD_SOURCE_CODE) || defined(FORCE_INLINE)
{
    if (str == NULL)
    {
        return 0;
    }

    size_t size = ms_strlen(str);

    return fwrite(str, size, 1, stdout) != size;
}
#else
;
#endif


#ifdef WIN32
    #define ms_puts(str) ms_puts_win(str)
#else
    #define ms_puts(str) ms_puts_any(str)
#endif




#ifdef __cplusplus
}
#endif



#endif
