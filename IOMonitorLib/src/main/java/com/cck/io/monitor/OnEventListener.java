package com.cck.io.monitor;

public interface OnEventListener {
    void onEvent(IOEvent event, String path, StackTraceElement[] callstack);
}
