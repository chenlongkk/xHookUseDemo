package com.cck.xhook;

import android.content.Context;

public class XHook {
    private static volatile XHook sInstance;
    static {
        System.loadLibrary("x_hook_use");
    }
    public static XHook getInstance() {
        if (sInstance == null) {
            synchronized (XHook.class) {
                sInstance = new XHook();
            }
        }
        return sInstance;
    }

    public void setup(Context context) {
        init();
    }

    private native void init();

    public native void refresh(int async);
}
