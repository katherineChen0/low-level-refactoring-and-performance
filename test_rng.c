#include "rand64-hw.h"
#include "rand64-sw.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEST_ITERATIONS 1000

void test_rng(const char *name, 
              unsigned long long (*rng)(void),
              void (*init)(void),
              void (*fini)(void)) {
    printf("Testing %s...\n", name);
    
    init();
    
    // Test basic functionality
    unsigned long long val = rng();
    if (val == 0) {
        printf("  FAIL: First value was zero\n");
        exit(1);
    }
    
    // Test for repeated values
    int duplicates = 0;
    unsigned long long prev = val;
    for (int i = 0; i < TEST_ITERATIONS; i++) {
        val = rng();
        if (val == prev) duplicates++;
        prev = val;
    }
    
    if (duplicates > TEST_ITERATIONS/100) {  // Allow 1% duplicates
        printf("  FAIL: Too many duplicates (%d/%d)\n", duplicates, TEST_ITERATIONS);
        exit(1);
    }
    
    fini();
    printf("  PASS: %s\n", name);
}

int main() {
    // Test hardware RNG if available
    if (rdrand_supported()) {
        test_rng("hardware RNG", hardware_rand64, hardware_rand64_init, hardware_rand64_fini);
    } else {
        printf("Skipping hardware RNG (not supported)\n");
    }
    
    // Test software RNGs
    setMrand48();
    test_rng("software RNG (mrand48_r)", software_rand64, software_rand64_init, software_rand64_fini);
    
    setFilename("/dev/urandom");
    test_rng("file-based RNG (/dev/urandom)", software_rand64, software_rand64_init, software_rand64_fini);
    
    printf("All RNG tests passed\n");
    return 0;
}