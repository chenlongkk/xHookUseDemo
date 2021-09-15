#include <jni.h>
#include "x_log.h"
#include <cstring>


//
// Created by chenlong on 2021/9/7.
//
namespace xlog {
    static jclass kXLogClazz;
    static jfieldID kXLogPtrField;

}

static xlog::XLog *getXLog(JNIEnv *env, jobject thiz) {
    jlong ptr_field = env->GetLongField(thiz, xlog::kXLogPtrField);
    if (ptr_field == 0) {
        return nullptr;
    }
    return reinterpret_cast<xlog::XLog*>(ptr_field);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_cck_io_monitor_XLog_nativeInit(JNIEnv *env, jobject thiz, jstring buffer_dir) {
    jclass clazz = env->FindClass("com/cck/io/monitor/XLog");
    if (clazz == nullptr) {
        return JNI_ERR;
    }
    jfieldID ptr_field_id = env->GetFieldID(clazz, "mPtr", "J");
    if (ptr_field_id == nullptr) {
        return JNI_ERR;
    }
    xlog::kXLogClazz = clazz;
    xlog::kXLogPtrField = ptr_field_id;
    const char* jpath = env->GetStringUTFChars(buffer_dir, nullptr);
    const char* path = strdup(jpath);
    env->ReleaseStringUTFChars(buffer_dir, jpath);
    auto xlog = new xlog::XLog(path);
    env->SetLongField(thiz, ptr_field_id, reinterpret_cast<jlong>(xlog));
    return JNI_OK;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_cck_io_monitor_XLog_nativeRelease(JNIEnv *env, jobject thiz) {
    auto xlog = getXLog(env, thiz);
    if (xlog == nullptr) {
        return;
    }
    xlog->release();
    delete xlog;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_cck_io_monitor_XLog_nativeWrite(JNIEnv *env, jobject thiz, jlong ptr, jstring msg) {
    auto *xlog = reinterpret_cast<xlog::XLog*>(ptr);
    if (xlog == nullptr) {
        return;
    }
    int len = env->GetStringUTFLength(msg);
    if (len <= 0) {
        return;
    }
    const char *j_msg = env->GetStringUTFChars(msg, nullptr);
    const char *log_msg = strdup(j_msg);
    env->ReleaseStringUTFChars(msg, j_msg);
    xlog->write(log_msg);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_cck_io_monitor_XLog_nativeFlush(JNIEnv *env, jobject thiz) {
    auto xlog = getXLog(env, thiz);
    xlog->flush();
}

