/**
 * intent.c — Inter-app communication (Intents / IPC)
 * Copyright 2026 Lumi Team. Apache-2.0
 */

#include "lumiapp.h"
#include <stdlib.h>
#include <string.h>

#define MAX_HANDLERS 128

typedef struct {
    char           *action;
    lumi_intent_cb  callback;
    void           *userdata;
} intent_handler_t;

static intent_handler_t g_handlers[MAX_HANDLERS];
static int g_handler_count = 0;

lumi_result_t lumi_intent_send(const lumi_intent_t *intent) {
    if (!intent || !intent->action) return LUMI_ERR_INVALID;

    lumi_log(LUMI_LOG_INFO, "intent", "Send: action=%s data=%s target=%s",
             intent->action,
             intent->data ? intent->data : "(null)",
             intent->target_app ? intent->target_app : "(broadcast)");

    /* Dispatch to local handlers that match the action */
    for (int i = 0; i < g_handler_count; i++) {
        if (strcmp(g_handlers[i].action, intent->action) == 0) {
            g_handlers[i].callback(intent, g_handlers[i].userdata);
        }
    }

    /* TODO: send to IPC bus for cross-app dispatch */
    return LUMI_OK;
}

lumi_result_t lumi_intent_register(const char *action, lumi_intent_cb cb, void *userdata) {
    if (!action || !cb) return LUMI_ERR_INVALID;
    if (g_handler_count >= MAX_HANDLERS) return LUMI_ERR_NOMEM;

    g_handlers[g_handler_count].action   = strdup(action);
    g_handlers[g_handler_count].callback = cb;
    g_handlers[g_handler_count].userdata = userdata;

    if (!g_handlers[g_handler_count].action) return LUMI_ERR_NOMEM;
    g_handler_count++;
    return LUMI_OK;
}
