package com.example.deskbuddyapp;

import android.app.Application;
import android.content.Context;

//class for getting context to be able to read brokerinfo in the MqttHandler class
public class ContextHelper extends Application {
    private static Context context;

    @Override
    public void onCreate() {
        super.onCreate();
        context = this;
    }
    public static Context getContext(){
        return context;
    }
}
