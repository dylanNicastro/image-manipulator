#include "hw2.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h> 

int main(int argc, char **argv) {
    extern char *optarg;
    extern int optind;
    extern int optopt;
    int currentCase;
    int err = 0;
    int iflag = 0; // 1 mandatory
    int oflag = 0; // 1 mandatory
    int cflag = 0; // optional flag, at most 1
    int pflag = 0; // can only be used if c flag used, at most 1
    int rflag = 0; // optional flag, at most 1
    char *ipath;
    char *opath;
    char *coptions;
    char *poptions;
    char *roptions;

    while ((currentCase = getopt(argc, argv, "i:o:c:p:r:")) != -1 && err != MISSING_ARGUMENT) {
        switch (currentCase) { // check what current arg is
            case 'i':
                iflag++;
                ipath = optarg;
                if (*ipath == '-') {
                    err = MISSING_ARGUMENT;
                }
                break;
            case 'o':
                oflag++;
                opath = optarg;
                if (*opath == '-') {
                    err = MISSING_ARGUMENT;
                }
                break;
            case 'c':
                cflag++;
                coptions = optarg;
                if (*coptions == '-') {
                    err = MISSING_ARGUMENT;
                }
                break;
            case 'p':
                pflag++;
                poptions = optarg;
                if (*poptions == '-') {
                    err = MISSING_ARGUMENT;
                }
                break;
            case 'r':
                rflag++;
                roptions = optarg;
                break;
            case '?':
                if (optopt == 'r') {
                    err = MISSING_ARGUMENT;
                }
                else {
                    err = UNRECOGNIZED_ARGUMENT;
                }
                break;
        }
    }

    if (iflag == 0 || oflag == 0 || err == MISSING_ARGUMENT){ // i and o are mandatory
        return MISSING_ARGUMENT;
    }
    if (err == UNRECOGNIZED_ARGUMENT) {
        return UNRECOGNIZED_ARGUMENT;
    }
    if (iflag > 1 || oflag > 1 || cflag > 1 || pflag > 1 || rflag > 1) {
        return DUPLICATE_ARGUMENT;
    }

    FILE *inputfile;
    inputfile = fopen(ipath, "r");
    if (inputfile == NULL) {
        return INPUT_FILE_MISSING;
    }
    fclose(inputfile);

    FILE *outputfile;
    outputfile = fopen(opath, "w");
    if (outputfile == NULL) {
        return OUTPUT_FILE_UNWRITABLE;
    }
    fclose(outputfile);

    if (pflag == 1 && cflag == 0) {
        return C_ARGUMENT_MISSING;
    }

    if (cflag == 1) {
        char *coptionsptr = strtok(coptions, ",");
        int i = 0;
        while (coptionsptr != NULL) {
            coptionsptr = strtok(NULL, ",");
            i++;
        }
        if (i != 4) {
            return C_ARGUMENT_INVALID;
        }
    }

    if (pflag == 1) {
        char *poptionsptr = strtok(poptions, ",");
        int i = 0;
        while (poptionsptr != NULL) {
            poptionsptr = strtok(NULL, ",");
            i++;
        }
        if (i != 2) {
            return P_ARGUMENT_INVALID;
        }
    }

    if (rflag == 1) {
        char *roptionsptr = strtok(roptions, ",");
        int i = 1;
        FILE *fontfile;
        roptionsptr = strtok(NULL, ",");
        printf("%s\n",roptionsptr);
        fontfile = fopen(roptionsptr, "r");
        while (roptionsptr != NULL) {
            roptionsptr = strtok(NULL, ",");
            i++;
        }
        if (i != 5) {
            return R_ARGUMENT_INVALID;
        }
        if (fontfile == NULL) {
            return R_ARGUMENT_INVALID;
        }
        fclose(fontfile);
    }

    return 0;

}
