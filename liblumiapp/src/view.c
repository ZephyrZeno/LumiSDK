/**
 * view.c — UI view tree and properties
 * Copyright 2026 Lumi Team. Apache-2.0
 */

#include "lumiapp.h"
#include <stdlib.h>
#include <string.h>

#define MAX_CHILDREN 256

struct lumi_view {
    lumi_view_type_t type;
    char *id;
    char *text;         /* for text/button/text_field */
    bool visible;

    /* Style */
    float width, height;
    float pad_top, pad_right, pad_bottom, pad_left;
    float mar_top, mar_right, mar_bottom, mar_left;
    uint32_t background;
    uint32_t foreground;
    float font_size;
    float border_radius;

    /* Tree */
    lumi_view_t *children[MAX_CHILDREN];
    int child_count;
    lumi_view_t *parent;

    /* Callbacks */
    lumi_click_cb  on_click_cb;
    void          *on_click_data;
    lumi_click_cb  on_long_click_cb;
    void          *on_long_click_data;
    lumi_text_cb   on_text_change_cb;
    void          *on_text_change_data;
};

static lumi_view_t *view_alloc(lumi_view_type_t type) {
    lumi_view_t *v = calloc(1, sizeof(lumi_view_t));
    if (!v) return NULL;
    v->type = type;
    v->visible = true;
    v->font_size = 14.0f;
    v->foreground = 0x000000FF;
    v->background = 0x00000000;
    return v;
}

/* ── Constructors ──────────────────────────────────────────────── */

lumi_view_t *lumi_column(void)  { return view_alloc(LUMI_VIEW_COLUMN); }
lumi_view_t *lumi_row(void)     { return view_alloc(LUMI_VIEW_ROW); }
lumi_view_t *lumi_stack(void)   { return view_alloc(LUMI_VIEW_STACK); }
lumi_view_t *lumi_scroll(void)  { return view_alloc(LUMI_VIEW_SCROLL); }
lumi_view_t *lumi_spacer(void)  { return view_alloc(LUMI_VIEW_SPACER); }
lumi_view_t *lumi_divider(void) { return view_alloc(LUMI_VIEW_DIVIDER); }
lumi_view_t *lumi_card(void)    { return view_alloc(LUMI_VIEW_CARD); }

lumi_view_t *lumi_text(const char *content) {
    lumi_view_t *v = view_alloc(LUMI_VIEW_TEXT);
    if (v && content) v->text = strdup(content);
    return v;
}

lumi_view_t *lumi_button(const char *label) {
    lumi_view_t *v = view_alloc(LUMI_VIEW_BUTTON);
    if (v && label) v->text = strdup(label);
    return v;
}

lumi_view_t *lumi_image(const char *source) {
    lumi_view_t *v = view_alloc(LUMI_VIEW_IMAGE);
    if (v && source) v->text = strdup(source);
    return v;
}

lumi_view_t *lumi_text_field(const char *placeholder) {
    lumi_view_t *v = view_alloc(LUMI_VIEW_TEXT_FIELD);
    if (v && placeholder) v->text = strdup(placeholder);
    return v;
}

/* ── Tree manipulation ─────────────────────────────────────────── */

void lumi_view_add_child(lumi_view_t *parent, lumi_view_t *child) {
    if (!parent || !child) return;
    if (parent->child_count >= MAX_CHILDREN) return;
    parent->children[parent->child_count++] = child;
    child->parent = parent;
}

void lumi_view_remove_child(lumi_view_t *parent, lumi_view_t *child) {
    if (!parent || !child) return;
    for (int i = 0; i < parent->child_count; i++) {
        if (parent->children[i] == child) {
            child->parent = NULL;
            for (int j = i; j < parent->child_count - 1; j++) {
                parent->children[j] = parent->children[j + 1];
            }
            parent->child_count--;
            return;
        }
    }
}

void lumi_view_destroy(lumi_view_t *view) {
    if (!view) return;
    for (int i = 0; i < view->child_count; i++) {
        lumi_view_destroy(view->children[i]);
    }
    free(view->id);
    free(view->text);
    free(view);
}

/* ── Properties ────────────────────────────────────────────────── */

void lumi_view_set_id(lumi_view_t *view, const char *id) {
    if (!view) return;
    free(view->id);
    view->id = id ? strdup(id) : NULL;
}

const char *lumi_view_get_id(lumi_view_t *view) {
    return view ? view->id : NULL;
}

lumi_view_type_t lumi_view_get_type(lumi_view_t *view) {
    return view ? view->type : LUMI_VIEW_CUSTOM;
}

void lumi_view_set_visible(lumi_view_t *view, bool visible) {
    if (view) view->visible = visible;
}

bool lumi_view_get_visible(lumi_view_t *view) {
    return view ? view->visible : false;
}

/* ── Styling ───────────────────────────────────────────────────── */

void lumi_view_set_width(lumi_view_t *view, float w) { if (view) view->width = w; }
void lumi_view_set_height(lumi_view_t *view, float h) { if (view) view->height = h; }

void lumi_view_set_padding(lumi_view_t *view, float top, float right, float bottom, float left) {
    if (!view) return;
    view->pad_top = top; view->pad_right = right;
    view->pad_bottom = bottom; view->pad_left = left;
}

void lumi_view_set_margin(lumi_view_t *view, float top, float right, float bottom, float left) {
    if (!view) return;
    view->mar_top = top; view->mar_right = right;
    view->mar_bottom = bottom; view->mar_left = left;
}

void lumi_view_set_background(lumi_view_t *view, uint32_t rgba)  { if (view) view->background = rgba; }
void lumi_view_set_foreground(lumi_view_t *view, uint32_t rgba)  { if (view) view->foreground = rgba; }
void lumi_view_set_font_size(lumi_view_t *view, float size)      { if (view) view->font_size = size; }
void lumi_view_set_border_radius(lumi_view_t *view, float r)     { if (view) view->border_radius = r; }

/* ── Event handlers ────────────────────────────────────────────── */

void lumi_view_on_click(lumi_view_t *view, lumi_click_cb cb, void *ud) {
    if (!view) return;
    view->on_click_cb = cb;
    view->on_click_data = ud;
}

void lumi_view_on_long_click(lumi_view_t *view, lumi_click_cb cb, void *ud) {
    if (!view) return;
    view->on_long_click_cb = cb;
    view->on_long_click_data = ud;
}

void lumi_view_on_text_change(lumi_view_t *view, lumi_text_cb cb, void *ud) {
    if (!view) return;
    view->on_text_change_cb = cb;
    view->on_text_change_data = ud;
}

/* ── Text / TextField specifics ────────────────────────────────── */

void lumi_text_set_content(lumi_view_t *view, const char *text) {
    if (!view) return;
    free(view->text);
    view->text = text ? strdup(text) : NULL;
}

const char *lumi_text_get_content(lumi_view_t *view) {
    return view ? view->text : NULL;
}

const char *lumi_text_field_get_value(lumi_view_t *view) {
    return view ? view->text : NULL;
}

void lumi_text_field_set_value(lumi_view_t *view, const char *value) {
    if (!view) return;
    free(view->text);
    view->text = value ? strdup(value) : NULL;
    if (view->on_text_change_cb) {
        view->on_text_change_cb(view, view->text, view->on_text_change_data);
    }
}
