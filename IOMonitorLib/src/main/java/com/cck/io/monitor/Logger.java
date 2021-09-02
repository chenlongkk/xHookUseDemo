package com.cck.io.monitor;

import android.util.Log;

import androidx.annotation.NonNull;

import java.util.Locale;

public class Logger {
    public static final String TAG = "MiPlayTrace";
    private static volatile boolean sDebuggable = true;

    public static void setDebuggable(boolean debuggable) {
        sDebuggable = debuggable;
    }

    public static void d(@NonNull String tag, @NonNull String msg) {
        if (sDebuggable) {
            Log.d(tag, msg);
        }
    }

    public static void w(@NonNull String tag, @NonNull String msg) {
        Log.w(tag, msg);
    }

    public static void d(@NonNull String tag, @NonNull String format, Object...params) {
        if (sDebuggable) {
            Log.d(tag, String.format(Locale.getDefault(), format, params));
        }
    }

    public static void i(@NonNull String tag, @NonNull String format, Object...params) {
        Log.d(tag, String.format(Locale.getDefault(), format, params));
    }

    public static void e(@NonNull String tag, @NonNull String msg, @NonNull Throwable error) {
        Log.d(tag, msg + "\n" + Log.getStackTraceString(error));
    }
}
