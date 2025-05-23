#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "output.h"

static enum output_type output_mode;
static int block_size;
static char *output_buffer;
static int buffer_pos;

int init_output(struct randall_options *opts)
{
    output_mode = opts->output;
    block_size = opts->block_size;
    output_buffer = NULL;
    buffer_pos = 0;

    if (output_mode == OUTPUT_BLOCK) {
        output_buffer = malloc(block_size);
        if (!output_buffer) {
            fprintf(stderr, "randall: failed to allocate output buffer\n");
            return -1;
        }
    }

    return 0;
}

int write_bytes(const char *buf, int nbytes)
{
    if (output_mode == OUTPUT_STDIO) {
        /* Use stdio output */
        size_t written = fwrite(buf, 1, nbytes, stdout);
        return written == (size_t)nbytes ? nbytes : -1;
    } else {
        /* Use block output with write() system call */
        int bytes_written = 0;
        
        while (bytes_written < nbytes) {
            int bytes_to_copy = nbytes - bytes_written;
            int space_left = block_size - buffer_pos;
            
            if (bytes_to_copy > space_left) {
                bytes_to_copy = space_left;
            }
            
            memcpy(output_buffer + buffer_pos, buf + bytes_written, bytes_to_copy);
            buffer_pos += bytes_to_copy;
            bytes_written += bytes_to_copy;
            
            /* Flush buffer if full */
            if (buffer_pos == block_size) {
                ssize_t written = write(STDOUT_FILENO, output_buffer, block_size);
                if (written < 0) {
                    return -1;
                }
                /* Note: we don't consider partial writes as errors per assignment */
                buffer_pos = 0;
            }
        }
        
        return nbytes;
    }
}

void finalize_output(void)
{
    if (output_mode == OUTPUT_BLOCK && output_buffer) {
        /* Flush any remaining data in buffer */
        if (buffer_pos > 0) {
            write(STDOUT_FILENO, output_buffer, buffer_pos);
        }
        free(output_buffer);
        output_buffer = NULL;
    }
}