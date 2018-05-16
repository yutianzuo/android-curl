package com.github.yutianzuo.curl_native.utils;

import android.os.Process;

import java.util.concurrent.ThreadFactory;
import java.util.concurrent.atomic.AtomicInteger;

/**
 *
 *         线程统一出处，默认线程优先级为{@link Process#THREAD_PRIORITY_BACKGROUND}
 */
public class PriorityThreadFactory implements ThreadFactory
{

    private int mThreadPriority = Process.THREAD_PRIORITY_BACKGROUND;
    private final AtomicInteger mCount = new AtomicInteger(1);

    public PriorityThreadFactory() {
    }

    public PriorityThreadFactory(int threadPriority) {
        this.mThreadPriority = threadPriority;
    }

    @SuppressWarnings("NullableProblems")
    public Thread newThread(final Runnable runnable) {
        return newThread(runnable, "PriorityThreadFactory#" + mCount.getAndIncrement());
    }

    public Thread newThread(final Runnable runnable, final String name) {
        Runnable wrapperRunnable = new Runnable() {
            public void run() {
                try {
                    Process.setThreadPriority(PriorityThreadFactory.this.mThreadPriority);
                } catch (Throwable e) {
                }

                runnable.run();
            }
        };
        return new Thread(wrapperRunnable, name);
    }
}
