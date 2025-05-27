#include "rand64-sw.h"

/* Software implementation.  */
/* Input stream containing random bytes.  */
FILE *urandstream;
char *filename = "/dev/random";

/* Initialize the software rand64 implementation.  */
void
software_rand64_init (void)
{
  urandstream = fopen (filename, "r");
  if (! urandstream) {
    fprintf(stderr, "urandstream error in initialization!\n");
    abort();
  }
}

/* Return a random value, using software operations.  */
unsigned long long
software_rand64 (void)
{
  unsigned long long int x;
  if (fread (&x, sizeof x, 1, urandstream) != 1) {
    fprintf(stderr, "fread error in getting random value! (no more data to read from)\n");
    abort();
  }
  return x;
}

/* Finalize the software rand64 implementation.  */
void
software_rand64_fini (void)
{
  fclose (urandstream);
}

void
setFilename (char* newfile)
{
  filename = newfile;
  // printf("Filename has been set!!!");
}