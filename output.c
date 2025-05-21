#include "output.h"
#include "options.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>

static char *output_buffer = NULL;
static size_t buffer_size = 0;
static size_t buffer_pos = 0;
static int output_fd = STDOUT_FILENO;

bool initialize_output(const struct options *opts) {
    if (strcmp(opts->output_method, "block") == 0) {
        buffer_size = opts->output_block_size;
        output_buffer = malloc(buffer_size);
        if (!output_buffer) {
            perror("malloc");
            return false;
        }
    }
    return true;
}

bool write_bytes(unsigned long long x, int nbytes) {
    if (output_buffer) {
        // Buffered output mode
        for (int i = 0; i < nbytes; i++) {
            output_buffer[buffer_pos++] = x & 0xFF;
            x >>= CHAR_BIT;
            
            if (buffer_pos >= buffer_size) {
                ssize_t bytes_written = write(output_fd, output_buffer, buffer_size);
                if (bytes_written < 0) {
                    perror("write");
                    return false;
                }
                buffer_pos = 0;
            }
        }
    } else {
        // Stdio mode
        do {
            if (putchar(x) < 0)
                return false;
            x >>= CHAR_BIT;
            nbytes--;
        } while (0 < nbytes);
    }
    return true;
}

bool flush_remaining_bytes(void) {
    if (output_buffer && buffer_pos > 0) {
        ssize_t bytes_written = write(output_fd, output_buffer, buffer_pos);
        if (bytes_written < 0) {
            perror("write");
            return false;
        }
    }
    return true;
}

void finalize_output(void) {
    if (output_buffer) {
        free(output_buffer);
        output_buffer = NULL;
    }
}