#ifndef RAND64_SW_H
#define RAND64_SW_H

/* Initialize software random number generator */
int software_rand64_init(void);

/* Generate a random 64-bit value using mrand48_r */
unsigned long long software_rand64(void);

/* Finalize software random number generator */
void software_rand64_fini(void);

/* Initialize file-based random number generator */
int file_rand64_init(const char *filename);

/* Generate a random 64-bit value from file */
unsigned long long file_rand64(void);

/* Finalize file-based random number generator */
void file_rand64_fini(void);

#endif