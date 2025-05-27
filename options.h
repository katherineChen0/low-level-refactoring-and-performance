#ifndef options_h
#define options_h

#include <errno.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>

int process_options(int argc, char **argv, long long* nbytes, int* inputChoice, int* outputChoice, long long* chunksize);

#endif /* rand64_hw_h */