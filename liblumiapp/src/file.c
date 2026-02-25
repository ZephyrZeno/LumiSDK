/**
 * file.c — File utility functions
 * Copyright 2026 Lumi Team. Apache-2.0
 */

#include "lumiapp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

lumi_result_t lumi_file_read(const char *path, char **out_data, size_t *out_len) {
    if (!path || !out_data || !out_len) return LUMI_ERR_INVALID;

    FILE *f = fopen(path, "rb");
    if (!f) return LUMI_ERR_NOT_FOUND;

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    if (size < 0) {
        fclose(f);
        return LUMI_ERR_IO;
    }

    char *buf = malloc((size_t)size + 1);
    if (!buf) {
        fclose(f);
        return LUMI_ERR_NOMEM;
    }

    size_t read = fread(buf, 1, (size_t)size, f);
    fclose(f);

    buf[read] = '\0';
    *out_data = buf;
    *out_len  = read;
    return LUMI_OK;
}

lumi_result_t lumi_file_write(const char *path, const char *data, size_t len) {
    if (!path || !data) return LUMI_ERR_INVALID;

    FILE *f = fopen(path, "wb");
    if (!f) return LUMI_ERR_IO;

    size_t written = fwrite(data, 1, len, f);
    fclose(f);

    return (written == len) ? LUMI_OK : LUMI_ERR_IO;
}

bool lumi_file_exists(const char *path) {
    if (!path) return false;
    struct stat st;
    return stat(path, &st) == 0;
}

lumi_result_t lumi_file_mkdir(const char *path) {
    if (!path) return LUMI_ERR_INVALID;
#ifdef _WIN32
    int ret = _mkdir(path);
#else
    int ret = mkdir(path, 0755);
#endif
    return (ret == 0) ? LUMI_OK : LUMI_ERR_IO;
}

lumi_result_t lumi_file_remove(const char *path) {
    if (!path) return LUMI_ERR_INVALID;
    return (remove(path) == 0) ? LUMI_OK : LUMI_ERR_IO;
}
