package com.example.deskbuddyapp;
import android.app.Application;
import android.content.Context;

//class for getting context to be able to read brokerinfo in the MqttHandler class
//Credit to this Stack overflow entry: https://stackoverflow.com/questions/4391720/how-can-i-get-a-resource-content-from-a-static-context/4391811#4391811
//Author of response: https://stackoverflow.com/users/244296/cristian

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
