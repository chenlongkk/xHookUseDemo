#include <jni.h>
#include "be_hooked.h"
#include <android/log.h>
//
// Created by chenlong on 2021/8/26.
//

int test() {
    int a = test_return_1();
    return a;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_cck_xhook_HookTest_getValue(JNIEnv *env, jobject thiz) {
    return test();
}
