package com.github.yutianzuo.curl_native;

import java.util.List;

public class JniCurl {
    static {
        System.loadLibrary("native_net");
    }

    public static native void init(int threadPoolSize, Object callBack);

    public static native void unInit();

    public static native void addBasicHeader(String strHash, String strKey, String strValue);

    public static native void addBasicURLParam(String strHash, String strKey, String strValue);

    public static native void setHost(String strHash, String strHost);

    public static native void setCertPath(String strHash, String strCertPath);

    public static native void setProxy(String strHash, String proxy);

    public static native void get(String strHash, int requestSeq, String strPath, List<String> headers_keys,
            List<String> headers_values, List<String> params_keys, List<String> params_values);

    public static native void postFromData(String strHash, int requestSeq, String strPath, List<String> headers_keys,
            List<String> headers_values, List<String> params_keys, List<String> params_values);

    public static native void postJson(String strHash, int requestSeq, String strPath, List<String> headers_keys,
            List<String> headers_values, String strJson);

    public static native void putJson(String strHash, int requestSeq, String strPath, List<String> headers_keys,
            List<String> headers_values, String strJson);

    public static native void postFile(String strHash, int requestSeq, String strPath, List<String> headers_keys,
            List<String> headers_values, String strFormName, List<String> params_keys, List<String> params_values,
            String strJsonName, String strJson, String strFileKeyName, String strFilePath, String strFileName);

    public static native void download(String strHash, int requestSeq, String strPath, List<String> headers_keys,
            List<String> headers_values, List<String> params_keys, List<String> params_values, String strFilePath);

    public static native String sha256(String value);

    public static native String aesCbc(byte[] encrytData, int len, String key, String iv);
}
