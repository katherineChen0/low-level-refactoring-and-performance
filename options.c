#include "options.h"
#include "rand64-sw.h"

int process_options(int argc, char **argv, long long* nbytes, int* inputChoice, int* outputChoice, long long* chunksize) {
    int opt;
    int inputFlag = 0;
    int outputFlag = 0;
    int nonFlaggedCount = 0;
    char *inputArg = NULL;
    char *outputArg = NULL;

    // Parse the flags and set the options to their respective arguments
    while ((opt = getopt(argc, argv, "i:o:")) != -1) {
        switch (opt) {
            case 'i':
                inputFlag++;
                inputArg = optarg;
                break;
            case 'o':
                outputFlag++;
                outputArg = optarg;
                break;
            case '?':
                fprintf(stderr, "Usage: %s [-i input] [-o output] NBYTES\n", argv[0]);
                return 1;
        }
    }

    // Parse non-flagged arguments (the number of bytes)
    for (int i = optind; i < argc; i++) {
        if (nonFlaggedCount >= 1) {
            fprintf(stderr, "Too many arguments provided, please provide at most one positive integer value for NBYTES\n");
            return 1;
        }
        nonFlaggedCount++;
        char *endptr;
        errno = 0;
        *nbytes = strtoll(argv[i], &endptr, 10);
        
        if (errno || *endptr || *nbytes < 0) {
            fprintf(stderr, "%s: invalid NBYTES argument '%s'\n", argv[0], argv[i]);
            return 1;
        }
    }

    // No argument provided
    if (nonFlaggedCount == 0) {
        fprintf(stderr, "Please provide a positive integer value for NBYTES\n");
        return 1;
    }

    // Too many options
    if (inputFlag > 1 || outputFlag > 1) {
        fprintf(stderr, "Too many options provided, please only provide at most one -i and one -o\n");
        return 1;
    }

    // Process Input Flag
    if (inputFlag) {
        if (strcmp(inputArg, "rdrand") == 0) {
            *inputChoice = 0;
        }
        else if (strcmp(inputArg, "mrand48_r") == 0) {
            *inputChoice = 1;
        }
        else if (inputArg[0] == '/' || strcmp(inputArg, "rdrand") != 0 && strcmp(inputArg, "mrand48_r") != 0) {
            FILE *file = fopen(inputArg, "r");
            if (file == NULL) {
                fprintf(stderr, "Failed to open file: %s\n", inputArg);
                return 1;
            }
            setFilename(inputArg);
            fclose(file);
              *inputChoice = 2;
        }
        else {
            fprintf(stderr, "Invalid input argument for -i: %s\n", inputArg);
            return 1;
        }
    }

    // Process Output Flag
    if (outputFlag) {
        if (strcmp(outputArg, "stdio") == 0) {
            *outputChoice = 0;
        }
        else {
            char *endptr2;
            errno = 0;
            *chunksize = strtoll(outputArg, &endptr2, 10);
            
            if (errno || *endptr2 || *chunksize <= 0) {
                fprintf(stderr, "Invalid output argument for -o: %s\n", outputArg);
                return 1;
            }
            *outputChoice = 1;
        }
    }

    return 0;
}
