#include "rand64-hw.h"
#include "rand64-sw.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEST_COUNT 10000

void test_rng(const char *name, 
              unsigned long long (*rng)(void), 
              void (*init)(void), 
              void (*fini)(void)) {
    init();
    printf("Testing %s...\n", name);
    
    // Test 1: Basic functionality
    unsigned long long val = rng();
    printf("  Basic test: %s\n", val != 0 ? "PASSED" : "FAILED");
    
    // Test 2: Value changes
    unsigned long long prev = val;
    int changes = 0;
    for (int i = 0; i < TEST_COUNT; i++) {
        val = rng();
        if (val != prev) changes++;
        prev = val;
    }
    float change_rate = (float)changes/TEST_COUNT;
    printf("  Change rate: %.2f%% (%s)\n", 
           change_rate*100, 
           change_rate > 0.9 ? "PASSED" : "FAILED");
    
    // Test 3: Bit distribution
    int bits[64] = {0};
    for (int i = 0; i < TEST_COUNT; i++) {
        val = rng();
        for (int b = 0; b < 64; b++) {
            if (val & (1ULL << b)) bits[b]++;
        }
    }
    
    int balanced_bits = 0;
    for (int b = 0; b < 64; b++) {
        float ratio = (float)bits[b]/TEST_COUNT;
        if (ratio > 0.4 && ratio < 0.6) balanced_bits++;
    }
    printf("  Balanced bits: %d/64 (%s)\n", 
           balanced_bits,
           balanced_bits > 50 ? "PASSED" : "FAILED");
    
    int passed = (val != 0) && (change_rate > 0.9) && (balanced_bits > 50);
    printf("  %s: %s\n\n", name, passed ? "RNG passed" : "RNG failed");
    
    fini();
    
    if (!passed) exit(1);
}

int main() {
    // Test hardware RNG if available
    if (rdrand_supported()) {
        test_rng("hardware RNG", hardware_rand64, hardware_rand64_init, hardware_rand64_fini);
    } else {
        printf("Hardware RNG not supported - skipping\n");
    }
    
    // Test mrand48 implementation
    setMrand48();
    test_rng("software RNG (mrand48)", software_rand64, software_rand64_init, software_rand64_fini);
    
    // Test file-based implementation
    setFilename("/dev/urandom");
    test_rng("file-based RNG (/dev/urandom)", software_rand64, software_rand64_init, software_rand64_fini);
    
    printf("All RNG tests completed\n");
    return 0;
}