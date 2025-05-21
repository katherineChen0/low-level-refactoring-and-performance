#ifndef OUTPUT_H
#define OUTPUT_H

#include <stdbool.h>

// Forward declaration of options struct
struct options;

bool initialize_output(const struct options *opts);
bool write_bytes(unsigned long long x, int nbytes);
void finalize_output(void);
bool flush_remaining_bytes(void);

#endif