#pragma once

#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <dirent.h>


void nrftw(const char*, void (*)(const char*));
