#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <dirent.h>
#include <unistd.h>


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


void walk_func(char *path, int depth) {
    printf("%s\n", path);
}

void nonRecursiveWalker() {

    DIR *dirs[PATH_MAX/2];
    char path[PATH_MAX];
    size_t pos = 0, depth = 0;
    struct dirent *entry;

    dirs[depth++] = opendir(".");
    if (!dirs[depth-1]) {
        fprintf(stderr, "Error opening this directory\n", path);
        return;
    }

    while (depth) {
        entry = readdir(dirs[depth-1]);

        if (entry) {
            if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
                continue;

            if (path_push(path, &pos, entry->d_name)) {
                fprintf(stderr, "Error opening path %s: File name too long\n", path);
                break;
            }

            if (entry->d_type == DT_DIR) {
                dirs[depth++] = opendir(path);
                if (!dirs[depth-1]) {
                    fprintf(stderr, "Error opening directory %s: Not enough file descriptors (%d)\n", path, depth);
                    break;
                }
                continue;
            }
            if (entry->d_type == DT_REG)
                walk_func(path, depth);
        }
        else {
            closedir(dirs[--depth]);
        }
        path_pop(path, &pos);
    }

    while (depth) closedir(dirs[--depth]);
}


int main(int argc, char **argv) {

    if (argc < 2) return 0;
    if (chdir(argv[1]) == -1) return 1;

    nonRecursiveWalker();

    return 0;
}
