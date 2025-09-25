#ifndef MSLIB_STRING_ALIGN_MACROS
#define MSLIB_STRING_ALIGN_MACROS

#define ALIGN_PAD8(buf)  (( 8 - (size_t)(buf) %  8) % 8 )
#define ALIGN_PAD32(buf) ((32 - (size_t)(buf) % 32) % 32)
#define ALIGN_PAD64(buf) ((64 - (size_t)(buf) % 64) % 64)

#endif
