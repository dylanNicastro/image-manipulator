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
    static char usage[] = "usage: -i ipath -o opath [-c coptions] [-p poptions] [-r roptions]\n";

    while ((currentCase = getopt(argc, argv, "i:o:c:p:r:")) != -1) {
        switch (currentCase) { // check what current arg is
            case 'i':
                iflag++;
                ipath = optarg;
                break;
            case 'o':
                oflag++;
                opath = optarg;
                break;
            case 'c':
                cflag++;
                coptions = optarg;
                if (*coptions == '-') {
                    printf("Mising C Arguments");
                    return MISSING_ARGUMENT;
                }
                break;
            case 'p':
                pflag++;
                poptions = optarg;
                if (*poptions == '-') {
                    printf("Mising P Arguments");
                    return MISSING_ARGUMENT;
                }
                break;
            case 'r':
                rflag++;
                roptions = optarg;
                if (*roptions == '-') {
                    printf("Missing R Arguments");
                    return MISSING_ARGUMENT;
                }
                break;
            case '?':
                err = 1;
                break;
        }
    }

    // unsigned char coptionsarr[4];
    // int i = 0;
    // for (char *delimtest = strtok(coptions,","); delimtest != NULL; delimtest = strtok(NULL, ","))
    // {
    //     coptionsarr[i] = *delimtest;
    //     i++;
    // }
    // printf("%d\n",coptionsarr[0]);


    // for (int i = 0; i < 4; i++) {
    //     printf("%d ", *coptions);
    //     coptions++;
    // }
    // coptions = coptions-4;
    // printf("%d ", *coptions);

    if (iflag == 0 || oflag == 0 ){ // i and o are mandatory
        return MISSING_ARGUMENT;
    }
    if (err == 1) {
        return UNRECOGNIZED_ARGUMENT;
    }
    if (iflag > 1 || oflag > 1 || cflag > 1 || pflag > 1 || rflag > 1) {
        return DUPLICATE_ARGUMENT;
    }
    if (pflag == 1 && cflag == 0) {
        return C_ARGUMENT_MISSING;
    }
    







    if (pflag || cflag || rflag || ipath || opath || coptions || poptions || roptions) {
        printf("%s", usage);
        return 0;
    }

}
