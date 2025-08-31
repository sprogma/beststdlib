#include "ms_string.h"


int main( void )
{
    {
        const char *str = "abcabcabc";
        const char *str_s = "abca";
        printf("%s\nin\n%s\n=>\n%s\n", str, str_s, ms_strstr(str, str_s));
    }
    {
        const char *str = "1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111123111111111111";
        const char *str_s = "123";
        printf("%s\nin\n%s\n=>\n%s\n", str, str_s, ms_strstr(str, str_s));
    }
}
