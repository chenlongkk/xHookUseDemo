package com.cck.xhook;

public class HookTest {
    static {
        System.loadLibrary("hook_test");
    }

    public native int getValue();
}
