#ifndef output_h
#define output_h

#include <limits.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>

bool writebytes(unsigned long long x, int nbytes);
bool writeblock(const void *buffer, size_t nbytes);

#endif /* output_h */