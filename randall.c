

#include "options.h"
#include "output.h"
#include "rand64-hw.h"
#include "rand64-sw.h"

/* Main program, which outputs N bytes of random data.  */
int
main (int argc, char **argv)
{
  /* Check arguments.  */
  // bool valid = false;
  long long nbytes;
  long long chunksize;
  int inputChoice = 0; // 0 (default) = rdrand, 1 = mrand48_r, 2 = /F
  int outputChoice = 0; // 0 (default) = stdio, 1 = N

  // Process the options
  if (process_options(argc, argv, &nbytes, &inputChoice, &outputChoice, &chunksize)) {
    // perror("PROCESS OPTIONS FAILED\n");
    return 1;
  }

  /* If there's no work to do, don't worry about which library to use.  */
  if (nbytes == 0)
    return 0;

  /* Now that we know we have work to do, arrange to use the
     appropriate library.  */
  void (*initialize) (void);
  unsigned long long (*rand64) (void);
  void (*finalize) (void);

  if (inputChoice == 0) { // rdrand
    if (rdrand_supported()) {
      initialize = hardware_rand64_init;
      rand64 = hardware_rand64;
      finalize = hardware_rand64_fini;
    } else {
      fprintf(stderr, "rdrand hardware random number generator currently not available!\n");
      return 1;
    }
  }
  else if (inputChoice == 1) { // mrand_48
    initialize = software_rand48_init;
    rand64 = software_rand48;
    finalize = software_rand48_fini;
    // printf("mrand48 set\n");
  }
  else if (inputChoice == 2) { // file
    initialize = software_rand64_init;
    rand64 = software_rand64;
    finalize = software_rand64_fini;
  }

  initialize ();
  int wordsize = sizeof rand64 ();

  if (outputChoice == 0) {
     do {
        unsigned long long x = rand64 ();
        int outbytes = nbytes < wordsize ? nbytes : wordsize;
        if (!writebytes (x, outbytes)) {
          fprintf(stderr, "Output error!");
          return 1;
        }
        nbytes -= outbytes;
    }
    while (0 < nbytes);

    if (fclose (stdout) != 0) {
      fprintf(stderr, "Output error!");
      return 1;
    }
  } else if (outputChoice == 1) {
    // printf("VALUE: %d", chunksize);

    int offset = 0;
    unsigned long long x;

    // write in chunks of chunksize at a time
    while (nbytes > 0) {
        size_t outbytes = (nbytes >= chunksize) ? chunksize : nbytes;                                                         
        x = rand64 ();

        memcpy(stdout, (const char*)&x + offset, outbytes);
                                                                                              
        if (write(1, stdout, outbytes) < 0) {
          fprintf(stderr, "Output error!");
          return 1;
        }
                                                                                     
        offset += outbytes;
        nbytes -= outbytes;
    }

  }

  finalize ();
  return 0;
}