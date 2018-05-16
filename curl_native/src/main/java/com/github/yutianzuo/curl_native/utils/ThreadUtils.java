package com.github.yutianzuo.curl_native.utils;

import android.os.Handler;
import android.os.Looper;

public class ThreadUtils {

    private static final Handler uiHandler = new Handler(Looper.getMainLooper());

    public static int getThreadCount() {
        return Thread.getAllStackTraces().size();
    }

    public static String getCurThreadName() {
        return Thread.currentThread().getName();
    }

    public static void runOnUiThread(Runnable action) {
        if (action == null) { return; }

        if (ThreadUtils.getCurThreadId() == Looper.getMainLooper().getThread().getId()) {
            action.run();
        } else {
            uiHandler.post(action);
        }
    }

    public static long getCurThreadId() {
        return Thread.currentThread().getId();
    }
}
