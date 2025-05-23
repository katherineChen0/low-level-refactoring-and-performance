#include "rand64-sw.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>

static FILE *urandstream = NULL;
static char *random_source = NULL;
static struct drand48_data drand_buf;

void software_rand64_init(const char *source) {
    if (strcmp(source, "mrand48_r") == 0) {
        random_source = strdup(source);
        srand48_r((long)(time(NULL) ^ getpid()), &drand_buf);
    } else {
        // File-based random source
        urandstream = fopen(source, "r");
        if (!urandstream) {
            fprintf(stderr, "Failed to open %s: %s\n", source, strerror(errno));
            abort();
        }
    }
}

unsigned long long software_rand64(void) {
    if (random_source && strcmp(random_source, "mrand48_r") == 0) {
        long int r1, r2;
        mrand48_r(&drand_buf, &r1);
        mrand48_r(&drand_buf, &r2);
        return ((unsigned long long)(uint32_t)r1 << 32) | (uint32_t)r2;
    } else {
        unsigned long long x;
        if (fread(&x, sizeof x, 1, urandstream) != 1) {
            if (feof(urandstream))
                fprintf(stderr, "EOF reading random source\n");
            else
                perror("fread");
            abort();
        }
        return x;
    }
}

void software_rand64_fini(void) {
    if (urandstream) {
        fclose(urandstream);
        urandstream = NULL;
    }
    if (random_source) {
        free(random_source);
        random_source = NULL;
    }
}