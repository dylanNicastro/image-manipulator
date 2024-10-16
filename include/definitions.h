#define MISSING_ARGUMENT 1
#define UNRECOGNIZED_ARGUMENT 2
#define DUPLICATE_ARGUMENT 3
#define INPUT_FILE_MISSING 4
#define OUTPUT_FILE_UNWRITABLE 5
#define C_ARGUMENT_MISSING 6
#define C_ARGUMENT_INVALID 7
#define P_ARGUMENT_INVALID 8
#define R_ARGUMENT_INVALID 9

void load(char ipath[]);
void save(char opath[]);
void convert(int convertfrom, int convertto);
void copypaste(int crow, int ccol, int cwidth, int cheight, int prow, int pcol);
void writemsg(char message[], char pathtofont[], int fontsize, int startrow, int startcol);
