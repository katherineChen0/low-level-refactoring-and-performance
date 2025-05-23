#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include "options.h"

static void usage(void)
{
    fprintf(stderr, "Usage: randall [-i input] [-o output] nbytes\n");
    fprintf(stderr, "  -i input: rdrand (default), mrand48_r, or /filename\n");
    fprintf(stderr, "  -o output: stdio (default) or block_size\n");
}

static int is_positive_integer(const char *str)
{
    if (!str || *str == '\0')
        return 0;
    
    for (const char *p = str; *p; p++) {
        if (!isdigit(*p))
            return 0;
    }
    return 1;
}

int parse_options(int argc, char **argv, struct randall_options *opts)
{
    /* Initialize default options */
    opts->nbytes = 0;
    opts->input = INPUT_RDRAND;
    opts->input_file = NULL;
    opts->output = OUTPUT_STDIO;
    opts->block_size = 0;

    int c;
    while ((c = getopt(argc, argv, "i:o:")) != -1) {
        switch (c) {
            case 'i':
                if (strcmp(optarg, "rdrand") == 0) {
                    opts->input = INPUT_RDRAND;
                } else if (strcmp(optarg, "mrand48_r") == 0) {
                    opts->input = INPUT_MRAND48_R;
                } else if (optarg[0] == '/') {
                    opts->input = INPUT_FILE;
                    opts->input_file = optarg;
                } else {
                    fprintf(stderr, "randall: invalid input option: %s\n", optarg);
                    usage();
                    return -1;
                }
                break;
            case 'o':
                if (strcmp(optarg, "stdio") == 0) {
                    opts->output = OUTPUT_STDIO;
                } else if (is_positive_integer(optarg)) {
                    opts->output = OUTPUT_BLOCK;
                    opts->block_size = atoi(optarg);
                    if (opts->block_size <= 0) {
                        fprintf(stderr, "randall: invalid block size: %s\n", optarg);
                        usage();
                        return -1;
                    }
                } else {
                    fprintf(stderr, "randall: invalid output option: %s\n", optarg);
                    usage();
                    return -1;
                }
                break;
            case '?':
                usage();
                return -1;
            default:
                usage();
                return -1;
        }
    }

    /* Check for nbytes argument */
    if (optind >= argc) {
        fprintf(stderr, "randall: missing nbytes argument\n");
        usage();
        return -1;
    }

    if (optind + 1 < argc) {
        fprintf(stderr, "randall: too many arguments\n");
        usage();
        return -1;
    }

    /* Parse nbytes */
    char *endptr;
    errno = 0;
    opts->nbytes = strtoll(argv[optind], &endptr, 10);
    if (errno != 0 || *endptr != '\0' || opts->nbytes < 0) {
        fprintf(stderr, "randall: invalid nbytes: %s\n", argv[optind]);
        usage();
        return -1;
    }

    return 0;
}