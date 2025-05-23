/* Generate random bytes. */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "options.h"
#include "output.h"
#include "rand64-hw.h"
#include "rand64-sw.h"

int main(int argc, char **argv)
{
    /* Parse options */
    struct randall_options opts;
    if (parse_options(argc, argv, &opts) != 0) {
        return 1;
    }

    /* Check for valid nbytes */
    if (opts.nbytes < 0) {
        fprintf(stderr, "randall: invalid number of bytes: %lld\n", opts.nbytes);
        return 1;
    }

    /* Initialize random number generator */
    void (*finalize_rand)(void) = NULL;
    unsigned long long (*rand64)(void) = NULL;

    switch (opts.input) {
        case INPUT_RDRAND:
            if (!rdrand_supported()) {
                fprintf(stderr, "randall: rdrand not supported\n");
                return 1;
            }
            rand64 = hardware_rand64;
            finalize_rand = hardware_rand64_fini;
            break;
        case INPUT_MRAND48_R:
            if (software_rand64_init() != 0) {
                fprintf(stderr, "randall: failed to initialize mrand48_r\n");
                return 1;
            }
            rand64 = software_rand64;
            finalize_rand = software_rand64_fini;
            break;
        case INPUT_FILE:
            if (file_rand64_init(opts.input_file) != 0) {
                fprintf(stderr, "randall: failed to open input file: %s\n", opts.input_file);
                return 1;
            }
            rand64 = file_rand64;
            finalize_rand = file_rand64_fini;
            break;
    }

    /* Initialize output */
    if (init_output(&opts) != 0) {
        if (finalize_rand) finalize_rand();
        return 1;
    }

    /* Generate and output random bytes */
    long long bytes_written = 0;
    while (bytes_written < opts.nbytes) {
        unsigned long long x = rand64();
        int outbytes = opts.nbytes - bytes_written;
        if (outbytes > sizeof(x))
            outbytes = sizeof(x);

        if (write_bytes((char *)&x, outbytes) != outbytes) {
            fprintf(stderr, "randall: output error\n");
            if (finalize_rand) finalize_rand();
            finalize_output();
            return 1;
        }
        bytes_written += outbytes;
    }

    /* Clean up */
    if (finalize_rand) finalize_rand();
    finalize_output();

    return 0;
}