#ifndef rand64_sw_h
#define rand64_sw_h

#include <stdio.h>

void
software_rand64_init(void);

unsigned long long
software_rand64(void);

void
software_rand64_fini(void);

void
setFilename (char* newfile);

#endif /* rand64_sw_h */