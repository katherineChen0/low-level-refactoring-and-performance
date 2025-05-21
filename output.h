#ifndef OUTPUT_H
#define OUTPUT_H

#include <stdbool.h>

bool initialize_output(const struct options *opts);
bool write_bytes(unsigned long long x, int nbytes);
void finalize_output(void);
bool flush_remaining_bytes(void);

#endif