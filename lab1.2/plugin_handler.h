#pragma once

#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <dlfcn.h>
#include "plugin_api.h"


struct required_plugins {
    void **dls;
    int dls_len;
    struct option *opts;
    int opts_len;
};


struct required_plugins rpload(int argc, char **argv, const char *dirpath);

void rpclose(struct required_plugins rp);
