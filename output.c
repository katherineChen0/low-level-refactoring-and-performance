#include "output.h"

bool writebytes(unsigned long long x, int nbytes)
{
    do {
        if (putchar(x) < 0)
            return false;
        x >>= CHAR_BIT;
        nbytes--;
    } while (0 < nbytes);
    return true;
}

bool writeblock(const void *buffer, size_t nbytes)
{
    const char *buf = (const char *)buffer;
    size_t written = 0;
    
    while (written < nbytes) {
        ssize_t result = write(STDOUT_FILENO, buf + written, nbytes - written);
        if (result < 0) {
            return false;
        }
        written += result;
    }
    return true;
}