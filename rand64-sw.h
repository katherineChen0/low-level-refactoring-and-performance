#ifndef RAND64_SW_H
#define RAND64_SW_H

void software_rand64_init(const char *source);
unsigned long long software_rand64(void);
void software_rand64_fini(void);

#endif