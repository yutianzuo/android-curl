package com.github.yutianzuo.curl_native;

public interface HttpCallback {
    void success(String respones);
    void progress(float persent);
    void fail(int errcode);
}
