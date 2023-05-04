package com.example.deskbuddyapp;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.ToggleButton;

import java.util.ArrayList;

public class ProfileActivity extends AppCompatActivity {

    private RoomProfile defaultProfile;
    protected ArrayList<RoomProfile> profileList;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_profile);
        defaultProfile = new RoomProfile();
        profileList = new ArrayList<>();
        profileList.add(0, defaultProfile);
    }

    public void clickDefault(View view) {
        this.defaultProfile = profileList.get(0);
    }

    public void clickProfile1(View view) {
        if(profileList.get(1) != null){
            this.defaultProfile = profileList.get(1);
        }else{
            RoomProfile tempRoom = new RoomProfile();
            tempRoom.setRoomName("Profile 1");
            profileList.add(1, tempRoom);
            /* This will be connected to the Settings page once created.
            Intent settings = new Intent(this, SettingsActivity.class);
            startActivity(settings);*/
        }
    }

    public void clickProfile2(View view) {
        if(profileList.get(2) != null){
            this.defaultProfile = profileList.get(2);
        }else{
            profileList.add(2, new RoomProfile());
            /* This will be connected to the Settings page once created.
            Intent settings = new Intent(this, SettingsActivity.class);
            startActivity(settings);*/
        }
    }

    public void clickProfile3(View view) {
        if(profileList.get(3) != null){
            this.defaultProfile = profileList.get(3);
        }else{
            profileList.add(3, new RoomProfile());
            /* This will be connected to the Settings page once created.
            Intent settings = new Intent(this, SettingsActivity.class);
            startActivity(settings);*/
        }
    }

    public void clickHome(View view) {
        Intent home = new Intent(this, MainActivity.class);
        startActivity(home);
    }

    public RoomProfile getDefaultProfile(){
        return this.defaultProfile;
    }

    public ArrayList<RoomProfile> getProfileList() {
        return profileList;
    }
}