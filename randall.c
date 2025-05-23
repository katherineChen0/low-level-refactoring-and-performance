#include "options.h"
#include "output.h"
#include "rand64-hw.h"
#include "rand64-sw.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>

int main(int argc, char **argv) {
    struct options opts;
    if (!parse_options(argc, argv, &opts)) {
        return 1;
    }

    if (opts.nbytes == 0) {
        return 0;
    }

    // Initialize random number generator
    void (*rand_init)(void) = NULL;
    unsigned long long (*rand64)(void) = NULL;
    void (*rand_fini)(void) = NULL;

    if (strcmp(opts.input_method, "rdrand") == 0) {
        if (!rdrand_supported()) {
            fprintf(stderr, "Error: RDRAND instruction not supported on this processor\n");
            return 1;
        }
        rand_init = hardware_rand64_init;
        rand64 = hardware_rand64;
        rand_fini = hardware_rand64_fini;
    } else {
        rand_init = (void (*)(void))software_rand64_init;
        rand64 = software_rand64;
        rand_fini = software_rand64_fini;
        software_rand64_init(opts.input_method);
    }

    if (!initialize_output(&opts)) {
        return 1;
    }

    int wordsize = sizeof(unsigned long long);
    int output_errno = 0;
    long long remaining = opts.nbytes;

    rand_init();
    while (remaining > 0) {
        unsigned long long x = rand64();
        int outbytes = remaining < wordsize ? remaining : wordsize;
        if (!write_bytes(x, outbytes)) {
            output_errno = errno;
            break;
        }
        remaining -= outbytes;
    }

    if (!flush_remaining_bytes()) {
        output_errno = errno;
    }

    if (output_errno) {
        errno = output_errno;
        perror("output");
    }

    rand_fini();
    finalize_output();

    return !!output_errno;
}