#ifndef OUTPUT_H
#define OUTPUT_H

#include "options.h"

/* Initialize output subsystem */
int init_output(struct randall_options *opts);

/* Write bytes to output */
int write_bytes(const char *buf, int nbytes);

/* Finalize output subsystem */
void finalize_output(void);

#endif