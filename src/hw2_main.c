#include "hw2.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h> 
#define PPMTYPE 1
#define SBUTYPE 2

int filecols = 0;
int filerows = 0;
int **filedata;
void load(char ipath[]) {
    char *dot = strrchr(ipath, '.');
    if (dot && !strcmp(dot, ".ppm")) {
        FILE *inputfile;
        inputfile = fopen(ipath, "r");
        fscanf(inputfile, "%*[^\n]\n");
        fscanf(inputfile, "%d %d\n", &filecols, &filerows);
        fscanf(inputfile, "%*[^\n]\n");

        filecols = filecols*3;

        filedata = malloc(filerows*sizeof(int*));
        for (int i = 0; i < filerows; i++) {
            filedata[i] = malloc(filecols*sizeof(int));
        }
        int i = 0;
        int j = 0;
        for (i = 0; i < filerows; i++) {
            for (j = 0; j < filecols; j++) {
                fscanf(inputfile,"%d ", &filedata[i][j]);
            }
        }
        fclose(inputfile);
    }
}


void save(char opath[], int **imgdata) {
    (void)opath;
    (void)imgdata;
    char *dot = strrchr(opath, '.');
    if (dot && !strcmp(dot, ".ppm")) {
        FILE *outputfile;
        outputfile = fopen(opath, "w");

        filecols = filecols/3;
        fprintf(outputfile,"P3\n%d %d\n255\n", filecols, filerows);

        int i = 0;
        int j = 0;
        for (i = 0; i < filerows; i++) {
            for (j = 0; j < filecols*3; j++) {
                fprintf(outputfile,"%d ",filedata[i][j]);
            }
            fprintf(outputfile,"\n");
        }
        for (i = 0; i < filerows; i++) {
            free(filedata[i]);
        }
        fclose(outputfile);
    }
    free(filedata);
}

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

    int crow = 0;
    int ccol = 0;
    int cwide = 0;
    int chigh = 0;
    if (cflag == 1) {
        char *temp = calloc(strlen(coptions)+1, sizeof(char));
        strcpy(temp, coptions);
        char *coptionsptr = strtok(temp, ",");
        int i = 0;
        while (coptionsptr != NULL) {
            coptionsptr = strtok(NULL, ",");
            i++;
        }
        free(temp);
        if (i != 4) {
            return C_ARGUMENT_INVALID;
        }

        char *newptr = strtok(coptions, ",");
        crow = atoi(newptr);
        newptr = strtok(NULL, ",");
        ccol = atoi(newptr);
        newptr = strtok(NULL, ",");
        cwide = atoi(newptr);
        newptr = strtok(NULL, ",");
        chigh = atoi(newptr);
    }
    
    int prow = 0;
    int pcol = 0;
    if (pflag == 1) {
        char *temp = calloc(strlen(poptions)+1, sizeof(char));
        strcpy(temp, poptions);
        char *poptionsptr = strtok(temp, ",");
        int i = 0;
        while (poptionsptr != NULL) {
            poptionsptr = strtok(NULL, ",");
            i++;
        }
        free(temp);
        if (i != 2) {
            return P_ARGUMENT_INVALID;
        }

        char *newptr = strtok(poptions, ",");
        prow = atoi(newptr);
        newptr = strtok(NULL, ",");
        pcol = atoi(newptr);
    }


    char *rmessage = 0;
    char *rpathtofont = 0;
    int rfontsize = 0;
    int rrow = 0;
    int rcol = 0;
    if (rflag == 1) {
        char *temp = calloc(strlen(roptions)+1, sizeof(char));
        strcpy(temp, roptions);
        char *roptionsptr = strtok(temp, ",");
        int i = 1;
        FILE *fontfile;
        roptionsptr = strtok(NULL, ",");
        fontfile = fopen(roptionsptr, "r");
        while (roptionsptr != NULL) {
            roptionsptr = strtok(NULL, ",");
            i++;
        }
        free(temp);
        if (i != 5) {
            return R_ARGUMENT_INVALID;
        }
        if (fontfile == NULL) {
            return R_ARGUMENT_INVALID;
        }
        fclose(fontfile);

        char *newptr = strtok(roptions, ",");
        rmessage = newptr;
        newptr = strtok(NULL, ",");
        rpathtofont = newptr;
        newptr = strtok(NULL, ",");
        rfontsize = atoi(newptr);
        newptr = strtok(NULL, ",");
        rrow = atoi(newptr);
        newptr = strtok(NULL, ",");
        rcol = atoi(newptr);
    }

    int inputfiletype = 0;
    char *dot = strrchr(ipath, '.');
    if (dot && !strcmp(dot, ".ppm")) {
        inputfiletype = PPMTYPE;
    }
    else if (dot && !strcmp(dot, ".sbu")) {
        inputfiletype = SBUTYPE;
    }

    int outputfiletype = 0;
    dot = strrchr(opath, '.');
    if (dot && !strcmp(dot, ".ppm")) {
        outputfiletype = PPMTYPE;
    }
    else if (dot && !strcmp(dot, ".sbu")) {
        outputfiletype = SBUTYPE;
    }

    if (inputfiletype == outputfiletype) {
        load(ipath);
        save(opath, filedata);
    }

    // temporary to prevent errors
    (void)crow;
    (void)ccol;
    (void)cwide;
    (void)chigh;
    (void)prow;
    (void)pcol;
    (void)rmessage;
    (void)rpathtofont;
    (void)rfontsize;
    (void)rrow;
    (void)rcol;
    return 0;

}
