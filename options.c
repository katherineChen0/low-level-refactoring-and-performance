#include "options.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <errno.h>
#include <stdbool.h>

bool parse_options(int argc, char **argv, struct options *opts) {
    // Set defaults
    opts->input_method = "rdrand";
    opts->output_method = "stdio";
    opts->output_block_size = 0;
    opts->nbytes = 0;

    int opt;
    while ((opt = getopt(argc, argv, "i:o:")) != -1) {
        switch (opt) {
        case 'i':
            opts->input_method = optarg;
            break;
        case 'o':
            opts->output_method = optarg;
            // Check if output method is a number
            char *endptr;
            opts->output_block_size = strtoll(optarg, &endptr, 10);
            if (*endptr == '\0' && opts->output_block_size > 0) {
                opts->output_method = "block";
            }
            break;
        default:
            fprintf(stderr, "Usage: %s [-i input] [-o output] NBYTES\n", argv[0]);
            return false;
        }
    }

    if (optind >= argc) {
        fprintf(stderr, "Expected argument after options\n");
        return false;
    }

    char *endptr;
    errno = 0;
    opts->nbytes = strtoll(argv[optind], &endptr, 10);
    if (errno || *endptr != '\0' || opts->nbytes < 0) {
        fprintf(stderr, "%s: invalid number of bytes\n", argv[optind]);
        return false;
    }

    return true;
}