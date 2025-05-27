#include "rand64-hw.h"
#include "rand64-sw.h"
#include <stdio.h>
#include <string.h>

void test_rng(const char *name, unsigned long long (*rng)(void), void (*init)(void), void (*fini)(void)) {
    init();
    printf("Testing %s...\n", name);
    
    // Test basic functionality
    unsigned long long prev = rng();
    int count = 0;
    for (int i = 0; i < 100; i++) {
        unsigned long long current = rng();
        if (current != prev) count++;
        prev = current;
    }
    
    printf("  Changed values: %d/100\n", count);
    fini();
}

int main() {
    // Test hardware RNG if available
    if (rdrand_supported()) {
        test_rng("hardware RNG", hardware_rand64, hardware_rand64_init, hardware_rand64_fini);
    } else {
        printf("Hardware RNG not supported\n");
    }
    
    // Test software RNGs
    test_rng("mrand48_r", software_rand64, software_rand64_init, software_rand64_fini);
    
    return 0;
}