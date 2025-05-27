#include "options.h"
#include "output.h"
#include "rand64-hw.h"
#include "rand64-sw.h"
#include <stdlib.h>
#include <string.h>

void (*initialize)(void) = NULL;
unsigned long long (*rand64)(void) = NULL;
void (*finalize)(void) = NULL;

/* Main program, which outputs N bytes of random data.  */
int main(int argc, char **argv)
{
    long long nbytes;
    long long chunksize = 0;
    int inputChoice = 0; // 0 (default) = rdrand, 1 = mrand48_r, 2 = /F
    int outputChoice = 0; // 0 (default) = stdio, 1 = N

    // Process the options
    if (process_options(argc, argv, &nbytes, &inputChoice, &outputChoice, &chunksize)) {
        return 1;
    }

    /* If there's no work to do, don't worry about which library to use.  */
    if (nbytes == 0)
        return 0;

    /* Now that we know we have work to do, arrange to use the
       appropriate library.  */
    void (*initialize)(void);
    unsigned long long (*rand64)(void);
    void (*finalize)(void);

    if (inputChoice == 0) { // rdrand
        if (rdrand_supported()) {
            initialize = hardware_rand64_init;
            rand64 = hardware_rand64;
            finalize = hardware_rand64_fini;
        } else {
            fprintf(stderr, "rdrand hardware random number generator not available\n");
            return 1;
        }
    } else if (inputChoice == 1) { // mrand48_r
        setMrand48();
        initialize = software_rand64_init;
        rand64 = software_rand64;
        finalize = software_rand64_fini;
    } else if (inputChoice == 2) { // file
        initialize = software_rand64_init;
        rand64 = software_rand64;
        finalize = software_rand64_fini;
    }

    initialize();
    int wordsize = sizeof(unsigned long long);

    if (outputChoice == 0) { // stdio output
        do {
            unsigned long long x = rand64();
            int outbytes = nbytes < wordsize ? nbytes : wordsize;
            if (!writebytes(x, outbytes)) {
                fprintf(stderr, "Output error\n");
                finalize();
                return 1;
            }
            nbytes -= outbytes;
        } while (0 < nbytes);

        if (fclose(stdout) != 0) {
            fprintf(stderr, "Output error\n");
            finalize();
            return 1;
        }
    } else if (outputChoice == 1) { // block output
        char *buffer = malloc(chunksize);
        if (!buffer) {
            fprintf(stderr, "Memory allocation error\n");
            finalize();
            return 1;
        }

        while (nbytes > 0) {
            size_t bytes_to_write = (nbytes >= chunksize) ? chunksize : nbytes;
            size_t buffer_pos = 0;

            // Fill buffer with random data
            while (buffer_pos < bytes_to_write) {
                unsigned long long x = rand64();
                size_t bytes_from_rand = (bytes_to_write - buffer_pos < wordsize) 
                                       ? bytes_to_write - buffer_pos : wordsize;
                memcpy(buffer + buffer_pos, &x, bytes_from_rand);
                buffer_pos += bytes_from_rand;
            }

            // Write buffer to stdout
            if (!writeblock(buffer, bytes_to_write)) {
                fprintf(stderr, "Output error\n");
                free(buffer);
                finalize();
                return 1;
            }

            nbytes -= bytes_to_write;
        }

        free(buffer);
    }

    finalize();
    return 0;
}