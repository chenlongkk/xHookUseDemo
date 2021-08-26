package com.cck.xhook;

import android.app.Application;

public class App extends Application {
    @Override
    public void onCreate() {
        super.onCreate();
        XHook.getInstance().setup(this);

    }
}
