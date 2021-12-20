#include "log.h"

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static const char *info =   "\033[1;32m[INFO]\033[0m ";
static const char *warn =   "\033[1;33m[WARN]\033[0m ";
static const char *error =  "\033[1;31m[ERROR]\033[0m ";

void bl_log_c(bl_log_level level, const char *format, ...)
{
    va_list argp;
    va_start(argp, format);

    if (level == BL_LOG_LEVEL_INFO) {
        char *new_format = malloc(
            (sizeof(char) * strlen(info)) + (sizeof(char) * strlen(format)) + 2
        );
        strcpy(new_format, info);
        strcpy(new_format + (strlen(info)), format);
        new_format[strlen(info) + strlen(format)] = '\n';
        new_format[strlen(info) + strlen(format) + 1] = '\0';
        vfprintf(stdout, new_format, argp);
        fflush(stdout);
        free(new_format);
    } else if (level == BL_LOG_LEVEL_WARN) {
        char *new_format = malloc(
            (sizeof(char) * strlen(warn)) + (sizeof(char) * strlen(format)) + 2
        );
        strcpy(new_format, warn);
        strcpy(new_format + (strlen(warn)), format);
        new_format[strlen(warn) + strlen(format)] = '\n';
        new_format[strlen(warn) + strlen(format) + 1] = '\0';
        vfprintf(stderr, new_format, argp);
        free(new_format);
    } else if (level == BL_LOG_LEVEL_ERROR) {
        char *new_format = malloc(
            (sizeof(char) * strlen(error)) + (sizeof(char) * strlen(format)) + 2
        );
        strcpy(new_format, error);
        strcpy(new_format + (strlen(error)), format);
        new_format[strlen(error) + strlen(format)] = '\n';
        new_format[strlen(error) + strlen(format) + 1] = '\0';
        vfprintf(stderr, new_format, argp);
        free(new_format);
    }

    va_end(argp);
}
