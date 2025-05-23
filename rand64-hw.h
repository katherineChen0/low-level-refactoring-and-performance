#ifndef RAND64_HW_H
#define RAND64_HW_H

/* Check if rdrand instruction is supported */
int rdrand_supported(void);

/* Generate a random 64-bit value using hardware rdrand */
unsigned long long hardware_rand64(void);

/* Finalize hardware random number generator (no-op) */
void hardware_rand64_fini(void);

#endif