#include "nrftw.h"


int push(char *path, size_t *pos, char *name) {
    if (*pos && *pos < PATH_MAX) path[(*pos)++] = '/';
    for (size_t i = 0; *pos < PATH_MAX; ++(*pos), ++i)
        if (!(path[*pos] = name[i])) return 0;
    return 1;
}


void pop(char *path, size_t *pos) {
    while (*pos && path[--(*pos)] != '/');
    path[*pos] = '\0';
}


void nrftw(const char *dir, void (*fn)(const char*)) {

    DIR *dirs[PATH_MAX/2];
    char path[PATH_MAX];
    size_t pos = 0, depth = 0;
    struct dirent *entry;

    pos = strlen(dir);
    if (pos >= PATH_MAX) {
        fprintf(stderr, "Error opening path %s: File name too long\n", dir);
        return;
    }
    strcpy(path, dir);

    dirs[depth++] = opendir(path);
    if (!dirs[depth-1]) {
        fprintf(stderr, "Error opening directory %s\n", path);
        return;
    }

    while (depth) {
        entry = readdir(dirs[depth-1]);

        if (entry) {
            if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
                continue;

            if (push(path, &pos, entry->d_name)) {
                pop(path, &pos);
                fprintf(stderr, "Error opening path %s/%s: File name too long\n", path, entry->d_name);
                continue;
            }

            if (entry->d_type == DT_DIR) {
                dirs[depth++] = opendir(path);
                if (!dirs[depth-1]) {
                    fprintf(stderr, "Error opening directory %s\n", path);
                    --depth;
                }
                else continue;
            }
            if (entry->d_type == DT_REG)
                fn(path);
        }
        else {
            closedir(dirs[--depth]);
        }
        pop(path, &pos);
    }
}
