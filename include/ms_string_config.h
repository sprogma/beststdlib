#ifndef MSLIB_STRING_CONFIG
#define MSLIB_STRING_CONFIG


#define ADD_SOURCE_CODE
// #define STRTOK_DYNAMIC_DELIMITERS
#define USE_GATHER_INSTRUCTION_IN_STRTOK
#define USE_MANY_LOADING_IN_STRSTR_32
#define USE_32_STEP_STREND
#define USE_32_STEP_NEXT_NONWHITESPACE
#define USE_32_STEP_MEMCLEAR
#define USE_32_STEP_MEMCPY
#define USE_32_STEP_STRCPY
#define USE_32_STEP_STRNCPY
#define USE_32_STEP_STRCHR


/*

    functions database:

        [X] puts
        [X] strchr, strlen
        [X] strcat, strncat
        [X] atoi
        [X] fgets
        [X] strdup / * there is some optimizations, witch requres using ms_free instead of free? * /
        [ ] getline
*/


/*
    to see realizated function use
    gc ms_string.h | sls "MSLIB_EXPORT.*?ms"
*/


#endif
