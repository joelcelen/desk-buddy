package com.example.deskbuddyapp;
import android.app.Application;
import android.content.Context;

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
