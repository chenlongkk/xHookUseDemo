//
// Created by chenlong on 2021/8/26.
//
#include <iostream>
#include <android/log.h>
#include <cstdio>
#include <xhook.h>
#include <memory>
#include <jni.h>
#include <fcntl.h>
#include <unistd.h>
#include "io_monitor.h"
#include "utils.h"
#include "io_log.h"

namespace iomonitor {
    static JavaVM *kJavaVm;
    static jclass kIOMonitorClass;
    static jclass kJavaContextClass;
    static jmethodID kMethodIDGetJavaContext;
    static jfieldID kFieldIDStack;
    static jfieldID  kFieldIDThreadId;
    static jfieldID  kFieldIDThreadName;
}

int hook_open(const char *path, int flags, mode_t mode) {
    int fd = open(path, flags, mode);
    using namespace iomonitor;
    if (kJavaVm == nullptr) {
        return fd;
    }
    int ret;
    JNIEnv *env;
    if ((ret = kJavaVm->GetEnv((void**)&env, JNI_VERSION_1_6)) != JNI_OK) {
        return fd;
    }
    jobject java_obj = env->CallStaticObjectMethod(kIOMonitorClass, kMethodIDGetJavaContext);
    jstring j_stack = static_cast<jstring>(env->GetObjectField(java_obj, kFieldIDStack));
    const char *j_stack_tmp = env->GetStringUTFChars(j_stack, nullptr);
    std::string stack = std::string(strdup(j_stack_tmp));
    env->ReleaseStringUTFChars(j_stack, j_stack_tmp);
    jstring j_thread_name = static_cast<jstring>(env->GetObjectField(java_obj, kFieldIDThreadName));
    const char *j_thread_name_tmp = env->GetStringUTFChars(j_thread_name, nullptr);
    std::string thread_name = std::string(strdup(j_thread_name_tmp));
    env->ReleaseStringUTFChars(j_thread_name, j_thread_name_tmp);
    jlong j_thread_id = env->GetLongField(java_obj, kFieldIDThreadId);
    JavaContext java_context = JavaContext(stack, thread_name, (long)j_thread_id);
    IOMonitor::get().on_open(fd, path, flags, mode, java_context);
    LOGI("hook open: %s, fd:%d", path, fd);
    LOGI("%s", stack.c_str());
    return fd;
}

ssize_t hook_read(int fd, void *buf, size_t count) {
    LOGI("hook read, fd:%d, buffer count %zu", fd, count);
    int64_t start = get_tick_count_micros();
    ssize_t ret = read(fd, buf, count);
    int64_t read_cost = get_tick_count_micros() - start;
    iomonitor::IOMonitor::get().on_read(fd, buf, count, ret, read_cost);
    return ret;
}

ssize_t hook_write(int fd, const void *buf, size_t count) {
    LOGI("hook write, fd: %d, buffer size %zu", fd, count);
    int64_t start = get_tick_count_micros();
    ssize_t ret = write(fd, buf, count);
    int64_t write_cost = get_tick_count_micros() - start;
    iomonitor::IOMonitor::get().on_write(fd, buf, count, ret, write_cost);
    return ret;
}

int hook_close(int fd) {
    LOGI("hook close, fd:%d", fd);
    iomonitor::IOMonitor::get().on_close(fd);
    return close(fd);
}

const static char *TARGET_MODULE_PATTERNS[] = {
        ".*/libopenjdkjvm.so$",
        ".*/libjavacore.so$",
        ".*/libopenjdk.so$"
};

extern "C"
JNIEXPORT jint JNICALL
Java_com_cck_io_monitor_IOMonitor_nativeInit(JNIEnv *env, jclass clazz, jboolean debug) {
    xhook_enable_debug(debug ? 1 : 0);
    int ret;
    for (auto target : TARGET_MODULE_PATTERNS) {
        ret = xhook_register(target, "open", (void *) hook_open, nullptr);
        if (ret < 0) {
            LOGI("register open fail, pattern: %s, err: %d", target, ret);
            return ret;
        }
        ret = xhook_register(target, "open64", (void *) hook_open, nullptr);
        if (ret < 0) {
            LOGI("register open64 fail, pattern: %s, err: %d", target, ret);
            return ret;
        }
        bool is_java_core_lib = strstr(target, ".*/libjavacore.so$") != nullptr;
        if (is_java_core_lib) {
            ret = xhook_register(target, "read", (void *) hook_read, nullptr);
            if (ret < 0) {
                LOGI("register read fail, pattern: %s, err: %d", target, ret);
                return ret;
            }
            ret = xhook_register(target, "write", (void *) hook_write, nullptr);
            if (ret < 0) {
                LOGI("register write fail, pattern: %s err: %d", target, ret);
                return ret;
            }
            ret = xhook_register(target, "close", (void *) hook_close, nullptr);
            if (ret < 0) {
                LOGI("register close fail, pattern: %s, err: %d", target, ret);
                return ret;
            }
        }
    }

    ret = xhook_refresh(1);
    if (ret < 0) {
        LOGI("hook refresh fail, err: %d", ret);
        return ret;
    }
    JavaVM *vm;
    env->GetJavaVM(&vm);
    iomonitor::kJavaVm = vm;
    jclass tmp_class = env->FindClass("com/cck/io/monitor/IOMonitor");
    if (tmp_class == nullptr) {
        LOGI("");
        return JNI_ERR;
    }
    iomonitor::kIOMonitorClass = reinterpret_cast<jclass>(env->NewGlobalRef(tmp_class));
    tmp_class = env->FindClass("com/cck/io/monitor/JavaContext");
    if (tmp_class == nullptr) {
        LOGI("");
        return JNI_ERR;
    }
    iomonitor::kJavaContextClass = reinterpret_cast<jclass>(env->NewGlobalRef(tmp_class));
    iomonitor::kFieldIDStack = env->GetFieldID(iomonitor::kJavaContextClass, "stack", "Ljava/lang/String;");
    iomonitor::kFieldIDThreadName = env->GetFieldID(iomonitor::kJavaContextClass, "threadName", "Ljava/lang/String;");
    iomonitor::kFieldIDThreadId = env->GetFieldID(iomonitor::kJavaContextClass, "threadId", "J");
    iomonitor::kMethodIDGetJavaContext = env->GetStaticMethodID(iomonitor::kIOMonitorClass, "createJavaContext", "()Lcom/cck/io/monitor/JavaContext;");
    return ret;
}