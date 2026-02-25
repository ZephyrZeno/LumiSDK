/**
 * lumiapp.h — LumiOS Application SDK Core API
 * Copyright 2026 Lumi Team. Apache-2.0
 *
 * Unified C API for building LumiOS native applications.
 * Language bindings (C++, Rust, Java) wrap this interface.
 */

#ifndef LUMIAPP_H
#define LUMIAPP_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ── Version ─────────────────────────────────────────────────────── */

#define LUMIAPP_VERSION_MAJOR 0
#define LUMIAPP_VERSION_MINOR 1
#define LUMIAPP_VERSION_PATCH 0
#define LUMIAPP_VERSION_STRING "0.1.0"

/* ── Result codes ────────────────────────────────────────────────── */

typedef enum {
    LUMI_OK             =  0,
    LUMI_ERR_NOMEM      = -1,
    LUMI_ERR_INVALID    = -2,
    LUMI_ERR_NOT_FOUND  = -3,
    LUMI_ERR_IO         = -4,
    LUMI_ERR_PERMISSION = -5,
    LUMI_ERR_TIMEOUT    = -6,
    LUMI_ERR_UNKNOWN    = -99,
} lumi_result_t;

const char *lumi_result_str(lumi_result_t code);

/* ── Logging ─────────────────────────────────────────────────────── */

typedef enum {
    LUMI_LOG_VERBOSE = 0,
    LUMI_LOG_DEBUG   = 1,
    LUMI_LOG_INFO    = 2,
    LUMI_LOG_WARN    = 3,
    LUMI_LOG_ERROR   = 4,
} lumi_log_level_t;

void lumi_log(lumi_log_level_t level, const char *tag, const char *fmt, ...);
void lumi_log_set_level(lumi_log_level_t min_level);

/* ── Application lifecycle ───────────────────────────────────────── */

typedef struct lumi_app lumi_app_t;

typedef struct {
    const char *app_id;       /* e.g. "com.lumios.browser" */
    const char *name;         /* display name */
    const char *version;      /* semver string */
    const char *icon;         /* path to icon asset */
} lumi_manifest_t;

typedef struct {
    void (*on_create)(lumi_app_t *app, void *userdata);
    void (*on_start)(lumi_app_t *app, void *userdata);
    void (*on_resume)(lumi_app_t *app, void *userdata);
    void (*on_pause)(lumi_app_t *app, void *userdata);
    void (*on_stop)(lumi_app_t *app, void *userdata);
    void (*on_destroy)(lumi_app_t *app, void *userdata);
} lumi_lifecycle_t;

lumi_app_t *lumi_app_create(const lumi_manifest_t *manifest,
                             const lumi_lifecycle_t *lifecycle,
                             void *userdata);
void        lumi_app_destroy(lumi_app_t *app);
int         lumi_app_run(lumi_app_t *app);
void        lumi_app_quit(lumi_app_t *app);
void       *lumi_app_get_userdata(lumi_app_t *app);
const lumi_manifest_t *lumi_app_get_manifest(lumi_app_t *app);

/* ── View system ─────────────────────────────────────────────────── */

typedef struct lumi_view lumi_view_t;

typedef enum {
    LUMI_VIEW_COLUMN,
    LUMI_VIEW_ROW,
    LUMI_VIEW_STACK,
    LUMI_VIEW_TEXT,
    LUMI_VIEW_BUTTON,
    LUMI_VIEW_IMAGE,
    LUMI_VIEW_TEXT_FIELD,
    LUMI_VIEW_SCROLL,
    LUMI_VIEW_LIST,
    LUMI_VIEW_SPACER,
    LUMI_VIEW_DIVIDER,
    LUMI_VIEW_CARD,
    LUMI_VIEW_CUSTOM,
} lumi_view_type_t;

typedef void (*lumi_click_cb)(lumi_view_t *view, void *userdata);
typedef void (*lumi_text_cb)(lumi_view_t *view, const char *text, void *userdata);

/* Layout containers */
lumi_view_t *lumi_column(void);
lumi_view_t *lumi_row(void);
lumi_view_t *lumi_stack(void);
lumi_view_t *lumi_scroll(void);

/* Leaf views */
lumi_view_t *lumi_text(const char *content);
lumi_view_t *lumi_button(const char *label);
lumi_view_t *lumi_image(const char *source);
lumi_view_t *lumi_text_field(const char *placeholder);
lumi_view_t *lumi_spacer(void);
lumi_view_t *lumi_divider(void);
lumi_view_t *lumi_card(void);

/* View tree manipulation */
void lumi_view_add_child(lumi_view_t *parent, lumi_view_t *child);
void lumi_view_remove_child(lumi_view_t *parent, lumi_view_t *child);
void lumi_view_destroy(lumi_view_t *view);

/* View properties */
void lumi_view_set_id(lumi_view_t *view, const char *id);
const char *lumi_view_get_id(lumi_view_t *view);
lumi_view_type_t lumi_view_get_type(lumi_view_t *view);
void lumi_view_set_visible(lumi_view_t *view, bool visible);
bool lumi_view_get_visible(lumi_view_t *view);

/* View styling */
void lumi_view_set_width(lumi_view_t *view, float width);
void lumi_view_set_height(lumi_view_t *view, float height);
void lumi_view_set_padding(lumi_view_t *view, float top, float right, float bottom, float left);
void lumi_view_set_margin(lumi_view_t *view, float top, float right, float bottom, float left);
void lumi_view_set_background(lumi_view_t *view, uint32_t rgba);
void lumi_view_set_foreground(lumi_view_t *view, uint32_t rgba);
void lumi_view_set_font_size(lumi_view_t *view, float size);
void lumi_view_set_border_radius(lumi_view_t *view, float radius);

/* Event handlers */
void lumi_view_on_click(lumi_view_t *view, lumi_click_cb cb, void *userdata);
void lumi_view_on_long_click(lumi_view_t *view, lumi_click_cb cb, void *userdata);
void lumi_view_on_text_change(lumi_view_t *view, lumi_text_cb cb, void *userdata);

/* Text-specific */
void lumi_text_set_content(lumi_view_t *view, const char *text);
const char *lumi_text_get_content(lumi_view_t *view);

/* TextField-specific */
const char *lumi_text_field_get_value(lumi_view_t *view);
void lumi_text_field_set_value(lumi_view_t *view, const char *value);

/* App root view */
void lumi_app_set_content(lumi_app_t *app, lumi_view_t *root);

/* ── Storage (key-value) ─────────────────────────────────────────── */

lumi_result_t lumi_storage_set(const char *key, const char *value);
const char   *lumi_storage_get(const char *key);
lumi_result_t lumi_storage_remove(const char *key);
lumi_result_t lumi_storage_clear(void);

/* ── Notifications ───────────────────────────────────────────────── */

typedef struct {
    const char *title;
    const char *body;
    const char *icon;
    const char *channel;
    int priority;   /* 0=default, 1=high, -1=low */
} lumi_notification_t;

lumi_result_t lumi_notify(const lumi_notification_t *notif);
lumi_result_t lumi_notify_simple(const char *title, const char *body);

/* ── Intents / IPC ───────────────────────────────────────────────── */

typedef struct {
    const char *action;
    const char *data;
    const char *mime_type;
    const char *target_app;
} lumi_intent_t;

lumi_result_t lumi_intent_send(const lumi_intent_t *intent);

typedef void (*lumi_intent_cb)(const lumi_intent_t *intent, void *userdata);
lumi_result_t lumi_intent_register(const char *action, lumi_intent_cb cb, void *userdata);

/* ── File utilities ──────────────────────────────────────────────── */

lumi_result_t lumi_file_read(const char *path, char **out_data, size_t *out_len);
lumi_result_t lumi_file_write(const char *path, const char *data, size_t len);
bool          lumi_file_exists(const char *path);
lumi_result_t lumi_file_mkdir(const char *path);
lumi_result_t lumi_file_remove(const char *path);

/* ── Timer ───────────────────────────────────────────────────────── */

typedef void (*lumi_timer_cb)(void *userdata);

int  lumi_timer_set(uint32_t delay_ms, bool repeat, lumi_timer_cb cb, void *userdata);
void lumi_timer_cancel(int timer_id);

#ifdef __cplusplus
}
#endif

#endif /* LUMIAPP_H */
