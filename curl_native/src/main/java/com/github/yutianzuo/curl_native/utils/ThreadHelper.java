package com.github.yutianzuo.curl_native.utils;

import android.os.Looper;

public class ThreadHelper
{

//    private static final ExecutorService backgroundTasksExecutor = Executors
//            .newFixedThreadPool(Runtime.getRuntime().availableProcessors() * 2, new PriorityThreadFactory());

    public static void runOnUiThread(Runnable action) {
        ThreadUtils.runOnUiThread(action);
    }

    public static final boolean threadInMain() {
        if (Thread.currentThread().getId() == Looper.getMainLooper().getThread().getId())
            return true;
        return false;
    }

//    public static Future<?> runOnBackgroundThread(Runnable action) {
//        if (action == null)
//            return null;
//
//        return backgroundTasksExecutor.submit(action);
//    }
}
