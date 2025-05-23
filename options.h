#ifndef OPTIONS_H
#define OPTIONS_H

enum input_type {
    INPUT_RDRAND,
    INPUT_MRAND48_R,
    INPUT_FILE
};

enum output_type {
    OUTPUT_STDIO,
    OUTPUT_BLOCK
};

struct randall_options {
    long long nbytes;
    enum input_type input;
    char *input_file;
    enum output_type output;
    int block_size;
};

/* Parse command line options and return 0 on success, -1 on error */
int parse_options(int argc, char **argv, struct randall_options *opts);

#endif