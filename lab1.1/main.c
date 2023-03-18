#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <dirent.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>



int path_push(char *path, size_t *pos, char *name) {
    if (*pos && *pos < PATH_MAX) path[(*pos)++] = '/';
    for (size_t i = 0; *pos < PATH_MAX; ++(*pos), ++i)
        if (!(path[*pos] = name[i])) return 0;
    return 1;
}


void path_pop(char *path, size_t *pos) {
    while (*pos && path[--(*pos)] != '/');
    path[*pos] = '\0';
}


void nonRecursiveWalker(void (*walk_func)(const char*)) {

    DIR *dirs[PATH_MAX/2];
    char path[PATH_MAX];
    size_t pos = 0, depth = 0;
    struct dirent *entry;

    dirs[depth++] = opendir(".");
    if (!dirs[depth-1]) {
        fprintf(stderr, "Error opening selected directory\n");
        return;
    }

    while (depth) {
        entry = readdir(dirs[depth-1]);

        if (entry) {
            if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
                continue;

            if (path_push(path, &pos, entry->d_name)) {
                path_pop(path, &pos);
                fprintf(stderr, "Error opening path %s/%s: File name too long\n", path, entry->d_name);
                break;
            }

            if (entry->d_type == DT_DIR) {
                dirs[depth++] = opendir(path);
                if (!dirs[depth-1]) {
                    fprintf(stderr, "Error opening directory %s: Not enough file descriptors (%ld)\n", path, depth);
                    break;
                }
                continue;
            }
            if (entry->d_type == DT_REG)
                walk_func(path);
        }
        else {
            closedir(dirs[--depth]);
        }
        path_pop(path, &pos);
    }

    while (depth) closedir(dirs[--depth]);
}



char debug_mode = 0;
char *pattern_wf = 0;

void walk_func(const char *path) {

    FILE* file = fopen(path, "r");
    if (!file) {
        fprintf(stderr, "Error opening file %s\n", path);
        return;
    }

    if (debug_mode) fprintf(stdout, "Searching in file %s\n", path);

    char ch;
    long pos = 0;
    while (!feof(file)) {
        if ((ch = getc(file)) == pattern_wf[pos]) {
            if (!pattern_wf[++pos]) {
                if (debug_mode) fprintf(stdout, "Match found at position %ld: ", ftell(file)-pos);
                char *rp = realpath(path, NULL);
                fprintf(stdout, "%s\n", rp);
                free(rp);
                break;
            }
        } else if (pos) {
            fseek(file, -pos, 1);
            pos = 0;
        }
    }
    fclose(file);
}


int main(int argc, char **argv) {

    while (1) {
        static const struct option long_options[] = {
            {"help",    0, 0, 'h'},
            {"version", 0, 0, 'v'},
            {0, 0, 0, 0}
        };
        switch (getopt_long(argc, argv, "hv", long_options, 0)) {
            case 'h':
                fprintf(stdout, "help\n");
                break;
            case 'v':
                fprintf(stdout, "version\n");
                break;
            case -1:
                goto options_end;
            default:
                return 1;
        }
    }
    options_end:

    for (int i = 1, j = 0; i < argc; ++i) {
        if (argv[i][0] == '-') continue;
        switch (j++) {
            case 0:
                if (chdir(argv[i]) == -1) {
                    fprintf(stderr, "Error opening directory %s\n", argv[i]);
                    return 1;
                }
                break;
            case 1:
                pattern_wf = argv[i];
                break;
            default:
                fprintf(stderr, "Too many arguments\n");
                return 1;
        }
    }
    if (!pattern_wf) {
        fprintf(stderr, "Not enough arguments\n");
        return 1;
    }

    if (getenv("LAB11DEBUG")) debug_mode = 1;

    nonRecursiveWalker(&walk_func);

    return 0;
}
