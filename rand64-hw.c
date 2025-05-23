#include <cpuid.h>
#include <immintrin.h>
#include "rand64-hw.h"

/* Check if rdrand instruction is supported by checking CPUID */
int rdrand_supported(void)
{
    unsigned int eax, ebx, ecx, edx;
    return (__get_cpuid(1, &eax, &ebx, &ecx, &edx) && (ecx & bit_RDRND));
}

/* Generate a random 64-bit value using rdrand instruction */
unsigned long long hardware_rand64(void)
{
    unsigned long long int x;
    while (!_rdrand64_step(&x))
        continue;
    return x;
}

/* No cleanup needed for hardware generator */
void hardware_rand64_fini(void)
{
    /* Nothing to do */
}