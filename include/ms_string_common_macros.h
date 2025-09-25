#ifndef MSLIB_STRING_ALIGN_MACROS
#define MSLIB_STRING_ALIGN_MACROS

#ifdef FORCE_INLINE
    #define MSLIB_EXPORT static __attribute__((always_inline)) __inline__
#else
    #define MSLIB_EXPORT static inline
#endif
        
#endif
