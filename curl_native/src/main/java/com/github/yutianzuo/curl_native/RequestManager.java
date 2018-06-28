package com.github.yutianzuo.curl_native;


import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.UUID;
import java.util.concurrent.atomic.AtomicInteger;

public class RequestManager {
    static private AtomicInteger gSeq = new AtomicInteger(0);
    private String seqHash; //this is actually native request_manager

    public RequestManager() {
        seqHash = getHash();

    }

    private String getHash() {
        return UUID.randomUUID().toString().replace("-", "");
    }

    /**
     * @param strKey "Cookie"
     * @param strValue "xxxx"
     */
    public void addBasicHeader(String strKey, String strValue) {
        JniCurl.addBasicHeader(seqHash, strKey, strValue);
    }

    /**
     * @param strKey "key"
     * @param strValue "value"
     */
    public void addBasicUrlParams(String strKey, String strValue) {
        JniCurl.addBasicURLParam(seqHash, strKey, strValue);
    }

    public void setHost(String strHost) {
        JniCurl.setHost(seqHash, strHost);
    }

    public void setCertPath(String strCert) {
        JniCurl.setCertPath(seqHash, strCert);
    }

    public void setProxy(String strProxy) {
        JniCurl.setProxy(seqHash, strProxy);
    }

    public void get(String strPath, Map<String, String> headers, Map<String, String> params, HttpCallback callback) {
        List<String> headerKeyList = null;
        List<String> headerValueList = null;
        List<String> paramKeyList = null;
        List<String> paramValueList = null;
        if (headers != null) {
            headerKeyList = new ArrayList<>();
            headerValueList = new ArrayList<>();
            for (Map.Entry<String, String> entry : headers.entrySet()) {
                headerKeyList.add(entry.getKey());
                headerValueList.add(entry.getValue());
            }
        }

        if (params != null) {
            paramKeyList = new ArrayList<>();
            paramValueList = new ArrayList<>();
            for (Map.Entry<String, String> entry : params.entrySet()) {
                paramKeyList.add(entry.getKey());
                paramValueList.add(entry.getValue());
            }
        }

        int seq = gSeq.incrementAndGet();
        if (callback != null) {
            HttpManager.INSTANCE.addCallback(seq, callback);
        }
        JniCurl.get(seqHash, seq, strPath, headerKeyList, headerValueList, paramKeyList,
                paramValueList);
    }

    public void postForm(String strPath, Map<String, String> headers, Map<String, String> formdatas,
            HttpCallback callback) {
        List<String> headerKeyList = null;
        List<String> headerValueList = null;
        List<String> paramKeyList = null;
        List<String> paramValueList = null;
        if (headers != null) {
            headerKeyList = new ArrayList<>();
            headerValueList = new ArrayList<>();
            for (Map.Entry<String, String> entry : headers.entrySet()) {
                headerKeyList.add(entry.getKey());
                headerValueList.add(entry.getValue());
            }
        }

        if (formdatas != null) {
            paramKeyList = new ArrayList<>();
            paramValueList = new ArrayList<>();
            for (Map.Entry<String, String> entry : formdatas.entrySet()) {
                paramKeyList.add(entry.getKey());
                paramValueList.add(entry.getValue());
            }
        }

        int seq = gSeq.incrementAndGet();
        if (callback != null) {
            HttpManager.INSTANCE.addCallback(seq, callback);
        }

        JniCurl.postFromData(seqHash, seq, strPath, headerKeyList, headerValueList, paramKeyList,
                paramValueList);
    }

    public void postJson(String strPath, Map<String, String> headers, String strJson, HttpCallback callback) {
        List<String> headerKeyList = null;
        List<String> headerValueList = null;
        if (headers != null) {
            headerKeyList = new ArrayList<>();
            headerValueList = new ArrayList<>();
            for (Map.Entry<String, String> entry : headers.entrySet()) {
                headerKeyList.add(entry.getKey());
                headerValueList.add(entry.getValue());
            }
        }
        int seq = gSeq.incrementAndGet();
        if (callback != null) {
            HttpManager.INSTANCE.addCallback(seq, callback);
        }
        JniCurl.postJson(seqHash, seq, strPath, headerKeyList, headerValueList, strJson);
    }

    public void postFile(String strPath, Map<String, String> headers, String strFormName, Map<String, String> formdatas,
            String strJsonName, String Json, String strFileKeyName, String strFilePath, String strFileName,
            HttpCallback callback) {
        List<String> headerKeyList = null;
        List<String> headerValueList = null;
        List<String> paramKeyList = null;
        List<String> paramValueList = null;
        if (headers != null) {
            headerKeyList = new ArrayList<>();
            headerValueList = new ArrayList<>();
            for (Map.Entry<String, String> entry : headers.entrySet()) {
                headerKeyList.add(entry.getKey());
                headerValueList.add(entry.getValue());
            }
        }

        if (formdatas != null) {
            paramKeyList = new ArrayList<>();
            paramValueList = new ArrayList<>();
            for (Map.Entry<String, String> entry : formdatas.entrySet()) {
                paramKeyList.add(entry.getKey());
                paramValueList.add(entry.getValue());
            }
        }

        int seq = gSeq.incrementAndGet();
        if (callback != null) {
            HttpManager.INSTANCE.addCallback(seq, callback);
        }

        JniCurl.postFile(seqHash, seq, strPath, headerKeyList, headerValueList, strFormName, paramKeyList,
                paramValueList, strJsonName, Json, strFileKeyName, strFilePath, strFileName);
    }

    public void putJson(String strPath, Map<String, String> headers, String strJson, HttpCallback callback) {
        List<String> headerKeyList = null;
        List<String> headerValueList = null;
        if (headers != null) {
            headerKeyList = new ArrayList<>();
            headerValueList = new ArrayList<>();
            for (Map.Entry<String, String> entry : headers.entrySet()) {
                headerKeyList.add(entry.getKey());
                headerValueList.add(entry.getValue());
            }
        }
        int seq = gSeq.incrementAndGet();
        if (callback != null) {
            HttpManager.INSTANCE.addCallback(seq, callback);
        }
        JniCurl.putJson(seqHash, seq, strPath, headerKeyList, headerValueList, strJson);
    }

    public void downloadFile(String strPath, Map<String, String> headers, Map<String, String> params,
            String strFilePath, HttpCallback callback) {
        List<String> headerKeyList = null;
        List<String> headerValueList = null;
        List<String> paramKeyList = null;
        List<String> paramValueList = null;
        if (headers != null) {
            headerKeyList = new ArrayList<>();
            headerValueList = new ArrayList<>();
            for (Map.Entry<String, String> entry : headers.entrySet()) {
                headerKeyList.add(entry.getKey());
                headerValueList.add(entry.getValue());
            }
        }

        if (params != null) {
            paramKeyList = new ArrayList<>();
            paramValueList = new ArrayList<>();
            for (Map.Entry<String, String> entry : params.entrySet()) {
                paramKeyList.add(entry.getKey());
                paramValueList.add(entry.getValue());
            }
        }

        int seq = gSeq.incrementAndGet();
        if (callback != null) {
            HttpManager.INSTANCE.addCallback(seq, callback);
        }

        JniCurl.download(seqHash, seq, strPath, headerKeyList, headerValueList, paramKeyList,
                paramValueList, strFilePath);
    }

}
