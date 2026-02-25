/**
 * test_sdk.c — Unit tests for liblumiapp
 * Copyright 2026 Lumi Team. Apache-2.0
 */

#include "lumiapp.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

static int tests_run = 0;
static int tests_passed = 0;

#define TEST(name) do { \
    printf("  TEST %-40s ", #name); \
    tests_run++; \
    test_##name(); \
    tests_passed++; \
    printf("PASS\n"); \
} while(0)

/* ── Result codes ──────────────────────────────────────────────── */

static void test_result_str(void) {
    assert(strcmp(lumi_result_str(LUMI_OK), "OK") == 0);
    assert(strcmp(lumi_result_str(LUMI_ERR_NOMEM), "Out of memory") == 0);
    assert(strcmp(lumi_result_str(LUMI_ERR_INVALID), "Invalid argument") == 0);
    assert(strcmp(lumi_result_str(LUMI_ERR_NOT_FOUND), "Not found") == 0);
    assert(strcmp(lumi_result_str(LUMI_ERR_IO), "I/O error") == 0);
}

/* ── Logging ───────────────────────────────────────────────────── */

static void test_log(void) {
    lumi_log_set_level(LUMI_LOG_VERBOSE);
    lumi_log(LUMI_LOG_INFO, "test", "Log test message: %d", 42);
    lumi_log(LUMI_LOG_ERROR, "test", "Error test");
    lumi_log_set_level(LUMI_LOG_INFO);
    /* If we get here without crash, logging works */
}

/* ── App lifecycle ─────────────────────────────────────────────── */

static int create_called = 0;
static int destroy_called = 0;

static void on_create(lumi_app_t *app, void *ud) {
    (void)app; (void)ud;
    create_called = 1;
}

static void on_destroy_cb(lumi_app_t *app, void *ud) {
    (void)app; (void)ud;
    destroy_called = 1;
}

static void test_app_lifecycle(void) {
    lumi_manifest_t manifest = {
        .app_id  = "com.test.app",
        .name    = "TestApp",
        .version = "1.0.0",
        .icon    = NULL,
    };
    lumi_lifecycle_t lc = {
        .on_create  = on_create,
        .on_start   = NULL,
        .on_resume  = NULL,
        .on_pause   = NULL,
        .on_stop    = NULL,
        .on_destroy = on_destroy_cb,
    };

    int userdata = 42;
    lumi_app_t *app = lumi_app_create(&manifest, &lc, &userdata);
    assert(app != NULL);

    const lumi_manifest_t *m = lumi_app_get_manifest(app);
    assert(m != NULL);
    assert(strcmp(m->app_id, "com.test.app") == 0);
    assert(strcmp(m->name, "TestApp") == 0);

    void *ud = lumi_app_get_userdata(app);
    assert(ud == &userdata);

    /* run will call on_create then exit (headless) */
    create_called = 0;
    lumi_app_run(app);
    assert(create_called == 1);

    destroy_called = 0;
    lumi_app_destroy(app);
    assert(destroy_called == 1);
}

static void test_app_null(void) {
    assert(lumi_app_create(NULL, NULL, NULL) == NULL);
    lumi_app_destroy(NULL); /* should not crash */
}

/* ── View system ───────────────────────────────────────────────── */

static void test_view_create(void) {
    lumi_view_t *col = lumi_column();
    assert(col != NULL);
    assert(lumi_view_get_type(col) == LUMI_VIEW_COLUMN);
    assert(lumi_view_get_visible(col) == true);

    lumi_view_t *txt = lumi_text("Hello");
    assert(txt != NULL);
    assert(strcmp(lumi_text_get_content(txt), "Hello") == 0);

    lumi_view_t *btn = lumi_button("Click");
    assert(btn != NULL);
    assert(lumi_view_get_type(btn) == LUMI_VIEW_BUTTON);

    lumi_view_destroy(col);
    lumi_view_destroy(txt);
    lumi_view_destroy(btn);
}

static void test_view_tree(void) {
    lumi_view_t *col = lumi_column();
    lumi_view_t *t1 = lumi_text("A");
    lumi_view_t *t2 = lumi_text("B");

    lumi_view_add_child(col, t1);
    lumi_view_add_child(col, t2);

    /* remove child */
    lumi_view_remove_child(col, t1);

    /* destroy tree (should recursively free t2) */
    lumi_view_destroy(col);
    /* t1 was removed, free separately */
    lumi_view_destroy(t1);
}

static void test_view_properties(void) {
    lumi_view_t *v = lumi_text("test");

    lumi_view_set_id(v, "my-text");
    assert(strcmp(lumi_view_get_id(v), "my-text") == 0);

    lumi_view_set_visible(v, false);
    assert(lumi_view_get_visible(v) == false);

    lumi_view_set_width(v, 100.0f);
    lumi_view_set_height(v, 50.0f);
    lumi_view_set_background(v, 0xFF0000FF);
    lumi_view_set_font_size(v, 18.0f);
    lumi_view_set_border_radius(v, 8.0f);

    lumi_text_set_content(v, "updated");
    assert(strcmp(lumi_text_get_content(v), "updated") == 0);

    lumi_view_destroy(v);
}

static int click_count = 0;
static void on_click(lumi_view_t *v, void *ud) {
    (void)v; (void)ud;
    click_count++;
}

static void test_view_callbacks(void) {
    lumi_view_t *btn = lumi_button("Test");
    lumi_view_on_click(btn, on_click, NULL);
    /* callbacks are stored but not triggered in test — no crash means OK */
    lumi_view_destroy(btn);
}

/* ── Storage ───────────────────────────────────────────────────── */

static void test_storage(void) {
    lumi_storage_clear();

    assert(lumi_storage_set("key1", "value1") == LUMI_OK);
    assert(lumi_storage_set("key2", "value2") == LUMI_OK);

    const char *v = lumi_storage_get("key1");
    assert(v != NULL);
    assert(strcmp(v, "value1") == 0);

    assert(lumi_storage_get("nonexist") == NULL);

    /* overwrite */
    assert(lumi_storage_set("key1", "updated") == LUMI_OK);
    assert(strcmp(lumi_storage_get("key1"), "updated") == 0);

    /* remove */
    assert(lumi_storage_remove("key1") == LUMI_OK);
    assert(lumi_storage_get("key1") == NULL);
    assert(lumi_storage_remove("key1") == LUMI_ERR_NOT_FOUND);

    /* clear */
    assert(lumi_storage_clear() == LUMI_OK);
    assert(lumi_storage_get("key2") == NULL);
}

static void test_storage_invalid(void) {
    assert(lumi_storage_set(NULL, "v") == LUMI_ERR_INVALID);
    assert(lumi_storage_set("k", NULL) == LUMI_ERR_INVALID);
    assert(lumi_storage_get(NULL) == NULL);
    assert(lumi_storage_remove(NULL) == LUMI_ERR_INVALID);
}

/* ── Notifications ─────────────────────────────────────────────── */

static void test_notify(void) {
    assert(lumi_notify_simple("Test Title", "Test Body") == LUMI_OK);
    assert(lumi_notify_simple(NULL, "body") == LUMI_ERR_INVALID);

    lumi_notification_t n = {
        .title = "Alert",
        .body = "Something happened",
        .icon = "alert.png",
        .channel = "alerts",
        .priority = 1,
    };
    assert(lumi_notify(&n) == LUMI_OK);
    assert(lumi_notify(NULL) == LUMI_ERR_INVALID);
}

/* ── Intent / IPC ──────────────────────────────────────────────── */

static int intent_received = 0;
static void intent_cb(const lumi_intent_t *intent, void *ud) {
    (void)ud;
    if (intent && intent->action) intent_received = 1;
}

static void test_intent(void) {
    intent_received = 0;
    assert(lumi_intent_register("com.test.ACTION", intent_cb, NULL) == LUMI_OK);

    lumi_intent_t intent = {
        .action = "com.test.ACTION",
        .data = "hello",
        .mime_type = NULL,
        .target_app = NULL,
    };
    assert(lumi_intent_send(&intent) == LUMI_OK);
    assert(intent_received == 1);

    assert(lumi_intent_send(NULL) == LUMI_ERR_INVALID);
    assert(lumi_intent_register(NULL, intent_cb, NULL) == LUMI_ERR_INVALID);
}

/* ── File utilities ────────────────────────────────────────────── */

static void test_file(void) {
#ifdef _WIN32
    const char *path = "C:\\Windows\\Temp\\lumi_test_file.txt";
#else
    const char *path = "/tmp/lumi_test_file.txt";
#endif
    const char *data = "Hello LumiOS!";

    assert(lumi_file_write(path, data, strlen(data)) == LUMI_OK);
    assert(lumi_file_exists(path) == true);

    char *read_data = NULL;
    size_t read_len = 0;
    assert(lumi_file_read(path, &read_data, &read_len) == LUMI_OK);
    assert(read_len == strlen(data));
    assert(strcmp(read_data, data) == 0);
    free(read_data);

    assert(lumi_file_remove(path) == LUMI_OK);
    assert(lumi_file_exists(path) == false);

    assert(lumi_file_read(NULL, NULL, NULL) == LUMI_ERR_INVALID);
    assert(lumi_file_write(NULL, NULL, 0) == LUMI_ERR_INVALID);
}

/* ── Timer ─────────────────────────────────────────────────────── */

static void timer_cb_fn(void *ud) { (void)ud; }

static void test_timer(void) {
    int id = lumi_timer_set(1000, false, timer_cb_fn, NULL);
    assert(id > 0);

    int id2 = lumi_timer_set(500, true, timer_cb_fn, NULL);
    assert(id2 > id);

    lumi_timer_cancel(id);
    lumi_timer_cancel(id2);
    lumi_timer_cancel(9999); /* non-existent, should not crash */

    assert(lumi_timer_set(0, false, NULL, NULL) == -1);
}

/* ── Main ──────────────────────────────────────────────────────── */

int main(void) {
    printf("=== LumiSDK Test Suite ===\n\n");

    printf("Result codes:\n");
    TEST(result_str);

    printf("\nLogging:\n");
    TEST(log);

    printf("\nApp lifecycle:\n");
    TEST(app_lifecycle);
    TEST(app_null);

    printf("\nView system:\n");
    TEST(view_create);
    TEST(view_tree);
    TEST(view_properties);
    TEST(view_callbacks);

    printf("\nStorage:\n");
    TEST(storage);
    TEST(storage_invalid);

    printf("\nNotifications:\n");
    TEST(notify);

    printf("\nIntent/IPC:\n");
    TEST(intent);

    printf("\nFile utilities:\n");
    TEST(file);

    printf("\nTimer:\n");
    TEST(timer);

    printf("\n=== Results: %d/%d passed ===\n", tests_passed, tests_run);
    return (tests_passed == tests_run) ? 0 : 1;
}
