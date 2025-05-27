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
        srand48_r(time(NULL), &rand_data);
    } else {
        urandstream = fopen(filename, "r");
        if (!urandstream) {
            fprintf(stderr, "Error opening %s\n", filename);
            abort();
        }
    }
}

/* Return a random value, using software operations.  */
unsigned long long
software_rand64(void)
{
    if (use_mrand48) {
        long int x1, x2;
        mrand48_r(&rand_data, &x1);
        mrand48_r(&rand_data, &x2);
        return ((unsigned long long)x1 << 32) | (unsigned long long)x2;
    } else {
        unsigned long long int x;
        if (fread(&x, sizeof x, 1, urandstream) != 1) {
            fprintf(stderr, "Error reading random data\n");
            abort();
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

void setFilename(char *newfile)
{
    filename = newfile;
    use_mrand48 = 0;
}

void setMrand48(void)
{
    use_mrand48 = 1;
}