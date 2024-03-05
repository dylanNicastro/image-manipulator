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
int colorcount = 0;
int *colordata;
int *sbudata;
int **filedata;
void load(char ipath[]) {
    char *dot = strrchr(ipath, '.');
    // PPM FILE
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
    // SBU FILE
    else if (dot && !strcmp(dot, ".sbu")) {
        FILE *inputfile;
        inputfile = fopen(ipath, "r");
        fscanf(inputfile, "%*[^\n]\n");
        fscanf(inputfile, "%d %d\n", &filecols, &filerows);
        fscanf(inputfile, "%d ", &colorcount);
        colordata = malloc(colorcount*3*sizeof(int));
        for (int i = 0; i < colorcount*3; i++) {
            fscanf(inputfile,"%d ",&colordata[i]);
        }
        fscanf(inputfile,"\n");
        
        sbudata = malloc(filerows*filecols*sizeof(int));
        int i = 0;
        int j = 0;
        for (i = 0; i < filerows*filecols; i++) {
            int n;
            int r;
            if (fscanf(inputfile,"%d ", &sbudata[i]) == 0) {
                fscanf(inputfile,"*%d %d", &n, &r);
                for (j = 0; j < n; j++) {
                    sbudata[i+j] = r;
                }
                i = i + n - 1;
            }
        }
        fclose(inputfile);
    }
}


void save(char opath[]) {
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
        free(filedata);
    }
    // SBU FILE
    else if (dot && !strcmp(dot, ".sbu")) {
        FILE *outputfile;
        outputfile = fopen(opath, "w");


        fprintf(outputfile,"SBU\n%d %d\n%d\n", filecols, filerows, colorcount);

        int i;
        for (i = 0; i < 3*colorcount; i++) {
            fprintf(outputfile,"%d ", colordata[i]);
        }
        fprintf(outputfile,"\n");
        free(colordata);

        int n;
        for (i = 0; i < filerows*filecols; i++) {
            n = 1;
            int r = sbudata[i];
            while (sbudata[i] == sbudata[i+1] && i < filerows*filecols-1) {
                n++;
                i++;
            }
            if (n > 1) {
                fprintf(outputfile,"*%d %d ",n,r);
                n = 1;
            }
            else {
                fprintf(outputfile,"%d ",sbudata[i]);
            }
        }

        fclose(outputfile);
        free(sbudata);
    }
}

void convert(int convertfrom, int convertto) {
    if (convertfrom == SBUTYPE && convertto == PPMTYPE) {
        filecols = filecols * 3;
        filedata = malloc(filerows*sizeof(int*));
        for (int i = 0; i < filerows; i++) {
            filedata[i] = malloc(filecols*sizeof(int));
        }
        int k = 0;
        for (int i = 0; i < filerows; i++) {
            for (int j = 0; j < filecols; j++) {
                filedata[i][j] = colordata[3*sbudata[k]];
                filedata[i][j+1] = colordata[3*sbudata[k]+1];
                filedata[i][j+2] = colordata[3*sbudata[k]+2];
                k++;
                j = j + 2;
            }
        }
        free(colordata);
        free(sbudata);
    }
    else if (convertfrom == PPMTYPE && convertto == SBUTYPE) {
        colordata = malloc(3*sizeof(int));
        sbudata = malloc(filerows*filecols*sizeof(int));

        // create color table
        int tempcount = 0;
        colorcount = 1;
        colordata = malloc(colorcount*3*sizeof(int));
        colordata[0] = filedata[0][0];
        colordata[1] = filedata[0][1];
        colordata[2] = filedata[0][2];
        sbudata[0] = 0;
        int j = 3;
        for (int i = 0; i < filerows; i++) {
            for (; j < filecols; j++) {
                int checkrgbexists = 0;
                for (tempcount = 0; tempcount < colorcount && checkrgbexists == 0; tempcount++) {
                    if (colordata[3*tempcount] == filedata[i][j] && colordata[3*tempcount+1] == filedata[i][j+1] && colordata[3*tempcount+2] == filedata[i][j+2]) {
                        checkrgbexists++;
                        sbudata[i*(filecols/3)+(j/3)] = tempcount;
                    }
                }
                if (checkrgbexists == 0) {
                    colorcount++;
                    colordata = realloc(colordata, colorcount*3*sizeof(int));
                    colordata[3*(colorcount-1)] = filedata[i][j];
                    colordata[3*(colorcount-1)+1] = filedata[i][j+1];
                    colordata[3*(colorcount-1)+2] = filedata[i][j+2];
                    sbudata[i*(filecols/3)+(j/3)] = colorcount-1;
                }
                j = j + 2;
            }
            j = 0;
        }

        filecols = filecols / 3;
        free(filedata);
    }
}

void copypaste(int crow, int ccol, int cwidth, int cheight, int prow, int pcol) {
    filecols = filecols/3;
    (void)prow;
    (void)pcol;
    // ccol is first column to be copied
    // crow is first row to be copied
    int copyendcol = ccol+cwidth-1; // last column to be copied
    int copyendrow = crow+cheight-1; // last row to be copied
    // cwidth is amount of columns to be copied
    // cheight is amount of rows to be copied

    if (copyendcol >= filecols) { // if last column is outside of image
        copyendcol = filecols - 1; // set last column to rightmost column of image
    }
    if (copyendrow >= filerows) { // if last row is outside of image
        copyendrow = filerows - 1; // set last row to bottom row of image
    }

    int pixelstocopy = cwidth*cheight; // width * height of copy rectangle

    int *copydata = malloc(pixelstocopy*3*sizeof(int)); // allocate memory for amount of pixels * 3 (integers for each pixel)
    
    int currentrow;
    int currentcol;
    int i = 0;
    for (currentrow = crow; currentrow <= copyendrow; currentrow++) { // for each row, from the first row to be copied up to the last row to be copied:
        for (currentcol = ccol*3; currentcol <= copyendcol*3+2; currentcol++) { // for each column, starting from the first integer to be copied in the row to the last integer to be copied in the row:
            copydata[i] = filedata[currentrow][currentcol];
            i++;
        }
    }
    // copydata array is now properly created, with indices 0 through (pixelstocopy*3 - 1)
    //printf("copyendrow: %d\ncopyendcol: %d\n",copyendrow,copyendcol);

    // pcol is first column to be pasted into
    // prow is first row to be pasted into
    int pasteendcol = pcol+cwidth-1; // last column to be pasted into
    int pasteendrow = prow+cheight-1; // last row to be pasted into
    //printf("pasteendrow: %d\npasteendcol: %d\n",pasteendrow,pasteendcol);
    if (pasteendcol > copyendcol-ccol+1) { // if the last column to be pasted into is greater than the amount of columns in the copy: (200-180)
        pasteendcol = pcol+copyendcol-ccol;
    }
    if (pasteendrow > copyendrow-crow+1) {
        pasteendrow = prow+copyendrow-crow;
    }
    //printf("pasteendrow: %d\npasteendcol: %d\n",pasteendrow,pasteendcol);

    if (pasteendcol >= filecols) { // if last column is outside of image
        pasteendcol = filecols-1; // set last column to rightmost column of image
    }
    if (pasteendrow >= filerows) { // if last row is outside of image
        pasteendrow = filerows-1; // set last row to bottom row of image
    }
    //printf("pasteendrow: %d\npasteendcol: %d\n",pasteendrow,pasteendcol);

    //printf("Last pixel index: %d %d\n", pasteendrow,pasteendcol);

    int expectedintsperrow = (copyendcol-ccol+1)*3;
    //printf("%d\n",expectedintsperrow);

    i = 0;
    for (currentrow = prow; currentrow <= pasteendrow; currentrow++) { // for each row, from the first row to be copied up to the last row to be copied:
        for (currentcol = pcol*3; currentcol <= pasteendcol*3+2; currentcol++) { // for each column, starting from the first integer to be copied in the row to the last integer to be copied in the row:
            filedata[currentrow][currentcol] = copydata[i];
            i++;
        }
        //printf("%d < %d?\n",i,expectedintsperrow*(currentrow-prow+1));
        if (i < expectedintsperrow*(currentrow-prow+1)) {
            i = expectedintsperrow*(currentrow-prow+1);
        }
        //printf("Expected filedata[%d][%d]: %d   -   Actual: %d\n",currentrow,currentcol,copydata[i-1],filedata[currentrow][currentcol]);
    }
    //printf("%d\n",i);
    




    filecols = filecols*3;
    free(copydata);
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

    if (inputfiletype == PPMTYPE) {
        load(ipath);
    }
    if (inputfiletype == SBUTYPE) {
        load(ipath);
        convert(SBUTYPE, PPMTYPE);
    }
    // FILE IS NOW LOADED INTO MEMORY AS PPM FILEDATA
    // CARRY OUT OPERATIONS
    if (cflag == 1 && pflag == 1) {
        copypaste(crow,ccol,cwide,chigh,prow,pcol);
    }

    // AFTER ALL OPERATIONS OUTPUT THE FILE
    if (outputfiletype == PPMTYPE) {
        save(opath);
    }
    if (outputfiletype == SBUTYPE) {
        convert(PPMTYPE, SBUTYPE);
        save(opath);
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
