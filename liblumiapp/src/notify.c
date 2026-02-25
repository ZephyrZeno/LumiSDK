/**
 * notify.c — Notification service
 * Copyright 2026 Lumi Team. Apache-2.0
 */

#include "lumiapp.h"
#include <string.h>

lumi_result_t lumi_notify(const lumi_notification_t *notif) {
    if (!notif || !notif->title) return LUMI_ERR_INVALID;

    lumi_log(LUMI_LOG_INFO, "notify", "[%s] %s: %s",
             notif->channel ? notif->channel : "default",
             notif->title,
             notif->body ? notif->body : "");

    /* TODO: send to LumiOS notification daemon via D-Bus or Unix socket */
    return LUMI_OK;
}

lumi_result_t lumi_notify_simple(const char *title, const char *body) {
    if (!title) return LUMI_ERR_INVALID;
    lumi_notification_t notif = {
        .title    = title,
        .body     = body,
        .icon     = NULL,
        .channel  = "default",
        .priority = 0,
    };
    return lumi_notify(&notif);
}
