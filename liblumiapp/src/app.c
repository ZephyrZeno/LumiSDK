/**
 * app.c — Application lifecycle management
 * Copyright 2026 Lumi Team. Apache-2.0
 */

#include "lumiapp.h"
#include <stdlib.h>
#include <string.h>
#include <signal.h>

struct lumi_app {
    lumi_manifest_t  manifest;
    lumi_lifecycle_t lifecycle;
    void            *userdata;
    lumi_view_t     *root_view;
    bool             running;
    bool             paused;
};

static lumi_app_t *g_current_app = NULL;

static void sigint_handler(int sig) {
    (void)sig;
    if (g_current_app) {
        g_current_app->running = false;
    }
}

lumi_app_t *lumi_app_create(const lumi_manifest_t *manifest,
                             const lumi_lifecycle_t *lifecycle,
                             void *userdata) {
    if (!manifest || !lifecycle) return NULL;

    lumi_app_t *app = calloc(1, sizeof(lumi_app_t));
    if (!app) return NULL;

    app->manifest.app_id  = manifest->app_id  ? strdup(manifest->app_id)  : NULL;
    app->manifest.name    = manifest->name    ? strdup(manifest->name)    : NULL;
    app->manifest.version = manifest->version ? strdup(manifest->version) : NULL;
    app->manifest.icon    = manifest->icon    ? strdup(manifest->icon)    : NULL;
    app->lifecycle = *lifecycle;
    app->userdata  = userdata;
    app->running   = false;
    app->paused    = false;
    app->root_view = NULL;

    lumi_log(LUMI_LOG_INFO, "app", "Created app: %s (%s) v%s",
             app->manifest.name ? app->manifest.name : "?",
             app->manifest.app_id ? app->manifest.app_id : "?",
             app->manifest.version ? app->manifest.version : "?");

    return app;
}

void lumi_app_destroy(lumi_app_t *app) {
    if (!app) return;

    if (app->lifecycle.on_destroy) {
        app->lifecycle.on_destroy(app, app->userdata);
    }

    if (app->root_view) {
        lumi_view_destroy(app->root_view);
    }

    free((void *)app->manifest.app_id);
    free((void *)app->manifest.name);
    free((void *)app->manifest.version);
    free((void *)app->manifest.icon);
    free(app);

    if (g_current_app == app) {
        g_current_app = NULL;
    }
}

int lumi_app_run(lumi_app_t *app) {
    if (!app) return -1;

    g_current_app = app;
    signal(SIGINT, sigint_handler);

    app->running = true;

    if (app->lifecycle.on_create) {
        app->lifecycle.on_create(app, app->userdata);
    }
    if (app->lifecycle.on_start) {
        app->lifecycle.on_start(app, app->userdata);
    }
    if (app->lifecycle.on_resume) {
        app->lifecycle.on_resume(app, app->userdata);
    }

    lumi_log(LUMI_LOG_INFO, "app", "Entering main loop");

    /* Main event loop (simplified — real impl ties into Wayland) */
    while (app->running) {
        /* TODO: integrate with Wayland/display event loop */
        /* For now, just a stub that breaks immediately in headless mode */
        break;
    }

    if (app->lifecycle.on_pause) {
        app->lifecycle.on_pause(app, app->userdata);
    }
    if (app->lifecycle.on_stop) {
        app->lifecycle.on_stop(app, app->userdata);
    }

    lumi_log(LUMI_LOG_INFO, "app", "App stopped");
    return 0;
}

void lumi_app_quit(lumi_app_t *app) {
    if (app) {
        app->running = false;
    }
}

void *lumi_app_get_userdata(lumi_app_t *app) {
    return app ? app->userdata : NULL;
}

const lumi_manifest_t *lumi_app_get_manifest(lumi_app_t *app) {
    return app ? &app->manifest : NULL;
}

void lumi_app_set_content(lumi_app_t *app, lumi_view_t *root) {
    if (!app) return;
    if (app->root_view) {
        lumi_view_destroy(app->root_view);
    }
    app->root_view = root;
}
