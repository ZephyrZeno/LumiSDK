/**
 * lumiapp_jni.c — JNI bridge for LumiApp Java bindings
 * Copyright 2026 Lumi Team. Apache-2.0
 */

#include <jni.h>
#include "lumiapp.h"
#include <stdlib.h>
#include <string.h>

/* ── Helpers ───────────────────────────────────────────────────── */

static const char *jstring_to_cstr(JNIEnv *env, jstring str) {
    if (!str) return NULL;
    return (*env)->GetStringUTFChars(env, str, NULL);
}

static void release_cstr(JNIEnv *env, jstring str, const char *cstr) {
    if (str && cstr) (*env)->ReleaseStringUTFChars(env, str, cstr);
}

/* ── LumiApp native methods ────────────────────────────────────── */

JNIEXPORT jlong JNICALL
Java_com_lumios_sdk_LumiApp_nativeCreate(JNIEnv *env, jclass cls,
    jstring appId, jstring name, jstring version, jobject lifecycle) {
    (void)cls;
    const char *c_id   = jstring_to_cstr(env, appId);
    const char *c_name = jstring_to_cstr(env, name);
    const char *c_ver  = jstring_to_cstr(env, version);

    lumi_manifest_t manifest = { c_id, c_name, c_ver, NULL };
    lumi_lifecycle_t lc = { NULL, NULL, NULL, NULL, NULL, NULL };

    lumi_app_t *app = lumi_app_create(&manifest, &lc, NULL);

    release_cstr(env, appId, c_id);
    release_cstr(env, name, c_name);
    release_cstr(env, version, c_ver);

    return (jlong)(intptr_t)app;
}

JNIEXPORT jint JNICALL
Java_com_lumios_sdk_LumiApp_nativeRun(JNIEnv *env, jclass cls, jlong handle) {
    (void)env; (void)cls;
    return lumi_app_run((lumi_app_t *)(intptr_t)handle);
}

JNIEXPORT void JNICALL
Java_com_lumios_sdk_LumiApp_nativeQuit(JNIEnv *env, jclass cls, jlong handle) {
    (void)env; (void)cls;
    lumi_app_quit((lumi_app_t *)(intptr_t)handle);
}

JNIEXPORT void JNICALL
Java_com_lumios_sdk_LumiApp_nativeDestroy(JNIEnv *env, jclass cls, jlong handle) {
    (void)env; (void)cls;
    lumi_app_destroy((lumi_app_t *)(intptr_t)handle);
}

/* ── Log ───────────────────────────────────────────────────────── */

JNIEXPORT void JNICALL
Java_com_lumios_sdk_LumiApp_logInfo(JNIEnv *env, jclass cls, jstring tag, jstring msg) {
    (void)cls;
    const char *t = jstring_to_cstr(env, tag);
    const char *m = jstring_to_cstr(env, msg);
    lumi_log(LUMI_LOG_INFO, t, "%s", m);
    release_cstr(env, tag, t);
    release_cstr(env, msg, m);
}

JNIEXPORT void JNICALL
Java_com_lumios_sdk_LumiApp_logError(JNIEnv *env, jclass cls, jstring tag, jstring msg) {
    (void)cls;
    const char *t = jstring_to_cstr(env, tag);
    const char *m = jstring_to_cstr(env, msg);
    lumi_log(LUMI_LOG_ERROR, t, "%s", m);
    release_cstr(env, tag, t);
    release_cstr(env, msg, m);
}

JNIEXPORT void JNICALL
Java_com_lumios_sdk_LumiApp_logDebug(JNIEnv *env, jclass cls, jstring tag, jstring msg) {
    (void)cls;
    const char *t = jstring_to_cstr(env, tag);
    const char *m = jstring_to_cstr(env, msg);
    lumi_log(LUMI_LOG_DEBUG, t, "%s", m);
    release_cstr(env, tag, t);
    release_cstr(env, msg, m);
}

/* ── Storage ───────────────────────────────────────────────────── */

JNIEXPORT jint JNICALL
Java_com_lumios_sdk_LumiApp_storageSet(JNIEnv *env, jclass cls, jstring key, jstring val) {
    (void)cls;
    const char *k = jstring_to_cstr(env, key);
    const char *v = jstring_to_cstr(env, val);
    jint r = lumi_storage_set(k, v);
    release_cstr(env, key, k);
    release_cstr(env, val, v);
    return r;
}

JNIEXPORT jstring JNICALL
Java_com_lumios_sdk_LumiApp_storageGet(JNIEnv *env, jclass cls, jstring key) {
    (void)cls;
    const char *k = jstring_to_cstr(env, key);
    const char *v = lumi_storage_get(k);
    release_cstr(env, key, k);
    return v ? (*env)->NewStringUTF(env, v) : NULL;
}

JNIEXPORT jint JNICALL
Java_com_lumios_sdk_LumiApp_storageRemove(JNIEnv *env, jclass cls, jstring key) {
    (void)cls;
    const char *k = jstring_to_cstr(env, key);
    jint r = lumi_storage_remove(k);
    release_cstr(env, key, k);
    return r;
}

JNIEXPORT jint JNICALL
Java_com_lumios_sdk_LumiApp_storageClear(JNIEnv *env, jclass cls) {
    (void)env; (void)cls;
    return lumi_storage_clear();
}

/* ── Notifications ─────────────────────────────────────────────── */

JNIEXPORT jint JNICALL
Java_com_lumios_sdk_LumiApp_notify(JNIEnv *env, jclass cls, jstring title, jstring body) {
    (void)cls;
    const char *t = jstring_to_cstr(env, title);
    const char *b = jstring_to_cstr(env, body);
    jint r = lumi_notify_simple(t, b);
    release_cstr(env, title, t);
    release_cstr(env, body, b);
    return r;
}
