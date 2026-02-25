/**
 * LumiApp.java — Java JNI bindings for LumiOS Application SDK
 * Copyright 2026 Lumi Team. Apache-2.0
 */
package com.lumios.sdk;

public class LumiApp {
    static {
        System.loadLibrary("lumiapp_jni");
    }

    /* ── Result codes ──────────────────────────────────────────── */
    public static final int OK         =  0;
    public static final int ERR_NOMEM  = -1;
    public static final int ERR_INVALID = -2;
    public static final int ERR_NOT_FOUND = -3;
    public static final int ERR_IO     = -4;

    /* ── Lifecycle interface ───────────────────────────────────── */
    public interface Lifecycle {
        default void onCreate()  {}
        default void onStart()   {}
        default void onResume()  {}
        default void onPause()   {}
        default void onStop()    {}
        default void onDestroy() {}
    }

    /* ── Native methods ────────────────────────────────────────── */
    private long nativeHandle;

    public LumiApp(String appId, String name, String version, Lifecycle lifecycle) {
        this.nativeHandle = nativeCreate(appId, name, version, lifecycle);
        if (this.nativeHandle == 0) {
            throw new RuntimeException("Failed to create LumiApp");
        }
    }

    public int run() {
        return nativeRun(nativeHandle);
    }

    public void quit() {
        nativeQuit(nativeHandle);
    }

    public void destroy() {
        if (nativeHandle != 0) {
            nativeDestroy(nativeHandle);
            nativeHandle = 0;
        }
    }

    @Override
    protected void finalize() {
        destroy();
    }

    private static native long nativeCreate(String appId, String name, String version, Lifecycle lifecycle);
    private static native int  nativeRun(long handle);
    private static native void nativeQuit(long handle);
    private static native void nativeDestroy(long handle);

    /* ── Log ───────────────────────────────────────────────────── */
    public static native void logInfo(String tag, String message);
    public static native void logError(String tag, String message);
    public static native void logDebug(String tag, String message);

    /* ── Storage ───────────────────────────────────────────────── */
    public static native int    storageSet(String key, String value);
    public static native String storageGet(String key);
    public static native int    storageRemove(String key);
    public static native int    storageClear();

    /* ── Notifications ─────────────────────────────────────────── */
    public static native int notify(String title, String body);
}
