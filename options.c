#include "options.h"
// #include "rand64-sw.c"

int process_options(int argc, char **argv, long long* nbytes, int* inputChoice, int* outputChoice, long long* chunksize) {
    int opt;
    // In C booleans do not exist unless <stdbool.h> is included, use int for backwards compatibility and efficiency
    int inputFlag = 0;
    int outputFlag = 0;
    int nonFlaggedCount = 0;
    // Similarly strings don't necessarily exist, use a pointer to a bunch of chars
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
                // Help option
                fprintf(stderr, "Usage: %s -i input -o output\n", argv[0]);
                return 1;
        }
    }

    // Parse non-flagged arguments (the number of bytes)
    for (int i = optind; i < argc; i++) {
        if (nonFlaggedCount >= 1) {
            fprintf(stderr, "Too many arguments provided, please provide at most one positive integer value for NBYTES");
            return 1;
        }
        // printf("Non-flagged argument %d: %s\n", nonFlaggedCount, argv[i]);
        nonFlaggedCount++;
        char *endptr;
        int valid = 0;
        errno = 0;
        *nbytes = strtoll (argv[i], &endptr, 10);
        // printf(nbytes);
        if (errno)
            perror (argv[i]);
        else
            valid = !*endptr && (0 <= *nbytes);

        if (!valid) {
            fprintf(stderr, "%s: usage: %s NBYTES\n", argv[0], argv[0]);
            return 1;
        }
    }

    // Error Checking...
    // No argument provided
    if (nonFlaggedCount == 0) {
        fprintf(stderr, "Please provide a positive integer value for NBYTES (./randall NBYTES)\n");
        return 1;
    }

    // Too many options
    if (inputFlag > 1 || outputFlag > 1) {
        fprintf(stderr, "Too many options provided, please only provide at most one -i and one -o\n");
        return 1;
    }

    // Process Input Flag
    if (inputFlag) {
        // printf("Processing input flag\n");
        if (strcmp(inputArg, "rdrand") == 0) {
            // printf("rdrand detected\n");
            *inputChoice = 0;
        }
        else if (strcmp(inputArg, "mrand48_r") == 0) {
            // printf("mrand48_r detected\n");
            *inputChoice = 1;
        }
        else if (inputArg[0] == '/') {
            // printf("Attempting file...\n");
            FILE *file = fopen(&inputArg[0], "r");
            if (file == NULL) {
                fprintf(stderr, "Failed to open file: %s (No such file or directory)\n", &inputArg[0]);
                return 1;
            }
            // printf("Input file %s", &inputArg[0]);
            setFilename(&inputArg[0]);
            fclose(file);
            *inputChoice = 2;
        }
        else {
            fprintf(stderr, "Invalid input argument for input (-i), please put rdrand, mrand48_r, or /F where F is your file\n");
            return 1;
        }
    }

    // Process Output Flag
    if (outputFlag) {
        // printf("Processing output flag\n");
        if (strcmp(outputArg, "stdio") == 0) {
            // printf("stdio detected\n");
            // stdio default
            outputChoice = 0;
        }
        else { // 
            // printf("Processing non stdio\n");
            char *endptr2;
            int valid2 = 0;
            *chunksize = strtoll(outputArg, &endptr2, 10);
            // printf(nbytes);
            if (errno)
                perror (outputArg);
            else
                valid2 = !*endptr2 && (0 <= *nbytes);

            if (!valid2) {
                fprintf(stderr, "Invalid input argument for output (-o), please use stdio or a positive integer N\n");
                return 1;
            }

            *outputChoice = 1;
        }
    }

    // if (inputFlag)
    //     printf("Input Flag Detected!\n");

    // if (outputFlag)
    //     printf("Output Flag Detected!\n");

    return 0;
}