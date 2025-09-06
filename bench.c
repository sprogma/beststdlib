#include "time.h"
#include "string.h"
#include "ms_string.h"

#ifdef WIN32
    #include "windows.h"
    #include "realtimeapiset.h"
#endif

#include "setjmp.h"
#include "signal.h"

/* best benchmark library ! [prototype] */

#define TABLE_VARIANT_COLUMN_SIZE 22
#define TABLE_OPTION_COLUMN_SIZE 16
#define SHOW_LITTLE_TIME_IN_MICROSECONDS
#define USE_LOG_SCALE_IN_GISTOGRAM

// #define RUN_WARMUP_GLOBAL_SCALE 1
// #define RUN_MEASURES_GLOBAL_SCALE 1
#define RUN_WARMUP_GLOBAL_SCALE 2
#define RUN_MEASURES_GLOBAL_SCALE 2

/* not works on linux */
// #define MIN_INSTEAD_OF_MEAN



#define BENCH(name) \
    void bench_##name(); \
    void wrapper_##name() \
    { \
        option_id = 0; \
        variant_id = 0; \
        current_option = NULL; \
        current_variant = NULL; \
        bench_##name(); \
    } \
    void bench_##name()
#define BENCH_OPTION(string) \
    if (current_option == NULL) \
    { \
        option_id++; \
        option_names[option_id] = string; \
    } \
    for(;current_option == NULL && !option_completed[option_id];current_option = string, option_completed[option_id] = 1)
#ifdef WIN32
    #ifdef MIN_INSTEAD_OF_MEAN
        #define RESET_COUNTER bestbench_counter_total = 0x7fffffffffffffffll;
    #else
        #define RESET_COUNTER bestbench_counter_total = 0;
    #endif
#else
    #define RESET_COUNTER bestbench_counter_total = 0.0;
#endif
#define BENCH_VARIANT(string) \
    variant_id++; \
    variant_names[variant_id] = string; \
    RESET_COUNTER; \
    if (!variant_completed[variant_id] && current_option == NULL) \
    { \
        abort(); \
    } \
    for (;!variant_completed[variant_id];current_variant = string, abort())
#define BENCH_RUN(warm, measure) \
            for (bench_run = -RUN_WARMUP_GLOBAL_SCALE * (warm); bench_run < RUN_MEASURES_GLOBAL_SCALE * (measure); ++bench_run)
#ifdef MIN_INSTEAD_OF_MEAN
    #ifdef WIN32
        #define BENCH_START \
                    if (bench_run >= 0) \
                    { \
                        QueryPerformanceCounter(&bestbench_counter_storage); \
                        bestbench_counter = -bestbench_counter_storage.QuadPart; \
                    }
        #define BENCH_END \
                    if (bench_run >= 0) \
                    { \
                        QueryPerformanceCounter(&bestbench_counter_storage); \
                        bestbench_counter += bestbench_counter_storage.QuadPart; \
                        if (bestbench_counter_total > bestbench_counter) \
                        { \
                            bestbench_counter_total = bestbench_counter; \
                        } \
                    }
    #else
        #error cannot use min instead of mean on linux
    #endif
#else
    #ifdef WIN32
        #define BENCH_START \
                    if (bench_run >= 0) \
                    { \
                        QueryPerformanceCounter(&bestbench_counter_storage); \
                        bestbench_counter_total -= bestbench_counter_storage.QuadPart; \
                    }
        #define BENCH_END \
                    if (bench_run >= 0) \
                    { \
                        QueryPerformanceCounter(&bestbench_counter_storage); \
                        bestbench_counter_total += bestbench_counter_storage.QuadPart; \
                    }
    #else
        #define BENCH_START \
                    if (bench_run >= 0) \
                    { \
                        clock_gettime(CLOCK_MONOTONIC, &bestbench_counter_storage); \
                        bestbench_counter_total -= (double)bestbench_counter_storage.tv_sec + (double)bestbench_counter_storage.tv_nsec / 1e9; \
                    }
        #define BENCH_END \
                    if (bench_run >= 0) \
                    { \
                        clock_gettime(CLOCK_MONOTONIC, &bestbench_counter_storage); \
                        bestbench_counter_total += (double)bestbench_counter_storage.tv_sec + (double)bestbench_counter_storage.tv_nsec / 1e9; \
                    }
    #endif
#endif

int option_id = 0;
int variant_id = 0;
int bench_run = 0;
const char *current_variant = NULL;
const char *current_option = NULL;
const char *option_names[64] = {};
const char *variant_names[64] = {};
/* variant option */
double measurements[64][64] = {};
int option_completed[64] = {};
int variant_completed[64] = {};
long long bestbench_counter = 0;
#ifdef WIN32
    LARGE_INTEGER bestbench_counter_storage;
    long long bestbench_counter_total = 0;
#else
    struct timespec bestbench_counter_storage;
    double bestbench_counter_total = 0;
#endif
volatile int do_not_optimize = 0;
// use longjmp to return index of table there is current option and variant


BENCH(strlen)
{
    int n;
    BENCH_OPTION("n=1")      { n = 1; }
    BENCH_OPTION("n=10")     { n = 10; }
    BENCH_OPTION("n=100")    { n = 100; }
    BENCH_OPTION("n=1e3")    { n = 1000; }
    BENCH_OPTION("n=1e5")    { n = 100000; }
    BENCH_OPTION("n=1e7")    { n = 10000000; }
    char *s = calloc(n, 1);
    if (n != 1)
    {
        memset(s, 'a', n - 1);
    }
    BENCH_VARIANT("gnu lib")
    {
        BENCH_RUN(256, 512)
        {
            BENCH_START
            do_not_optimize += strlen(s);
            BENCH_END
        }
        free(s);
    }
    BENCH_VARIANT("MS lib d32")
    {
        BENCH_RUN(256, 512)
        {
            BENCH_START
            do_not_optimize += (ms_strend_avx2_d32(s) - s); 
            BENCH_END
        }
        free(s);
    }
    BENCH_VARIANT("MS lib d64")
    {
        BENCH_RUN(256, 512)
        {
            BENCH_START
            do_not_optimize += (ms_strend_avx2_d64(s) - s); 
            BENCH_END
        }
        free(s);
    }
}

BENCH(memcpy)
{
    int n;
    BENCH_OPTION("n=1")      { n = 1; }
    BENCH_OPTION("n=10")     { n = 10; }
    BENCH_OPTION("n=100")    { n = 100; }
    BENCH_OPTION("n=1e3")    { n = 1000; }
    BENCH_OPTION("n=1e5")    { n = 100000; }
    BENCH_OPTION("n=1e7")    { n = 10000000; }
    char *s = calloc(n, 1);
    char *d = calloc(n, 1);
    if (n != 1)
    {
        memset(s, 'a', n - 1);
    }
    BENCH_VARIANT("gnu lib")
    {
        BENCH_RUN(32, 256)
        {
            BENCH_START
            memcpy(d, s, n);
            BENCH_END
        }
        do_not_optimize += d[n - 1];
        free(s);
        free(d);
    }
    BENCH_VARIANT("MS lib d32")
    {
        BENCH_RUN(32, 256)
        {
            BENCH_START
            ms_memcpy_avx2_d32(d, s, n);
            BENCH_END
        }
        do_not_optimize += d[n - 1];
        free(s);
        free(d);
    }
    BENCH_VARIANT("MS lib d64")
    {
        BENCH_RUN(32, 256)
        {
            BENCH_START
            ms_memcpy_avx2_d64(d, s, n);
            BENCH_END
        }
        do_not_optimize += d[n - 1];
        free(s);
        free(d);
    }
}

BENCH(strcpy)
{
    int n;
    BENCH_OPTION("n=1")      { n = 1; }
    BENCH_OPTION("n=10")     { n = 10; }
    BENCH_OPTION("n=100")    { n = 100; }
    BENCH_OPTION("n=1e3")    { n = 1000; }
    BENCH_OPTION("n=1e5")    { n = 100000; }
    BENCH_OPTION("n=1e7")    { n = 10000000; }
    char *s = calloc(n, 1);
    char *d = calloc(n, 1);
    if (n != 1)
    {
        memset(s, 'a', n - 1);
    }
    BENCH_VARIANT("gnu lib")
    {
        BENCH_RUN(32, 256)
        {
            BENCH_START
            strcpy(d, s);
            BENCH_END
        }
        do_not_optimize += d[n - 1];
        free(s);
        free(d);
    }
    BENCH_VARIANT("MS lib d32")
    {
        BENCH_RUN(32, 256)
        {
            BENCH_START
            ms_strcpy_avx2_d32(d, s);
            BENCH_END
        }
        do_not_optimize += d[n - 1];
        free(s);
        free(d);
    }
    BENCH_VARIANT("MS lib d64")
    {
        BENCH_RUN(32, 256)
        {
            BENCH_START
            ms_strcpy_avx2_d64(d, s);
            BENCH_END
        }
        do_not_optimize += d[n - 1];
        free(s);
        free(d);
    }
}

void fill_strtok(char *s, int n, int step, int delim, const char *delimiters)
{
    int delim_id = 0;
    
    {
        int pos = 0;
        int now = 0;
        int len = step; 
        while (pos < n)
        {
            len--;
            if (now == 0)
            {
                s[pos++] = ' ';
                if (len == 0)
                {
                    len = delim;
                    now = 1;
                }
            }
            else
            {
                s[pos++] = delimiters[delim_id++];
                if (delimiters[delim_id] == 0)
                {
                    delim_id = 0;
                }
                if (len == 0)
                {
                    len = step;
                    now = 0;
                }
            }
        }
        s[pos++] = 0;
    }
}

BENCH(strtok)
{
    int n, sep, step, delim = 1;
    BENCH_OPTION("n=10 sep=1")        { n = 10; sep = 0; step = 5; }
    BENCH_OPTION("n=10 sep=2")        { n = 10; sep = 1; step = 5; }
    BENCH_OPTION("n=10 sep=64")       { n = 10; sep = 2; step = 5; }
    BENCH_OPTION("n=1e3 sep=1")      { n = 1000; sep = 0; step = 50; }
    BENCH_OPTION("n=1e3 sep=64")     { n = 1000; sep = 2; step = 50; }
    BENCH_OPTION("n=1e5 sep=1")    { n = 1000; sep = 0; step = 100; }
    BENCH_OPTION("n=1e5 sep=64")   { n = 1000; sep = 2; step = 100; }
    BENCH_OPTION("n=1e7/100/1 sep=1")  { n = 10000000; sep = 0; step = 100; }
    BENCH_OPTION("n=1e7/100/1 sep=2")  { n = 10000000; sep = 1; step = 100; }
    BENCH_OPTION("n=1e7/100/1 sep=64") { n = 10000000; sep = 2; step = 100; }
    BENCH_OPTION("n=1e7/1e4/1 sep=1")  { n = 10000000; sep = 0; step = 10000; }
    BENCH_OPTION("n=1e7/1e4/1 sep=2")  { n = 10000000; sep = 1; step = 10000; }
    BENCH_OPTION("n=1e7/1e4/1 sep=64") { n = 10000000; sep = 2; step = 10000; }
    BENCH_OPTION("n=1e7/1e3/1e3 sep=1")  { n = 10000000; sep = 0; step = 1000; delim = 1000; }
    BENCH_OPTION("n=1e7/1e3/1e3 sep=2")  { n = 10000000; sep = 1; step = 1000; delim = 1000; }
    BENCH_OPTION("n=1e7/1e3/1e3 sep=64") { n = 10000000; sep = 2; step = 1000; delim = 1000; }

    const char *delimiters;
    if (sep == 0)
    {
        delimiters = "a";
    }
    else if (sep == 1)
    {
        delimiters = "az";
    }
    else
    {
        delimiters = "!#%')+-/13579;=?ACEGIKMOQSUWY[]_acegikmoqsuwy{}";
    }
    char *s = malloc(n + 1);
    
    BENCH_VARIANT("gnu lib")
    {
        BENCH_RUN(4, 16)
        {
            fill_strtok(s, n, step, delim, delimiters);
            
            BENCH_START
            char *token = strtok(s, delimiters);
            while (token != NULL) 
            {
                token = strtok(NULL, delimiters);
            }
            BENCH_END
        }
        do_not_optimize += s[n - 1];
        free(s);
    }
    BENCH_VARIANT("MS lib gather")
    {
        BENCH_RUN(4, 16)
        {
            fill_strtok(s, n, step, delim, delimiters);
            
            BENCH_START
            char *token = ms_strtok_avx2_gather(s, delimiters);
            while (token != NULL) 
            {
                token = ms_strtok_avx2_gather(NULL, delimiters);
            }
            BENCH_END
        }
        do_not_optimize += s[n - 1];
        free(s);
    }
    BENCH_VARIANT("MS lib base")
    {
        BENCH_RUN(4, 16)
        {
            fill_strtok(s, n, step, delim, delimiters);
            
            BENCH_START
            char *token = ms_strtok_x64(s, delimiters);
            while (token != NULL) 
            {
                token = ms_strtok_x64(NULL, delimiters);
            }
            BENCH_END
        }
        do_not_optimize += s[n - 1];
        free(s);
    }
}

BENCH(strdup)
{
    int n;
    BENCH_OPTION("n=1")      { n = 1; }
    BENCH_OPTION("n=10")     { n = 10; }
    BENCH_OPTION("n=100")    { n = 100; }
    BENCH_OPTION("n=1e3")    { n = 1000; }
    BENCH_OPTION("n=1e5")    { n = 100000; }
    BENCH_OPTION("n=1e7")    { n = 10000000; }
    char *s = calloc(n, 1);
    char *d = NULL;
    if (n != 1)
    {
        memset(s, 'a', n - 1);
    }
    BENCH_VARIANT("gnu lib")
    {
        BENCH_RUN(32, 256)
        {
            BENCH_START
            d = strdup(s);
            BENCH_END
        }
        free(s);
        free(d);
    }
    BENCH_VARIANT("MS lib")
    {
        BENCH_RUN(32, 256)
        {
            BENCH_START
            d = ms_strdup(s);
            BENCH_END
        }
        free(s);
        free(d);
    }
}


BENCH(strcat)
{
    int n;
    BENCH_OPTION("n=2")      { n = 2; }
    BENCH_OPTION("n=10")     { n = 10; }
    BENCH_OPTION("n=100")    { n = 100; }
    BENCH_OPTION("n=1e3")    { n = 1000; }
    BENCH_OPTION("n=1e5")    { n = 100000; }
    BENCH_OPTION("n=1e7")    { n = 10000000; }
    BENCH_VARIANT("gnu lib")
    {
        BENCH_RUN(32, 256)
        {
            char *s = calloc(n, 1);
            char *d = calloc(n * 2 + 1, 1);
            if (n != 1)
            {
                memset(s, 'a', n - 1);
                memset(d, 'a', n - 1);
            }
            BENCH_START
            strcat(d, s);
            BENCH_END
            free(s);
            free(d);
        }
    }
    BENCH_VARIANT("MS lib")
    {
        BENCH_RUN(32, 256)
        {
            char *s = calloc(n, 1);
            char *d = calloc(n * 2 + 1, 1);
            if (n != 1)
            {
                memset(s, 'a', n - 1);
                memset(d, 'a', n - 1);
            }
            BENCH_START
            ms_strcat(d, s);
            BENCH_END
            free(s);
            free(d);
        }
    }
    BENCH_VARIANT("MS lib")
    {
        BENCH_RUN(32, 256)
        {
            char *s = calloc(n, 1);
            char *d = calloc(n * 2 + 1, 1);
            if (n != 1)
            {
                memset(s, 'a', n - 1);
                memset(d, 'a', n - 1);
            }
            BENCH_START
            ms_strcat(d, s);
            BENCH_END
            free(s);
            free(d);
        }
    }
}


BENCH(puts)
{
    int n;
    BENCH_OPTION("n=2")      { n = 2; }
    BENCH_OPTION("n=10")     { n = 10; }
    BENCH_OPTION("n=100")    { n = 100; }
    BENCH_OPTION("n=1e3")    { n = 1000; }
    BENCH_OPTION("n=1e5")    { n = 100000; }
    BENCH_OPTION("n=1e7")    { n = 10000000; }
    char *s = calloc(n, 1);
    if (n != 1)
    {
        memset(s, 'a', n - 1);
    }
    BENCH_VARIANT("gnu lib")
    {
        BENCH_RUN(2, 2)
        {
            BENCH_START
            puts(s);
            BENCH_END
        }
        free(s);
    }
    BENCH_VARIANT("MS lib")
    {
        BENCH_RUN(2, 2)
        {
            BENCH_START
            ms_puts(s);
            BENCH_END
        }
        free(s);
    }
}


BENCH(atoi)
{
    int n;
    BENCH_OPTION("no_spaces")  { n = 0; }
    BENCH_OPTION("n=10")       { n = 10; }
    BENCH_OPTION("n=100")      { n = 100; }
    BENCH_OPTION("n=1e3")      { n = 1000; }
    BENCH_OPTION("n=1e5")      { n = 100000; }
    BENCH_OPTION("n=1e7")      { n = 10000000; }
    char *s;
    if (n == 0)
    {
        s = calloc(16, 1);
        strcpy(s, "1234567890");
    }
    else
    {
        s = calloc(n, 1);
        memset(s, ' ', n - 1);
        s[n - 3] = '1';
        s[n - 2] = '2';
    }
    BENCH_VARIANT("gnu lib")
    {
        BENCH_RUN(32, 256 + (n == 0 ? 10000 : 0))
        {
            BENCH_START
            do_not_optimize += atoi(s);
            BENCH_END
        }
        free(s);
    }
    BENCH_VARIANT("MS lib")
    {
        BENCH_RUN(32, 256 + (n == 0 ? 10000 : 0))
        {
            BENCH_START
            do_not_optimize += ms_atoi(s);
            BENCH_END
        }
        free(s);
    }
}

#ifdef WIN32
jmp_buf buf;
#else
sigjmp_buf buf;
#endif

void handler(int sig)
{
    if (sig == SIGABRT)
    {
        if (!current_option)
        {
            variant_completed[variant_id] = 1;
            memset(option_completed, 0, sizeof(option_completed));
        }
        else
        {
            #ifdef WIN32
                QueryPerformanceFrequency(&bestbench_counter_storage);
                measurements[variant_id][option_id] = (double)bestbench_counter_total / (double)bestbench_counter_storage.QuadPart
            #else
                measurements[variant_id][option_id] = bestbench_counter_total
            #endif
            #ifdef MIN_INSTEAD_OF_MEAN
                ;
            #else
                / (double)bench_run;
            #endif
            bestbench_counter = 0;
        }
        option_id = 0;
        #ifdef WIN32
        longjmp(buf, 0);
        #else
        siglongjmp(buf, 0);
        #endif
    }
}



void complete_benchmark(void (*fn)(), const char *testing_name)
{
    memset(option_names, 0, sizeof(option_names));
    memset(variant_names, 0, sizeof(variant_names));
    memset(option_completed, 0, sizeof(option_completed));
    memset(variant_completed, 0, sizeof(variant_completed));
    memset(measurements, 0, sizeof(measurements));
    option_id = 0;
    variant_id = 0;

    #ifdef WIN32
    int result = setjmp(buf);
    #else
    int result = sigsetjmp(buf, 1);
    #endif
    (void)result;
    
    if (signal(SIGABRT, &handler) == SIG_ERR)
    {
        printf("ERROR: cannot set handler on SIGABRT\n");
        exit(1);
    }
    
    fn();

    /* draw image */
    {
        FILE *f = fopen("data.csv", "w");
        
        fprintf(f, "Category");
        for (int i = 1; variant_names[i]; ++i)
        {
            putc(';', f);
            fprintf(f, "%s", variant_names[i]);
        }
        putc('\n', f);
        for (int i = 1; option_names[i]; ++i)
        {
            fprintf(f, "%s", option_names[i]);
            for (int j = 1; variant_names[j]; ++j)
            {
                putc(';', f);
                fprintf(f, "%lg", measurements[j][i] * 1e3);
            }
            putc('\n', f);
        }

        fclose(f);
        f = fopen("plot.gp", "w");
        fprintf(f, R"code(
set encoding utf8
set datafile separator ";"

# set terminal sixelgd truecolor size 1200,700 enhanced font "Arial,14"
set terminal pngcairo size 1200,700 enhanced font "Arial,14"
set output "histogram.png"

set title "function '%s' results"
set xlabel ""
set ylabel "time (ms)"

# set yrange [0:*]

set style data histogram
set style histogram clustered gap 1
set style fill solid 0.8 border -1
set boxwidth 0.8
        )code", testing_name);
        #ifdef USE_LOG_SCALE_IN_GISTOGRAM
        fprintf(f, "set logscale y");
        #endif
        fprintf(f, R"code(

set style line 1 lc rgb "#666666" lt 1 lw 1
set grid ytics linestyle 1

set key inside right top vertical

set xtics rotate by -90
# set xtic rotate by 0 scale 0

plot 'data.csv' using 2:xtic(1) title columnheader(2), \
        )code");
        for (int i = 2; variant_names[i]; ++i)
        {
            fprintf(f, "                ''         using %d        title columnheader(%d), %c\n", i + 1, i + 1, (variant_names[i + 1] ? '\\' : ' '));
        }
        fprintf(f, R"code(
set key off
set format y "%%g"
ncols = 2
delta = 0.18
        )code");
        fclose(f);

        
        system("gnuplot ./plot.gp");
        system("pwsh -c see histogram.png");
    }

    
    puts("measurements:");
    for (int x = 0; x < TABLE_VARIANT_COLUMN_SIZE; ++x)
    {
        putchar('-');
    }
    for (int i = 1; option_names[i]; ++i)
    {
        for (int x = 0; x < TABLE_OPTION_COLUMN_SIZE; ++x)
        {
            putchar('-');
        }
    }
    putchar('\n');
    for (int x = 0; x < TABLE_VARIANT_COLUMN_SIZE; ++x)
    {
        putchar(' ');
    }
    for (int i = 1; option_names[i]; ++i)
    {
        printf("%*.*s", (int)TABLE_OPTION_COLUMN_SIZE, (int)TABLE_OPTION_COLUMN_SIZE, option_names[i]);
    }
    putchar('\n');
    for (int i = 1; variant_names[i]; ++i)
    {
        printf("%-*.*s", (int)TABLE_VARIANT_COLUMN_SIZE, (int)TABLE_VARIANT_COLUMN_SIZE, variant_names[i]);
        for (int j = 1; option_names[j]; ++j)
        {
            #ifdef SHOW_LITTLE_TIME_IN_MICROSECONDS
            if (measurements[i][j] * 1e3 < 10.0)
            {
                printf("%*.3lf us", (int)TABLE_OPTION_COLUMN_SIZE - 3, measurements[i][j] * 1e6);
            }
            else
            {
            #endif
                printf("%*.3lf ms", (int)TABLE_OPTION_COLUMN_SIZE - 3, measurements[i][j] * 1e3);
            #ifdef SHOW_LITTLE_TIME_IN_MICROSECONDS
            }
            #endif
        }
        putchar('\n');
    }
    puts("\n\n");
}




int cnt = 0;
int main( void )
{
    if (1)
    {
        /* simple string functions */
        complete_benchmark(wrapper_strlen, "strlen");
        complete_benchmark(wrapper_memcpy, "memcpy");
        complete_benchmark(wrapper_strcpy, "strcpy");
        complete_benchmark(wrapper_strcat, "strcat");
        complete_benchmark(wrapper_strtok, "strtok");
        complete_benchmark(wrapper_strcpy, "strdup");
    }
    if (0)
    {
        /* io functions */
        complete_benchmark(wrapper_puts, "puts");
    }
    if (1)
    {
        /* convertion functions */
        complete_benchmark(wrapper_atoi, "atoi");
    }
}
