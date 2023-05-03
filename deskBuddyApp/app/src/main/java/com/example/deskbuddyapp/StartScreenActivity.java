package com.example.deskbuddyapp;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;

public class StartScreenActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_start_screen);
        new Handler().postDelayed(() -> {
            Intent intent = new Intent(StartScreenActivity.this, MainActivity.class);
            startActivity(intent);
            finish();
        }, 3000);
    }
}