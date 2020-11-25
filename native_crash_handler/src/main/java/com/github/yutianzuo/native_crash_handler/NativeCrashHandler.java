package com.github.yutianzuo.native_crash_handler;

public class NativeCrashHandler {
    static final String TAG = "NativeCrashHandler";
    static {
        System.loadLibrary("native_crash_handler");
    }

    public static native boolean installNativeCrashHandler(String path);
}
