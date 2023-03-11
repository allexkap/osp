#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <dirent.h>
#include <unistd.h>


void path_push(char *path, size_t *pos, char *name) {
    size_t i = 0;
    if (*pos) path[(*pos)++] = '/';
    while (path[*pos] = name[i]) ++(*pos), ++i;
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

    while (depth) {
        entry = readdir(dirs[depth-1]);

        if (entry) {
            if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
                continue;

            path_push(path, &pos, entry->d_name);

            if (entry->d_type == DT_DIR) {
                dirs[depth++] = opendir(path);
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
}


int main(int argc, char **argv) {

    if (argc < 2) return 0;
    if (chdir(argv[1]) == -1) return 1;

    nonRecursiveWalker();

    return 0;
}
