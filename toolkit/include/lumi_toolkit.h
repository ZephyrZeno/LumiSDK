/**
 * lumi_toolkit.h — High-level UI component library built on liblumiapp
 * Copyright 2026 Lumi Team. Apache-2.0
 */

#ifndef LUMI_TOOLKIT_H
#define LUMI_TOOLKIT_H

#include "lumiapp.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ── Scaffold (standard app layout) ──────────────────────────────── */

typedef struct {
    const char *title;
    uint32_t    background;    /* RGBA */
    bool        show_back;
    lumi_click_cb on_back;
    void         *on_back_data;
} lumi_appbar_config_t;

lumi_view_t *lumi_tk_scaffold(const lumi_appbar_config_t *appbar, lumi_view_t *body);
lumi_view_t *lumi_tk_appbar(const lumi_appbar_config_t *config);

/* ── List / Adapter ──────────────────────────────────────────────── */

typedef lumi_view_t *(*lumi_list_builder_cb)(int index, void *item, void *userdata);

lumi_view_t *lumi_tk_list(void **items, int count,
                           lumi_list_builder_cb builder, void *userdata);

/* ── Common widgets ──────────────────────────────────────────────── */

lumi_view_t *lumi_tk_icon_button(const char *icon_path, const char *label,
                                  lumi_click_cb on_click, void *userdata);

lumi_view_t *lumi_tk_switch(bool initial, lumi_click_cb on_toggle, void *userdata);

lumi_view_t *lumi_tk_progress_bar(float progress); /* 0.0 – 1.0 */

lumi_view_t *lumi_tk_dialog(const char *title, const char *message,
                             const char *ok_label, const char *cancel_label,
                             lumi_click_cb on_ok, lumi_click_cb on_cancel,
                             void *userdata);

lumi_view_t *lumi_tk_toast(const char *message);

lumi_view_t *lumi_tk_avatar(const char *image_path, float size);

lumi_view_t *lumi_tk_badge(const char *text, uint32_t color);

/* ── Navigation ──────────────────────────────────────────────────── */

typedef struct {
    const char *label;
    const char *icon;
    bool selected;
} lumi_nav_item_t;

lumi_view_t *lumi_tk_bottom_nav(const lumi_nav_item_t *items, int count,
                                 lumi_click_cb on_select, void *userdata);

lumi_view_t *lumi_tk_tab_bar(const char **labels, int count, int active,
                              lumi_click_cb on_select, void *userdata);

#ifdef __cplusplus
}
#endif

#endif /* LUMI_TOOLKIT_H */
