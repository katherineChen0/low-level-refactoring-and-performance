#ifndef rand64_sw_h
#define rand64_sw_h

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void software_rand64_init(void);
unsigned long long software_rand64(void);
void software_rand64_fini(void);
void setFilename(char* newfile);
void setMrand48(void);

#endif /* rand64_sw_h */