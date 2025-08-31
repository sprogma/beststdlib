#include "ms_string.h"

/* best benchmark library ! [prototype] */

#define BENCH(name) \
    void bench_##name(); \
    void wrapper_##name() \
    { \
        bench_##name(); \
    } \
    void bench_##name()
#define BENCH_OPTION(string)
#define BENCH_VARIANT(string)
#define BENCH_START
#define BENCH_END
// use longjmp to return index of table there is current option and variant

int f(int n)
{
    int s = 0;
    for (int i = 0; i < n; ++i)
    {
        s += i;
    }
    return s; 
}

int g(int n)
{
    int s = 0;
    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < i; ++j)
        {
            s++;
        }
    }
    return s; 
}

BENCH(sum)
{
    int n;
    BENCH_OPTION("100")   { n = 100; }
    BENCH_OPTION("1000")  { n = 1000; }
    BENCH_OPTION("10000") { n = 10000; }
    BENCH_VARIANT("f")
    { 
        BENCH_START 
        f(n); 
        BENCH_END
    }
    BENCH_VARIANT("g") 
    { 
        BENCH_START 
        g(n); 
        BENCH_END
    }
}

int main( void )
{
    wrapper_sum();
}
