/**
 * storage.c — Key-value persistent storage
 * Copyright 2026 Lumi Team. Apache-2.0
 */

#include "lumiapp.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MAX_ENTRIES 1024

typedef struct {
    char *key;
    char *value;
} kv_entry_t;

static kv_entry_t g_store[MAX_ENTRIES];
static int g_store_count = 0;

static int find_key(const char *key) {
    for (int i = 0; i < g_store_count; i++) {
        if (g_store[i].key && strcmp(g_store[i].key, key) == 0) {
            return i;
        }
    }
    return -1;
}

lumi_result_t lumi_storage_set(const char *key, const char *value) {
    if (!key || !value) return LUMI_ERR_INVALID;

    int idx = find_key(key);
    if (idx >= 0) {
        free(g_store[idx].value);
        g_store[idx].value = strdup(value);
        return g_store[idx].value ? LUMI_OK : LUMI_ERR_NOMEM;
    }

    if (g_store_count >= MAX_ENTRIES) return LUMI_ERR_NOMEM;

    g_store[g_store_count].key   = strdup(key);
    g_store[g_store_count].value = strdup(value);
    if (!g_store[g_store_count].key || !g_store[g_store_count].value) {
        free(g_store[g_store_count].key);
        free(g_store[g_store_count].value);
        return LUMI_ERR_NOMEM;
    }
    g_store_count++;
    return LUMI_OK;
}

const char *lumi_storage_get(const char *key) {
    if (!key) return NULL;
    int idx = find_key(key);
    return (idx >= 0) ? g_store[idx].value : NULL;
}

lumi_result_t lumi_storage_remove(const char *key) {
    if (!key) return LUMI_ERR_INVALID;
    int idx = find_key(key);
    if (idx < 0) return LUMI_ERR_NOT_FOUND;

    free(g_store[idx].key);
    free(g_store[idx].value);

    for (int i = idx; i < g_store_count - 1; i++) {
        g_store[i] = g_store[i + 1];
    }
    g_store_count--;
    return LUMI_OK;
}

lumi_result_t lumi_storage_clear(void) {
    for (int i = 0; i < g_store_count; i++) {
        free(g_store[i].key);
        free(g_store[i].value);
    }
    g_store_count = 0;
    return LUMI_OK;
}
