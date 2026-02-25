/**
 * toolkit.c — UI component library implementation
 * Copyright 2026 Lumi Team. Apache-2.0
 */

#include "lumi_toolkit.h"
#include <stdlib.h>
#include <string.h>

/* ── Scaffold ──────────────────────────────────────────────────── */

lumi_view_t *lumi_tk_appbar(const lumi_appbar_config_t *config) {
    lumi_view_t *bar = lumi_row();
    if (!bar) return NULL;

    lumi_view_set_height(bar, 56.0f);
    lumi_view_set_background(bar, config ? config->background : 0x2196F3FF);
    lumi_view_set_padding(bar, 0, 16, 0, 16);

    if (config && config->show_back) {
        lumi_view_t *back = lumi_button("<");
        lumi_view_set_foreground(back, 0xFFFFFFFF);
        if (config->on_back) {
            lumi_view_on_click(back, config->on_back, config->on_back_data);
        }
        lumi_view_add_child(bar, back);
    }

    if (config && config->title) {
        lumi_view_t *title = lumi_text(config->title);
        lumi_view_set_font_size(title, 20.0f);
        lumi_view_set_foreground(title, 0xFFFFFFFF);
        lumi_view_add_child(bar, title);
    }

    return bar;
}

lumi_view_t *lumi_tk_scaffold(const lumi_appbar_config_t *appbar, lumi_view_t *body) {
    lumi_view_t *root = lumi_column();
    if (!root) return NULL;

    if (appbar) {
        lumi_view_t *bar = lumi_tk_appbar(appbar);
        if (bar) lumi_view_add_child(root, bar);
    }

    if (body) {
        lumi_view_add_child(root, body);
    }

    return root;
}

/* ── List ──────────────────────────────────────────────────────── */

lumi_view_t *lumi_tk_list(void **items, int count,
                           lumi_list_builder_cb builder, void *userdata) {
    lumi_view_t *scroll = lumi_scroll();
    if (!scroll) return NULL;

    lumi_view_t *col = lumi_column();
    if (!col) { lumi_view_destroy(scroll); return NULL; }

    for (int i = 0; i < count; i++) {
        lumi_view_t *item = builder(i, items[i], userdata);
        if (item) lumi_view_add_child(col, item);
    }

    lumi_view_add_child(scroll, col);
    return scroll;
}

/* ── Icon button ───────────────────────────────────────────────── */

lumi_view_t *lumi_tk_icon_button(const char *icon_path, const char *label,
                                  lumi_click_cb on_click, void *userdata) {
    lumi_view_t *col = lumi_column();
    if (!col) return NULL;

    lumi_view_set_padding(col, 8, 8, 8, 8);

    if (icon_path) {
        lumi_view_t *icon = lumi_image(icon_path);
        lumi_view_set_width(icon, 24.0f);
        lumi_view_set_height(icon, 24.0f);
        lumi_view_add_child(col, icon);
    }

    if (label) {
        lumi_view_t *txt = lumi_text(label);
        lumi_view_set_font_size(txt, 12.0f);
        lumi_view_add_child(col, txt);
    }

    if (on_click) {
        lumi_view_on_click(col, on_click, userdata);
    }

    return col;
}

/* ── Switch toggle ─────────────────────────────────────────────── */

lumi_view_t *lumi_tk_switch(bool initial, lumi_click_cb on_toggle, void *userdata) {
    lumi_view_t *btn = lumi_button(initial ? "ON" : "OFF");
    lumi_view_set_background(btn, initial ? 0x4CAF50FF : 0x9E9E9EFF);
    lumi_view_set_foreground(btn, 0xFFFFFFFF);
    lumi_view_set_border_radius(btn, 16.0f);
    lumi_view_set_width(btn, 52.0f);
    lumi_view_set_height(btn, 28.0f);
    if (on_toggle) {
        lumi_view_on_click(btn, on_toggle, userdata);
    }
    return btn;
}

/* ── Progress bar ──────────────────────────────────────────────── */

lumi_view_t *lumi_tk_progress_bar(float progress) {
    lumi_view_t *track = lumi_row();
    lumi_view_set_height(track, 4.0f);
    lumi_view_set_background(track, 0xE0E0E0FF);
    lumi_view_set_border_radius(track, 2.0f);

    lumi_view_t *fill = lumi_spacer();
    float clamped = progress < 0.0f ? 0.0f : (progress > 1.0f ? 1.0f : progress);
    lumi_view_set_width(fill, clamped * 200.0f);
    lumi_view_set_height(fill, 4.0f);
    lumi_view_set_background(fill, 0x2196F3FF);
    lumi_view_set_border_radius(fill, 2.0f);

    lumi_view_add_child(track, fill);
    return track;
}

/* ── Dialog ────────────────────────────────────────────────────── */

lumi_view_t *lumi_tk_dialog(const char *title, const char *message,
                             const char *ok_label, const char *cancel_label,
                             lumi_click_cb on_ok, lumi_click_cb on_cancel,
                             void *userdata) {
    lumi_view_t *card = lumi_card();
    lumi_view_set_background(card, 0xFFFFFFFF);
    lumi_view_set_border_radius(card, 12.0f);
    lumi_view_set_padding(card, 24, 24, 24, 24);
    lumi_view_set_width(card, 300.0f);

    lumi_view_t *col = lumi_column();

    if (title) {
        lumi_view_t *t = lumi_text(title);
        lumi_view_set_font_size(t, 18.0f);
        lumi_view_set_foreground(t, 0x212121FF);
        lumi_view_add_child(col, t);
    }
    if (message) {
        lumi_view_t *m = lumi_text(message);
        lumi_view_set_font_size(m, 14.0f);
        lumi_view_set_foreground(m, 0x757575FF);
        lumi_view_add_child(col, m);
    }

    lumi_view_t *actions = lumi_row();
    if (cancel_label) {
        lumi_view_t *btn = lumi_button(cancel_label);
        if (on_cancel) lumi_view_on_click(btn, on_cancel, userdata);
        lumi_view_add_child(actions, btn);
    }
    if (ok_label) {
        lumi_view_t *btn = lumi_button(ok_label);
        lumi_view_set_background(btn, 0x2196F3FF);
        lumi_view_set_foreground(btn, 0xFFFFFFFF);
        if (on_ok) lumi_view_on_click(btn, on_ok, userdata);
        lumi_view_add_child(actions, btn);
    }
    lumi_view_add_child(col, actions);
    lumi_view_add_child(card, col);
    return card;
}

/* ── Toast ─────────────────────────────────────────────────────── */

lumi_view_t *lumi_tk_toast(const char *message) {
    lumi_view_t *v = lumi_text(message ? message : "");
    lumi_view_set_background(v, 0x323232E6);
    lumi_view_set_foreground(v, 0xFFFFFFFF);
    lumi_view_set_font_size(v, 14.0f);
    lumi_view_set_border_radius(v, 20.0f);
    lumi_view_set_padding(v, 12, 24, 12, 24);
    return v;
}

/* ── Avatar ────────────────────────────────────────────────────── */

lumi_view_t *lumi_tk_avatar(const char *image_path, float size) {
    lumi_view_t *img = lumi_image(image_path ? image_path : "");
    lumi_view_set_width(img, size);
    lumi_view_set_height(img, size);
    lumi_view_set_border_radius(img, size / 2.0f);
    return img;
}

/* ── Badge ─────────────────────────────────────────────────────── */

lumi_view_t *lumi_tk_badge(const char *text, uint32_t color) {
    lumi_view_t *v = lumi_text(text ? text : "");
    lumi_view_set_background(v, color);
    lumi_view_set_foreground(v, 0xFFFFFFFF);
    lumi_view_set_font_size(v, 10.0f);
    lumi_view_set_border_radius(v, 8.0f);
    lumi_view_set_padding(v, 2, 6, 2, 6);
    return v;
}

/* ── Bottom nav ────────────────────────────────────────────────── */

lumi_view_t *lumi_tk_bottom_nav(const lumi_nav_item_t *items, int count,
                                 lumi_click_cb on_select, void *userdata) {
    lumi_view_t *bar = lumi_row();
    lumi_view_set_height(bar, 56.0f);
    lumi_view_set_background(bar, 0xFFFFFFFF);

    for (int i = 0; i < count; i++) {
        lumi_view_t *item = lumi_tk_icon_button(
            items[i].icon, items[i].label, on_select, userdata);
        if (items[i].selected) {
            lumi_view_set_foreground(item, 0x2196F3FF);
        }
        lumi_view_add_child(bar, item);
    }

    return bar;
}

/* ── Tab bar ───────────────────────────────────────────────────── */

lumi_view_t *lumi_tk_tab_bar(const char **labels, int count, int active,
                              lumi_click_cb on_select, void *userdata) {
    lumi_view_t *bar = lumi_row();
    lumi_view_set_height(bar, 48.0f);
    lumi_view_set_background(bar, 0x2196F3FF);

    for (int i = 0; i < count; i++) {
        lumi_view_t *tab = lumi_button(labels[i]);
        lumi_view_set_foreground(tab, (i == active) ? 0xFFFFFFFF : 0xBBDEFBFF);
        if (on_select) lumi_view_on_click(tab, on_select, userdata);
        lumi_view_add_child(bar, tab);
    }

    return bar;
}
