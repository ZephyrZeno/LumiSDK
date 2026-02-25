/**
 * hello_c — Minimal LumiOS app in C
 * Copyright 2026 Lumi Team. Apache-2.0
 */

#include "lumiapp.h"
#include "lumi_toolkit.h"

static void on_create(lumi_app_t *app, void *userdata) {
    (void)userdata;
    lumi_log(LUMI_LOG_INFO, "hello", "App created!");

    lumi_appbar_config_t appbar = {
        .title      = "Hello LumiOS",
        .background = 0x6200EEFF,
        .show_back  = false,
    };

    lumi_view_t *body = lumi_column();
    lumi_view_set_padding(body, 24, 24, 24, 24);

    lumi_view_t *greeting = lumi_text("Welcome to LumiOS!");
    lumi_view_set_font_size(greeting, 24.0f);
    lumi_view_set_foreground(greeting, 0x212121FF);
    lumi_view_add_child(body, greeting);

    lumi_view_t *desc = lumi_text("This is a simple app built with LumiSDK.");
    lumi_view_set_font_size(desc, 14.0f);
    lumi_view_set_foreground(desc, 0x757575FF);
    lumi_view_add_child(body, desc);

    lumi_view_t *root = lumi_tk_scaffold(&appbar, body);
    lumi_app_set_content(app, root);
}

int main(void) {
    lumi_manifest_t manifest = {
        .app_id  = "com.lumios.hello",
        .name    = "Hello",
        .version = "1.0.0",
        .icon    = NULL,
    };
    lumi_lifecycle_t lc = {
        .on_create  = on_create,
        .on_destroy = NULL,
    };

    lumi_app_t *app = lumi_app_create(&manifest, &lc, NULL);
    int ret = lumi_app_run(app);
    lumi_app_destroy(app);
    return ret;
}
