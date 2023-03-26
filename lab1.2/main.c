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
    struct option *opts;
    int opts_len;
};


struct required_plugins rpload(int argc, char **argv, const char *dirpath) {

    struct dirent *entry;
    DIR *dir = opendir(dirpath);
    if (!dir) {
        fprintf(stderr, "Error opening selected directory %s\n", dirpath);
        return (struct required_plugins) {};
    }

    int dls_pos = 0, dls_cap = 8;
    void **dls = malloc(sizeof(void*) * dls_cap);   //!

    int opts_pos = 0, opts_cap = 8;
    struct option *opts = malloc(sizeof(struct option) * opts_cap);   //!


    while (1) {
        // Считываем следующий файл
        entry = readdir(dir);

        // Если файла нет -> выходим
        if (!entry) break;

        // Если не обычный -> пропускаем
        if (entry->d_type != DT_REG) continue;

        // Если оканчивается не на .so -> пропускаем
        int str_pos = strlen(entry->d_name);
        if (str_pos < 3 || strcmp(entry->d_name+str_pos-3, ".so")) continue;

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

        // Если парамер совпал с нужным -> не закрываем
        for (int j = 1; j < argc; ++j) {
            if (argv[j][0] != '-' || argv[j][1] != '-') continue;
            for (size_t i = 0; i < ppi.sup_opts_len; ++i) {
                if (!strcmp(ppi.sup_opts[i].opt.name, argv[j]+2)) {
                    goto dontclose;
                }
            }
        }
        dlclose(dls[--dls_pos]);
        continue;
        dontclose:

        // Добавляем опции плагина в общий список
        for (size_t i = 0; i < ppi.sup_opts_len; ++i) {
            if (opts_pos == opts_cap) {
                opts_cap *= 2;
                opts = realloc(opts, sizeof(struct option) * opts_cap);     //!
            }
            opts[opts_pos++] = ppi.sup_opts[i].opt;
        }
    }
    closedir(dir);

    return (struct required_plugins) {dls, dls_pos, opts, opts_pos};
}


void rpclose(struct required_plugins rp) {
    for (int i = 0; i < rp.dls_len; ++i) dlclose(rp.dls[i]);
    free(rp.dls);
    free(rp.opts);
}



int parse_options(int argc, char **argv, const char *opts, const struct option *longopts) {
    int longindex;
    while (1) {
        switch (getopt_long(argc, argv, opts, longopts, &longindex)) {
            case 0:
                * (char**) longopts[longindex].flag = optarg;
                break;
            case 'P':
                printf("path = %s\n", optarg);  //?
                break;
            case 'A':
                printf("and\n");    //?
                break;
            case 'O':
                printf("or\n");     //?
                break;
            case 'N':
                printf("not\n");    //?
                break;
            case 'v':
                fprintf(stdout, "version\n");
                return 1;
            case 'h':
                fprintf(stdout, "help\n");
                return 1;
            case -1:
                return 0;
            default:
                return 1;

        }
    }
}


int main(int argc, char **argv) {

    struct required_plugins rp = rpload(argc, argv, ".");

    if (!parse_options(argc, argv, "P:AONvh", rp.opts)) {
        for (int i = 0; i < rp.dls_len; ++i) {
            int (*fun)(const char*, struct option[], size_t) = dlsym(rp.dls[i], "plugin_process_file");
            (*fun)("", rp.opts, 1);
        }
    }

    rpclose(rp);
    return 0;
}
