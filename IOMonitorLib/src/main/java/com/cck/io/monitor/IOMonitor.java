package com.cck.io.monitor;

import androidx.annotation.NonNull;

public class IOMonitor {
    private static final String TAG = "IOMonitor";
    static {
        System.loadLibrary("io_monitor");
    }

    public static void install(@NonNull Config config) {
        int ret = nativeInit(config.isDebug);
        if (ret < 0) {
            Logger.i(TAG, "native init fail, error: " + ret);
        }
    }

    //used by c++
    @SuppressWarnings("unused")
    private static JavaContext createJavaContext() {
        return new JavaContext(shrinkCallstack(new Throwable()),
                Thread.currentThread().getId(),
                Thread.currentThread().getName()
                );
    }

    private static native int nativeInit(boolean debug);

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
