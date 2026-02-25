/**
 * LumiView.java — View system JNI bindings
 * Copyright 2026 Lumi Team. Apache-2.0
 */
package com.lumios.sdk;

public class LumiView {
    long nativeHandle;

    private LumiView(long handle) {
        this.nativeHandle = handle;
    }

    /* ── Factories ─────────────────────────────────────────────── */
    public static LumiView column()    { return new LumiView(nativeColumn()); }
    public static LumiView row()       { return new LumiView(nativeRow()); }
    public static LumiView stack()     { return new LumiView(nativeStack()); }
    public static LumiView text(String content) { return new LumiView(nativeText(content)); }
    public static LumiView button(String label) { return new LumiView(nativeButton(label)); }
    public static LumiView image(String source) { return new LumiView(nativeImage(source)); }
    public static LumiView textField(String placeholder) { return new LumiView(nativeTextField(placeholder)); }
    public static LumiView spacer()    { return new LumiView(nativeSpacer()); }
    public static LumiView divider()   { return new LumiView(nativeDivider()); }

    /* ── Tree ──────────────────────────────────────────────────── */
    public void addChild(LumiView child) { nativeAddChild(nativeHandle, child.nativeHandle); }
    public void removeChild(LumiView child) { nativeRemoveChild(nativeHandle, child.nativeHandle); }

    /* ── Properties ────────────────────────────────────────────── */
    public void setId(String id)        { nativeSetId(nativeHandle, id); }
    public void setVisible(boolean v)   { nativeSetVisible(nativeHandle, v); }
    public void setWidth(float w)       { nativeSetWidth(nativeHandle, w); }
    public void setHeight(float h)      { nativeSetHeight(nativeHandle, h); }
    public void setBackground(int rgba) { nativeSetBackground(nativeHandle, rgba); }
    public void setForeground(int rgba) { nativeSetForeground(nativeHandle, rgba); }
    public void setFontSize(float s)    { nativeSetFontSize(nativeHandle, s); }
    public void setText(String t)       { nativeSetText(nativeHandle, t); }
    public String getText()             { return nativeGetText(nativeHandle); }

    /* ── Callbacks ─────────────────────────────────────────────── */
    public interface OnClickListener { void onClick(LumiView view); }

    public void setOnClickListener(OnClickListener listener) {
        nativeSetOnClick(nativeHandle, listener);
    }

    public void destroy() {
        if (nativeHandle != 0) {
            nativeDestroyView(nativeHandle);
            nativeHandle = 0;
        }
    }

    /* ── Native methods ────────────────────────────────────────── */
    private static native long nativeColumn();
    private static native long nativeRow();
    private static native long nativeStack();
    private static native long nativeText(String content);
    private static native long nativeButton(String label);
    private static native long nativeImage(String source);
    private static native long nativeTextField(String placeholder);
    private static native long nativeSpacer();
    private static native long nativeDivider();
    private static native void nativeAddChild(long parent, long child);
    private static native void nativeRemoveChild(long parent, long child);
    private static native void nativeDestroyView(long handle);
    private static native void nativeSetId(long handle, String id);
    private static native void nativeSetVisible(long handle, boolean v);
    private static native void nativeSetWidth(long handle, float w);
    private static native void nativeSetHeight(long handle, float h);
    private static native void nativeSetBackground(long handle, int rgba);
    private static native void nativeSetForeground(long handle, int rgba);
    private static native void nativeSetFontSize(long handle, float s);
    private static native void nativeSetText(long handle, String t);
    private static native String nativeGetText(long handle);
    private static native void nativeSetOnClick(long handle, OnClickListener listener);
}
