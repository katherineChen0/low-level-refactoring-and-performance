#ifndef OPTIONS_H
#define OPTIONS_H

struct options {
    const char *input_method;
    const char *output_method;
    long long output_block_size;
    long long nbytes;
};

bool parse_options(int argc, char **argv, struct options *opts);

#endif