package com.cck.io.monitor;


import android.content.Context;

public class XLog {
    static {
        System.loadLibrary("x_log");
    }

    private long mPtr;
    private final boolean mInitSuccess;

    public XLog(XLogConfig config) {
        int ret = nativeInit(config.bufferPath);
        mInitSuccess = ret == 0 && mPtr != 0;
    }

    public void release() {
        nativeRelease();
        mPtr = 0;
    }

    public void flush() {
        nativeFlush();
    }

    public void write(String msg) {
        if (mPtr == 0 || !mInitSuccess) {
            return;
        }
        nativeWrite(mPtr, msg);
    }

    private native int nativeInit(String bufferPath);
    private native void nativeRelease();
    private native void nativeFlush();
    private native void nativeWrite(long ptr, String msg);
}
