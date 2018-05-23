#include <jni.h>
#include <string>
#include "android_utils.h"

#include "manager/httpmanager.h"


JavaVM *g_jvm;
jobject g_obj = nullptr;

jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    g_jvm = vm;
    return JNI_VERSION_1_6;
}

static void GlobalCallBackFunc(int result, const std::string &respones, float persent, size_t seq,
                               int errcode, void *extra) {
    LOGE("%s", respones.c_str());
    HttpManager::get_instance()->manual_lock();

    JNIEnv *env = NULL;
    if (g_jvm) {
        g_jvm->AttachCurrentThread(&env, NULL);
    }

    if (env && g_obj) {
        jclass clazz = env->GetObjectClass(g_obj);
        jmethodID method = env->GetMethodID(clazz, "callBack", "(ILjava/lang/String;FII)V");
        jstring jrespones = env->NewStringUTF(respones.c_str());
        env->CallVoidMethod(g_obj, method, result, jrespones, persent, seq, errcode);
    }


    if (g_jvm) {
        g_jvm->DetachCurrentThread();
    }

    HttpManager::get_instance()->manual_unlock();
}


extern "C"
JNIEXPORT void JNICALL
Java_com_github_yutianzuo_curl_1native_JniCurl_init(JNIEnv *env, jclass type, jint threadPoolSize, jobject callBack) {
    HttpManager::init(threadPoolSize);
    if (callBack) {
        if (g_obj) {
            env->DeleteGlobalRef(g_obj);
        }
        g_obj = env->NewGlobalRef(callBack);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_github_yutianzuo_curl_1native_JniCurl_unInit(JNIEnv *env, jclass type) {
    HttpManager::uninit();
    if (g_obj) {
        env->DeleteGlobalRef(g_obj);
        g_obj = NULL;
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_github_yutianzuo_curl_1native_JniCurl_addBasicHeader(JNIEnv *env, jclass type, jstring strHash_,
                                                              jstring strKey_, jstring strValue_) {
    const char *strHash = env->GetStringUTFChars(strHash_, 0);
    const char *strKey = env->GetStringUTFChars(strKey_, 0);
    const char *strValue = env->GetStringUTFChars(strValue_, 0);

    if (strHash && strKey && strValue) {
        RequestManager *p = HttpManager::get_instance()->get_request_manager(strHash);
        p->add_basic_headers(strKey, strValue);
    }

    env->ReleaseStringUTFChars(strHash_, strHash);
    env->ReleaseStringUTFChars(strKey_, strKey);
    env->ReleaseStringUTFChars(strValue_, strValue);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_github_yutianzuo_curl_1native_JniCurl_addBasicURLParam(JNIEnv *env, jclass type, jstring strHash_,
                                                                jstring strKey_, jstring strValue_) {
    const char *strHash = env->GetStringUTFChars(strHash_, 0);
    const char *strKey = env->GetStringUTFChars(strKey_, 0);
    const char *strValue = env->GetStringUTFChars(strValue_, 0);

    if (strHash && strKey && strValue) {
        RequestManager *p = HttpManager::get_instance()->get_request_manager(strHash);
        p->add_basic_url_params(strKey, strValue);
    }

    env->ReleaseStringUTFChars(strHash_, strHash);
    env->ReleaseStringUTFChars(strKey_, strKey);
    env->ReleaseStringUTFChars(strValue_, strValue);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_github_yutianzuo_curl_1native_JniCurl_setHost(JNIEnv *env, jclass type, jstring strHash_, jstring strHost_) {
    const char *strHash = env->GetStringUTFChars(strHash_, 0);
    const char *strHost = env->GetStringUTFChars(strHost_, 0);

    if (strHash && strHost) {
        RequestManager *p = HttpManager::get_instance()->get_request_manager(strHash);
        p->set_host(strHost);
    }

    env->ReleaseStringUTFChars(strHash_, strHash);
    env->ReleaseStringUTFChars(strHost_, strHost);
}

using JNIMAP = std::map<std::string, std::string>;

static JNIMAP gen_map(JNIEnv *env, jobject headers_keys, jobject headers_values) {
    JNIMAP map_ret;
    if (headers_keys && headers_values) {
        jclass cls_list_key = env->GetObjectClass(headers_keys);
        jclass cls_list_value = env->GetObjectClass(headers_values);
        if (cls_list_key && cls_list_value) {
            jmethodID arraylist_key_get = env->GetMethodID(cls_list_key, "get",
                                                           "(I)Ljava/lang/Object;");
            jmethodID arraylist_key_size = env->GetMethodID(cls_list_key, "size", "()I");

            jmethodID arraylist_value_get = env->GetMethodID(cls_list_value, "get",
                                                             "(I)Ljava/lang/Object;");
            jmethodID arraylist_value_size = env->GetMethodID(cls_list_value, "size", "()I");

            if (arraylist_key_get && arraylist_key_size && arraylist_value_get && arraylist_value_size) {
                jint len_key = env->CallIntMethod(headers_keys, arraylist_key_size);
                jint len_value = env->CallIntMethod(headers_values, arraylist_value_size);
                if (len_key > 0 && len_key == len_value) {
                    for (int i = 0; i < len_key; ++i) {
                        jstring jstr_key = (jstring) env->CallObjectMethod(headers_keys, arraylist_key_get, i);
                        jstring jstr_value = (jstring) env->CallObjectMethod(headers_values, arraylist_value_get, i);
                        if (jstr_key && jstr_value) {
                            const char *sz_key = (char *) env->GetStringUTFChars(jstr_key,
                                                                                 NULL);
                            const char *sz_value = (char *) env->GetStringUTFChars(jstr_value,
                                                                                   NULL);
                            if (sz_key && sz_value) {

                                map_ret[sz_key] = sz_value;

                                env->ReleaseStringUTFChars(jstr_key, sz_key);
                                env->ReleaseStringUTFChars(jstr_value, sz_value);
                            }
                        }
                    }
                }
            }
        }
    }
    return map_ret;
}


extern "C"
JNIEXPORT void JNICALL
Java_com_github_yutianzuo_curl_1native_JniCurl_get(JNIEnv *env, jclass type, jstring strHash_, jint requestSeq,
                                                   jstring strPath_, jobject headers_keys, jobject headers_values,
                                                   jobject params_keys, jobject params_values) {
    const char *strHash = env->GetStringUTFChars(strHash_, 0);
    const char *strPath = env->GetStringUTFChars(strPath_, 0);

    if (strHash && strPath) {
        JNIMAP inner_headers = gen_map(env, headers_keys, headers_values);
        JNIMAP inner_url_params = gen_map(env, params_keys, params_values);
        RequestManager *p = HttpManager::get_instance()->get_request_manager(strHash);
        p->get(strPath, inner_headers, inner_url_params, GlobalCallBackFunc, requestSeq);
    }

    env->ReleaseStringUTFChars(strHash_, strHash);
    env->ReleaseStringUTFChars(strPath_, strPath);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_github_yutianzuo_curl_1native_JniCurl_setCertPath(JNIEnv *env, jclass type, jstring strHash_,
                                                           jstring strCertPath_) {
    const char *strHash = env->GetStringUTFChars(strHash_, 0);
    const char *strCertPath = env->GetStringUTFChars(strCertPath_, 0);

    if (strHash && strCertPath) {
        RequestManager *p = HttpManager::get_instance()->get_request_manager(strHash);
        p->set_cert_path(strCertPath);
    }

    env->ReleaseStringUTFChars(strHash_, strHash);
    env->ReleaseStringUTFChars(strCertPath_, strCertPath);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_github_yutianzuo_curl_1native_JniCurl_setProxy(JNIEnv *env, jclass type, jstring strHash_, jstring proxy_) {
    const char *strHash = env->GetStringUTFChars(strHash_, 0);
    const char *proxy = env->GetStringUTFChars(proxy_, 0);

    if (strHash && proxy) {
        RequestManager *p = HttpManager::get_instance()->get_request_manager(strHash);
        p->set_proxy_path(proxy);
    }

    env->ReleaseStringUTFChars(strHash_, strHash);
    env->ReleaseStringUTFChars(proxy_, proxy);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_github_yutianzuo_curl_1native_JniCurl_postFromData(JNIEnv *env, jclass type, jstring strHash_, jint requestSeq,
                                                    jstring strPath_, jobject headers_keys, jobject headers_values,
                                                    jobject params_keys, jobject params_values) {
    const char *strHash = env->GetStringUTFChars(strHash_, 0);
    const char *strPath = env->GetStringUTFChars(strPath_, 0);

    if (strHash && strPath) {
        JNIMAP inner_headers = gen_map(env, headers_keys, headers_values);
        JNIMAP inner_url_params = gen_map(env, params_keys, params_values);
        RequestManager *p = HttpManager::get_instance()->get_request_manager(strHash);
        p->post_form(strPath, inner_headers, inner_url_params, GlobalCallBackFunc, requestSeq);
    }

    env->ReleaseStringUTFChars(strHash_, strHash);
    env->ReleaseStringUTFChars(strPath_, strPath);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_github_yutianzuo_curl_1native_JniCurl_postJson(JNIEnv *env, jclass type, jstring strHash_, jint requestSeq,
                                                        jstring strPath_, jobject headers_keys, jobject headers_values,
                                                        jstring strJson_) {
    const char *strHash = env->GetStringUTFChars(strHash_, 0);
    const char *strPath = env->GetStringUTFChars(strPath_, 0);
    const char *strJson = env->GetStringUTFChars(strJson_, 0);

    std::string str_json;
    if (strJson) {
        str_json = strJson;
    }


    if (strHash && strPath) {
        JNIMAP inner_headers = gen_map(env, headers_keys, headers_values);
        RequestManager *p = HttpManager::get_instance()->get_request_manager(strHash);
        p->post_json(strPath, inner_headers, str_json, GlobalCallBackFunc, requestSeq);
    }

    env->ReleaseStringUTFChars(strHash_, strHash);
    env->ReleaseStringUTFChars(strPath_, strPath);
    env->ReleaseStringUTFChars(strJson_, strJson);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_github_yutianzuo_curl_1native_JniCurl_putJson(JNIEnv *env, jclass type, jstring strHash_, jint requestSeq,
                                                       jstring strPath_, jobject headers_keys, jobject headers_values,
                                                       jstring strJson_) {
    const char *strHash = env->GetStringUTFChars(strHash_, 0);
    const char *strPath = env->GetStringUTFChars(strPath_, 0);
    const char *strJson = env->GetStringUTFChars(strJson_, 0);

    std::string str_json;
    if (strJson) {
        str_json = strJson;
    }

    if (strHash && strPath) {
        JNIMAP inner_headers = gen_map(env, headers_keys, headers_values);
        RequestManager *p = HttpManager::get_instance()->get_request_manager(strHash);
        p->put(strPath, str_json, inner_headers, GlobalCallBackFunc, requestSeq);
    }


    env->ReleaseStringUTFChars(strHash_, strHash);
    env->ReleaseStringUTFChars(strPath_, strPath);
    env->ReleaseStringUTFChars(strJson_, strJson);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_github_yutianzuo_curl_1native_JniCurl_postFile(JNIEnv *env, jclass type, jstring strHash_, jint requestSeq,
                                                        jstring strPath_, jobject headers_keys, jobject headers_values,
                                                        jstring strFormName_, jobject params_keys,
                                                        jobject params_values, jstring strJsonName_, jstring strJson_,
                                                        jstring strFileKeyName_, jstring strFilePath_,
                                                        jstring strFileName_) {
    const char *strHash = env->GetStringUTFChars(strHash_, 0);
    const char *strPath = env->GetStringUTFChars(strPath_, 0);
    const char *strFormName = env->GetStringUTFChars(strFormName_, 0);
    const char *strJsonName = env->GetStringUTFChars(strJsonName_, 0);
    const char *strJson = env->GetStringUTFChars(strJson_, 0);
    const char *strFileKeyName = env->GetStringUTFChars(strFileKeyName_, 0);
    const char *strFilePath = env->GetStringUTFChars(strFilePath_, 0);
    const char *strFileName = env->GetStringUTFChars(strFileName_, 0);

    std::string str_formname;
    std::string str_jsonname;
    std::string str_json;
    std::string str_filekeyname;
    std::string str_filename;
    std::string str_filepath;

    if (strFormName) {
        str_formname = strFormName;
    }
    if (strJsonName) {
        str_jsonname = strJsonName;
    }
    if (strJson) {
        str_json = strJson;
    }
    if (strFileKeyName) {
        str_filekeyname = strFileKeyName;
    }
    if (strFilePath) {
        str_filepath = strFilePath;
    }
    if (strFileName) {
        str_filename = strFileName;
    }

    if (strHash && strPath) {
        JNIMAP inner_headers = gen_map(env, headers_keys, headers_values);
        JNIMAP inner_form_params = gen_map(env, params_keys, params_values);
        RequestManager *p = HttpManager::get_instance()->get_request_manager(strHash);
        p->post_file(strPath, inner_headers, str_formname, inner_form_params, str_jsonname, str_json,
                     str_filekeyname, str_filename, str_filepath, GlobalCallBackFunc,
                     requestSeq);
    }

    env->ReleaseStringUTFChars(strHash_, strHash);
    env->ReleaseStringUTFChars(strPath_, strPath);
    env->ReleaseStringUTFChars(strFormName_, strFormName);
    env->ReleaseStringUTFChars(strJsonName_, strJsonName);
    env->ReleaseStringUTFChars(strJson_, strJson);
    env->ReleaseStringUTFChars(strFileKeyName_, strFileKeyName);
    env->ReleaseStringUTFChars(strFilePath_, strFilePath);
    env->ReleaseStringUTFChars(strFileName_, strFileName);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_github_yutianzuo_curl_1native_JniCurl_download(JNIEnv *env, jclass type, jstring strHash_, jint requestSeq,
                                                        jstring strPath_, jobject headers_keys, jobject headers_values,
                                                        jobject params_keys, jobject params_values,
                                                        jstring strFilePath_) {
    const char *strHash = env->GetStringUTFChars(strHash_, 0);
    const char *strPath = env->GetStringUTFChars(strPath_, 0);
    const char *strFilePath = env->GetStringUTFChars(strFilePath_, 0);

    std::string str_filepath;
    if (strFilePath) {
        str_filepath = strFilePath;
    }

    if (strHash && strPath) {
        JNIMAP inner_headers = gen_map(env, headers_keys, headers_values);
        JNIMAP inner_url_params = gen_map(env, params_keys, params_values);
        RequestManager *p = HttpManager::get_instance()->get_request_manager(strHash);
        p->download(strPath, inner_headers, inner_url_params, str_filepath, GlobalCallBackFunc, requestSeq);
    }


    env->ReleaseStringUTFChars(strHash_, strHash);
    env->ReleaseStringUTFChars(strPath_, strPath);
    env->ReleaseStringUTFChars(strFilePath_, strFilePath);
}