package com.perfxlab.bankcarddetect;

import android.app.Application;

public class App extends Application {
    static  Application application;
    @Override
    public void onCreate() {
        super.onCreate();
        application = this;
    }

    public static Application  getAppInstance(){
        return application;
    }
}
