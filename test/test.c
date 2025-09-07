#include "stdio.h"
#include "ms_string.h"

#define TEST(exp) \
    if (!(exp)) \
    { \
        fprintf(stderr, "Error: test on %s:%d is failed: <%s> is not true\n", __FILE__, __LINE__, #exp); \
    }
    
char str_a1000[1001] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                       "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                       "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                       "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                       "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                       "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                       "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                       "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                       "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                       "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                       "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                       "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                       "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                       "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";

/*

    functions?
    
    ms_atoi
    ms_memcpy
    ms_strcat
    ms_strchr
    ms_strcpy
    ms_strdup
    ms_strend
    ms_strlen
    ms_strncat
    ms_strncpy
    ms_strtok

*/

int main()
{

    TEST(ms_strlen("") == 0);
    TEST(ms_strlen("a") == 1);
    TEST(ms_strlen("abc") == 3);
    TEST(ms_strlen(str_a1000) == 1000);
    TEST(ms_strlen(NULL) == 0);

    {
        char s[1010] = "abc";
        strcat(s, "abc");
        TEST(ms_strlen(s) == 6);
        strcat(s, str_a1000);
        TEST(ms_strlen(s) == 1006);
    }

    {
        char s[500] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\n"
                      "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\t"
                      "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa "
                      "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\v"
                      "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa;"
                      "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa."
                      "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa,"
                      "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa!"
                      "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa?"
                      "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
        TEST(strcmp(ms_strtok(s, "\n\t \v;.,!?"), "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa") == 0);
        TEST(strcmp(ms_strtok(NULL, "\n\t \v;.,!?"), "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa") == 0);
        TEST(strcmp(ms_strtok(NULL, "\n\t \v;.,!?"), "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa") == 0);
        TEST(strcmp(ms_strtok(NULL, "\n\t \v;.,!?"), "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa") == 0);
        TEST(strcmp(ms_strtok(NULL, "\n\t \v;.,!?"), "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa") == 0);
        TEST(strcmp(ms_strtok(NULL, "\n\t \v;.,!?"), "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa") == 0);
        TEST(strcmp(ms_strtok(NULL, "\n\t \v;.,!?"), "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa") == 0);
        TEST(strcmp(ms_strtok(NULL, "\n\t \v;.,!?"), "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa") == 0);
        TEST(strcmp(ms_strtok(NULL, "\n\t \v;.,!?"), "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa") == 0);
        TEST(strcmp(ms_strtok(NULL, "\n\t \v;.,!?"), "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa") == 0);
        TEST(ms_strtok(NULL, "\n\t \v;.,!?") == NULL);
    }

    /* all next tests are not quality (almost all are too little for avx checks) */

    TEST(ms_atoi("0") == 0);
    TEST(ms_atoi("123") == 123);
    TEST(ms_atoi("-42") == -42);
    TEST(ms_atoi("+7") == 7);
    TEST(ms_atoi("   15") == 15);
    TEST(ms_atoi("2147483647") == 2147483647);
    TEST(ms_atoi("-2147483648") == -2147483648);
    TEST(ms_atoi("12abc") == 12);
    TEST(ms_atoi("") == 0);
    TEST(ms_atoi(NULL) == 0);


    {
        char buf[32];
        TEST(ms_strcpy(buf, "") == buf && buf[0] == '\0');
        TEST(ms_strcpy(buf, "abc") == buf && strcmp(buf, "abc") == 0);
    }

    
    {
        char buf[33];
        TEST(ms_strncpy(buf, "ab", 3) == buf);
        TEST(buf[2] == '\0');
        TEST(ms_strncpy(buf, "hello", 3) == buf && memcmp(buf, "hel", 3) == 0);
    }

    {
        char *d = ms_strdup("hello world");
        TEST(d != NULL && strcmp(d, "hello world") == 0);
        free(d);

        d = ms_strdup("");
        TEST(d != NULL && strcmp(d, "") == 0);
        free(d);

        d = ms_strdup(NULL);
        TEST(d == NULL);
    }

    {
        char src[32] = "0123456789";
        char dst[32];
        TEST(ms_memcpy(dst, src, 6) == dst && memcmp(dst, "012345", 6) == 0);
        TEST(ms_memcpy(dst, src, 0) == dst);
    }

    {
        char buf[32];

        ms_strcpy(buf, "foo");
        TEST(ms_strcat(buf, "bar") == buf && strcmp(buf, "foobar") == 0);
        
        ms_strcpy(buf, "a");
        TEST(ms_strncat(buf, "bcdef", 2) == buf && strcmp(buf, "abc") == 0);
        ms_strcpy(buf, "x");
        TEST(ms_strncat(buf, "yz", 10) == buf && strcmp(buf, "xyz") == 0);
    }

    {
        char buf[32] = "ABCXA";

        TEST(ms_strchr(buf, 'X') == buf + 3);
        TEST(ms_strchr(buf, 'Y') == NULL);
    }
    
    printf("All tests was passed.\n");
    return 0;
}
