//
// Created by chenlong on 2021/8/26.
//
#include <iostream>
#include <android/log.h>
#include <xhook.h>
#include <jni.h>

#define TAG "IO_MONITOR_JNI"

int hook_test_return_1() {
    return 20;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_cck_xhook_XHook_init(JNIEnv *env, jobject thiz) {
    int ret;
    xhook_enable_debug(1);
    ret = xhook_register(".*/libhook_test.so$", "_Z13test_return_1v", (void *) hook_test_return_1,
                         NULL);//注意这里c++的符号并不是函数名称
    if (ret < 0) {
        __android_log_print(ANDROID_LOG_DEBUG, TAG, "register fail");
    } else {
        __android_log_print(ANDROID_LOG_DEBUG, TAG, "register success");
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_cck_xhook_XHook_refresh(JNIEnv *env, jobject thiz, jint async) {
    int ret = xhook_refresh(async);
    if (ret < 0) {
        __android_log_print(ANDROID_LOG_DEBUG, TAG, "hook fail");
    } else {
        __android_log_print(ANDROID_LOG_DEBUG, TAG, "hook success");
    }
}
