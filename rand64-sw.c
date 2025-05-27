#include "rand64-sw.h"

/* Software implementation.  */
static FILE *urandstream;
static char *filename = "/dev/random";
static struct drand48_data rand_data;
static int use_mrand48 = 0;

/* Initialize the software rand64 implementation.  */
void software_rand64_init(void)
{
    if (use_mrand48) {
        unsigned seed;
        FILE *urand = fopen("/dev/urandom", "r");
        if (urand) {
            if (fread(&seed, sizeof seed, 1, urand) != 1) {
                // Fallback to time if /dev/urandom fails
                seed = time(NULL);
            }
            fclose(urand);
        } else {
            seed = time(NULL);
        }
        srand48_r(seed, &rand_data);
    } else {
        urandstream = fopen(filename, "r");
        if (!urandstream) {
            fprintf(stderr, "Error opening %s\n", filename);
            abort();
        }
    }
}

void setFilename(char *newfile)
{
    filename = newfile;
    use_mrand48 = 0;
    
    // Close existing stream if open
    if (urandstream) {
        fclose(urandstream);
        urandstream = NULL;
    }
    
    // Try to open the new file immediately to verify it's readable
    FILE *test = fopen(filename, "r");
    if (!test) {
        fprintf(stderr, "Error: cannot open file %s\n", filename);
        return;
    }
    fclose(test);
}

/* Return a random value, using software operations.  */
unsigned long long
software_rand64(void)
{
    if (use_mrand48) {
        long int x1, x2, x3, x4;
        // Generate 4 numbers and combine them with bit mixing
        mrand48_r(&rand_data, &x1);
        mrand48_r(&rand_data, &x2);
        mrand48_r(&rand_data, &x3);
        mrand48_r(&rand_data, &x4);
        
        // Combine with XOR and shifts for better randomness
        unsigned long long result = (unsigned long long)(x1 & 0xFFFF) << 48;
        result ^= (unsigned long long)(x2 & 0xFFFF) << 32;
        result ^= (unsigned long long)(x3 & 0xFFFF) << 16;
        result ^= (unsigned long long)(x4 & 0xFFFF);
        
        // Additional mixing
        result ^= result >> 32;
        result *= 0x2545F4914F6CDD1DULL;
        result ^= result >> 32;
        
        return result;
    } else {
        unsigned long long int x;
        // Reopen the stream if it's not open
        if (!urandstream) {
            urandstream = fopen(filename, "r");
            if (!urandstream) {
                fprintf(stderr, "Error reopening file %s\n", filename);
                abort();
            }
        }
        if (fread(&x, sizeof x, 1, urandstream) != 1) {
            // If we hit EOF, rewind and try again
            rewind(urandstream);
            if (fread(&x, sizeof x, 1, urandstream) != 1) {
                fprintf(stderr, "Error reading random data from file\n");
                abort();
            }
        }
        return x;
    }
}

/* Finalize the software rand64 implementation.  */
void software_rand64_fini(void)
{
    if (!use_mrand48 && urandstream) {
        fclose(urandstream);
    }
}

void setMrand48(void)
{
    use_mrand48 = 1;
}