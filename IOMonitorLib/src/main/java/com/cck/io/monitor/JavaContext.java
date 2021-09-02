package com.cck.io.monitor;

public class JavaContext {
    public final String stack;
    public final long threadId;
    public final String threadName;

    public JavaContext(String stack, long threadId, String threadName) {
        this.stack = stack;
        this.threadId = threadId;
        this.threadName = threadName;
    }
}
