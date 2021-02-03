//
// Created by tianzuo yu on 11/24/20.
//
#include <jni.h>

#include "android_utils.h"
#include <signal.h>
#include <string>
#include <string.h>
#include <fcntl.h>
static JavaVM *g_jvm;

class JNIEnvPtr {
public:
    JNIEnvPtr() : env_{nullptr}, need_detach_{false} {
        if (g_jvm->GetEnv((void**) &env_, JNI_VERSION_1_6) ==
            JNI_EDETACHED) {
            g_jvm->AttachCurrentThread(&env_, nullptr);
            need_detach_ = true;
        }
    }

    ~JNIEnvPtr() {
        if (need_detach_) {
            g_jvm->DetachCurrentThread();
        }
    }

    JNIEnv* operator->() {
        return env_;
    }

private:
    JNIEnvPtr(const JNIEnvPtr&) = delete;
    JNIEnvPtr& operator=(const JNIEnvPtr&) = delete;

private:
    JNIEnv* env_;
    bool need_detach_;
};

static struct sigaction old_sa[NSIG];
static std::string g_path;

static
void android_sigaction(int signal, siginfo_t *info, void *reserved)
{
    LOGE("catch native crash, signal:%d", signal);
    ::open(g_path.data(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
    if (old_sa[signal].sa_sigaction) {
        old_sa[signal].sa_sigaction(signal, info, reserved);
    }
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *jvm, void *reserved)
{
    g_jvm = jvm;
    return JNI_VERSION_1_6;
}


extern "C"
JNIEXPORT jboolean JNICALL
Java_com_github_yutianzuo_native_1crash_1handler_NativeCrashHandler_installNativeCrashHandler
(JNIEnv *env, jclass clazz, jstring path) {
    g_path.clear();
    const char* native_path = nullptr;
    if (path) {
        native_path = env->GetStringUTFChars(path, 0);
    }
    if (path && native_path) {
        g_path = native_path;
        env->ReleaseStringUTFChars(path, native_path);
    } else {
        LOGE("path is null");
        return JNI_FALSE;
    }
    LOGD("gpath is:%s", g_path.data());

    ///try to catch native crash(signal)
    struct sigaction handler;
    memset(&handler, 0, sizeof(struct sigaction));
    handler.sa_sigaction = android_sigaction;
    handler.sa_flags = SA_RESETHAND | SA_SIGINFO;

#define CATCHSIG(X) sigaction(X, &handler, &old_sa[X])
    int ret = CATCHSIG(SIGILL);
    if (ret != 0) {
        LOGE("SIGILL ret is : %d", ret);
    }
    ret = CATCHSIG(SIGABRT);
    if (ret != 0) {
        LOGE("SIGABRT ret is : %d", ret);
    }
    ret = CATCHSIG(SIGBUS);
    if (ret != 0) {
        LOGE("SIGBUS ret is : %d", ret);
    }
    ret = CATCHSIG(SIGFPE);
    if (ret != 0) {
        LOGE("SIGFPE ret is : %d", ret);
    }
    ret = CATCHSIG(SIGSEGV);
    if (ret != 0) {
        LOGE("SIGSEGV ret is : %d", ret);
    }

    ret = CATCHSIG(SIGSTKFLT);
    if (ret != 0) {
        LOGE("SIGSTKFLT ret is : %d", ret);
    }

    ret = CATCHSIG(SIGPIPE);
    if (ret != 0) {
        LOGE("SIGPIPE ret is : %d", ret);
    }

    ret = CATCHSIG(SIGTRAP);
    if (ret != 0) {
        LOGE("SIGTRAP ret is : %d", ret);
    }

    ret = CATCHSIG(SIGSYS);
    if (ret != 0) {
        LOGE("SIGSYS ret is : %d", ret);
    }

    ret = CATCHSIG(SIGUSR1);
    if (ret != 0) {
        LOGE("SIGUSR1 ret is : %d", ret);
    }

    ret = CATCHSIG(SIGUSR2);
    if (ret != 0) {
        LOGE("SIGUSR2 ret is : %d", ret);
    }

    return JNI_TRUE;
}