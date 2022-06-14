package com.github.yutianzuo.curl_native;


import android.content.Context;
import android.util.Log;
import android.util.SparseArray;

import com.github.yutianzuo.curl_native.utils.Misc;
import com.github.yutianzuo.curl_native.utils.ThreadHelper;

import java.nio.charset.Charset;
import java.util.ArrayList;
import java.util.List;

public enum HttpManager {
    INSTANCE;

    public static final int RESULT_FAILED = -1;
    public static final int RESULT_ALL_SUCCESS = 0;
    public static final int RESULT_DOWNLOAD_PROGRESS = 1;
    public static final int RESULT_UPLOAD_PROGRESS = 2;

    private List<RequestManager> mRequests = new ArrayList<>();
    private SparseArray<HttpCallback> mCallbackMap = new SparseArray<>();

    public void Init(int threadPoolSize, Context context) {
        JniCurl.init(threadPoolSize, this);
        Misc.copyCertFile(context);
    }

    public void Uninit() {
        JniCurl.unInit();
    }

    public RequestManager getRequest() {
        RequestManager request = new RequestManager();
        mRequests.add(request);
        return request;
    }

    public void addCallback(int seq, HttpCallback callback) {
        mCallbackMap.put(seq, callback);
    }

    /**
     * jni callback, dont proguard
     * callback invoke in mainthread
     */
    public void callBack(int type, final byte[] strResponse, final float persent, int seq,
            final int errcode) {
        //Log.e("JAVA_TAG", strResponse);
        final HttpCallback callback = mCallbackMap.get(seq);
        final String strResponse_safe = strResponse == null ? "" : new String(strResponse, Charset.defaultCharset());
        if (callback != null) {
            if (type == RESULT_FAILED) {
                ThreadHelper.runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        callback.fail(errcode);
                    }
                });

            } else {
                if (type == RESULT_DOWNLOAD_PROGRESS || type == RESULT_UPLOAD_PROGRESS) {
                    ThreadHelper.runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            callback.progress(persent);
                        }
                    });
                } else if (type == RESULT_ALL_SUCCESS) {
                    ThreadHelper.runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            callback.success(strResponse_safe);
                        }
                    });
                }
            }
        }
    }

}
