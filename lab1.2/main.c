#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <getopt.h>
#include <stdlib.h>
#include <dlfcn.h>
#include "nrftw.h"
#include "plugin_api.h"



struct required_plugins {
    void **dls;
    int dls_len;
    int opts_len;
};


struct required_plugins rpload(const char *dirpath) {

    struct dirent *entry;
    DIR *dir = opendir(dirpath);
    if (!dir) {
        fprintf(stderr, "Error opening selected directory %s\n", dirpath);
        return (struct required_plugins) {};
    }

    int dls_pos = 0, dls_cap = 8, opts_len = 0;
    void **dls = malloc(sizeof(void*) * dls_cap);   //!

    while (1) {
        // Считываем следующий файл
        entry = readdir(dir);

        // Если файла нет -> выходим
        if (!entry) break;

        // Если не обычный -> пропускаем
        if (entry->d_type != DT_REG) continue;

        // Если оканчивается не на .so -> пропускаем
        int pos = 0;
        while ((entry->d_name)[pos]) ++pos;
        if (pos < 3 || strcmp(entry->d_name+pos-3, ".so")) continue;

        // Если место под динамические библиотеки заполнено -> увеличиваем его
        if (dls_pos == dls_cap) {
            dls_cap *= 2;
            dls = realloc(dls, sizeof(void*) * dls_cap);    //!
        }

        // Открываем библиотеку
        char *path = realpath(entry->d_name, NULL);         //!
        dls[dls_pos++] = dlopen(path, RTLD_LAZY | RTLD_GLOBAL);
        free(path);

        // Если ничего не открылось -> пропускаем
        if (!dls[dls_pos-1]) {
            printf("ERROR: dlopen: %s\n", dlerror());       //?
            --dls_pos;
            continue;
        }

        // Если нет функции plugin_get_info_ptr -> закрываем, пропускаем
        int (*plugin_get_info_ptr)(struct plugin_info*) = dlsym(dls[dls_pos-1], "plugin_get_info");
        if (!plugin_get_info_ptr) {
            printf("ERROR: dlopen: %s\n", dlerror());       //?
            dlclose(dls[--dls_pos]);
            continue;
        }

        // Подгружаем данные из плагина
        struct plugin_info ppi;
        (*plugin_get_info_ptr)(&ppi);
        opts_len += ppi.sup_opts_len;
    }
    closedir(dir);

    return (struct required_plugins) {dls, dls_pos, opts_len};
}


void rpclose(struct required_plugins rp) {
    for (int i = 0; i < rp.dls_len; ++i) dlclose(rp.dls[i]);
    free(rp.dls);
}



int main(int argc, char **argv) {
    (void) argc, (void) argv;

    struct required_plugins rp = rpload(".");
    printf("%d\n", rp.opts_len);
    rpclose(rp);

    return 0;
}
