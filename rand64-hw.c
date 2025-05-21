#include "rand64-hw.h"
#include <cpuid.h>
#include <immintrin.h>
#include <stdbool.h>

/* Description of the current CPU. */
struct cpuid { unsigned eax, ebx, ecx, edx; };

/* Return information about the CPU. */
static struct cpuid cpuid(unsigned int leaf, unsigned int subleaf) {
    struct cpuid result;
    asm ("cpuid"
         : "=a" (result.eax), "=b" (result.ebx),
           "=c" (result.ecx), "=d" (result.edx)
         : "a" (leaf), "c" (subleaf));
    return result;
}

bool rdrand_supported(void) {
    struct cpuid extended = cpuid(1, 0);
    return (extended.ecx & bit_RDRND) != 0;
}

void hardware_rand64_init(void) {
    // Nothing to initialize
}

unsigned long long hardware_rand64(void) {
    unsigned long long int x;
    while (!_rdrand64_step(&x))
        continue;
    return x;
}

void hardware_rand64_fini(void) {
    // Nothing to clean up
}