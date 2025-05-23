#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "rand64-sw.h"

static struct drand48_data drand_buf;
static FILE *rand_file = NULL;

/* Initialize mrand48_r with current time as seed */
int software_rand64_init(void)
{
    struct timespec ts;
    if (clock_gettime(CLOCK_REALTIME, &ts) != 0) {
        return -1;
    }
    
    if (srand48_r(ts.tv_sec ^ ts.tv_nsec, &drand_buf) != 0) {
        return -1;
    }
    
    return 0;
}

/* Generate random 64-bit value using two calls to mrand48_r */
unsigned long long software_rand64(void)
{
    long int result1, result2;
    mrand48_r(&drand_buf, &result1);
    mrand48_r(&drand_buf, &result2);
    return ((unsigned long long)result1 << 32) | (unsigned long long)result2;
}

/* No cleanup needed for mrand48_r */
void software_rand64_fini(void)
{
    /* Nothing to do */
}

/* Initialize file-based random number generator */
int file_rand64_init(const char *filename)
{
    rand_file = fopen(filename, "rb");
    if (!rand_file) {
        return -1;
    }
    return 0;
}

/* Read 64-bit value from file */
unsigned long long file_rand64(void)
{
    unsigned long long result = 0;
    if (fread(&result, sizeof(result), 1, rand_file) != 1) {
        /* If we hit EOF or error, try to rewind and read again */
        rewind(rand_file);
        if (fread(&result, sizeof(result), 1, rand_file) != 1) {
            /* If still failing, return 0 */
            result = 0;
        }
    }
    return result;
}

/* Close file */
void file_rand64_fini(void)
{
    if (rand_file) {
        fclose(rand_file);
        rand_file = NULL;
    }
}