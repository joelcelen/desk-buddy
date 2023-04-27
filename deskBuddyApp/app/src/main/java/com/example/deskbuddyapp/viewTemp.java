package com.example.deskbuddyapp;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.view.View;

import java.util.ArrayList;
import java.util.Calendar;

import java.util.Date;



public class viewTemp extends AppCompatActivity {
    int preference = 24;    //hard coded preference
    ArrayList <Double> hourlyReading = new ArrayList<>();     //hard coded data
    ArrayList <Double> weeklyReading = new ArrayList<>();     // hard coded data

    viewTemp(int preference){
        this.preference = preference;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_view_temp);
        Date currentTime = Calendar.getInstance().getTime();

    }

    private void dailyGraph(View view){

    }
    private void weeklyGraph(View view){

    }
}