#ifndef rand64_hw_h
#define rand64_hw_h

#include <cpuid.h>
#include <immintrin.h>

struct cpuid {
    unsigned eax, ebx, ecx, edx;
};

struct cpuid
cpuid (unsigned int leaf, unsigned int subleaf);

_Bool
rdrand_supported(void);

void
hardware_rand64_init (void);

unsigned long long
hardware_rand64(void);

void
hardware_rand64_fini(void);

#endif /* rand64_hw_h */