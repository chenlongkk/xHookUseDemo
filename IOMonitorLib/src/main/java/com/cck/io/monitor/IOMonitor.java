package com.cck.io.monitor;

import android.util.Log;

import androidx.annotation.NonNull;

import java.util.List;
import java.util.concurrent.CopyOnWriteArrayList;

public class IOMonitor {
    private static final String TAG = "IOMonitor";
    private final List<OnEventListener> mOnEventListeners = new CopyOnWriteArrayList<>();

    static {
        System.loadLibrary("io_monitor");
    }

    public static void install(@NonNull Config config) {
        int ret = nativeInit(config.isDebug,
                config.mainThreadIoTotalTimeMax,
                config.mainThreadIoSingleTimeMax,
                config.bufferMin,
                config.fileReadCountMax,
                config.fileWriteCountMax,
                config.sameReadCountMax);
        if (ret < 0) {
            Logger.i(TAG, "native init fail, error: " + ret);
        }
    }

    private static JavaContext createJavaContext() {
        return new JavaContext(shrinkCallstack(new Throwable()),
                Thread.currentThread().getId(),
                Thread.currentThread().getName()
                );
    }

    private static native int nativeInit(boolean debug,
                                   int mainThreadIoTotalTimeMax,//主线程连续io总耗时最大值
                                   int mainThreadIoSingleTimeMax,//主线程单次io耗时最大值
                                   int bufferMin,//最小buffer
                                   int fileReadCountMax,//单个文件最大读次数
                                   int fileWriteCountMax,//单个最大写次数
                                   int sameReadCountMax//同一个文件同一个线程最大读次数
                                   );


    private static String shrinkCallstack(Throwable stack) {
        StackTraceElement[] elements = stack.getStackTrace();
        StringBuilder sb = new StringBuilder();
        for (int i = elements.length - 1; i >= 0 ; i--) {
            StackTraceElement trace = elements[i];
            if (trace.getClassName().contains("java.io.")
                || trace.getClassName().contains("com.cck.io.monitor.")) {
                continue;
            }
            sb.insert(0, trace.toString() + "\n");
        }
        return sb.toString();
    }
}
