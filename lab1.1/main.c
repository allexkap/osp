#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include "nrftw.h"



char debug_mode = 0;
char *pattern = NULL;

void walk_func(const char *path) {

    FILE* file = fopen(path, "r");
    if (!file) {
        fprintf(stderr, "Error opening file %s\n", path);
        return;
    }

    if (debug_mode) fprintf(stdout, "Searching in file %s\n", path);

    long pos = 0;
    while (!feof(file)) {
        if (getc(file) == pattern[pos]) {
            if (!pattern[++pos]) {
                if (debug_mode) fprintf(stdout, "Match found at position %ld: ", ftell(file)-pos);
                char *rp = realpath(path, NULL);
                fprintf(stdout, "%s\n", rp);
                free(rp);
                break;
            }
        } else if (pos) {
            fseek(file, -pos, SEEK_CUR);
            pos = 0;
        }
    }
    fclose(file);
}


int main(int argc, char **argv) {

    if (getenv("LAB11DEBUG")) debug_mode = 1;

    while (1) {
        static const struct option long_options[] = {
            {"help",    no_argument, NULL, 'h'},
            {"version", no_argument, NULL, 'v'},
            {0, 0, 0, 0}
        };
        switch (getopt_long(argc, argv, "hv", long_options, NULL)) {
            case 'h':
                fprintf(stdout, "help\n");
                return 0;
            case 'v':
                fprintf(stdout, "version\n");
                return 0;
            case -1:
                goto options_end;
            default:
                return 1;
        }
    }
    options_end:

    char *dir = NULL;
    for (int i = 1, j = 0; i < argc; ++i) {
        if (argv[i][0] == '-') continue;
        switch (j++) {
            case 0:
                dir = argv[i];
                break;
            case 1:
                pattern = argv[i];
                break;
            default:
                fprintf(stderr, "Too many arguments\n");
                return 1;
        }
    }
    if (!pattern) {
        fprintf(stderr, "Not enough arguments\n");
        return 1;
    }

    nrftw(dir, &walk_func);

    return 0;
}
