#include "ms_string.h"


int main( void )
{
    {
        char str[] = "123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
        char str_s[] = "abcd";
        printf("%s\nin\n%s\n=>\n%s\n", str_s, str, ms_strstr(str, str_s));
    }
    {
        char str[] = "1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111123111111111111";
        char str_s[] = "123";
        printf("%s\nin\n%s\n=>\n%s\n", str_s, str, ms_strstr(str, str_s));
    }
}
