package com.example.deskbuddyapp;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ViewFlipper;
import com.google.android.material.slider.Slider;
import java.util.HashMap;

public class ProfileActivity extends AppCompatActivity {

    private ViewFlipper viewFlipper;
    private RoomProfile currentProfile;
    private RoomProfile profile1;
    private RoomProfile profile2;
    private RoomProfile profile3;
    private RoomProfile profile4;
    private double sliderTemp;
    private double sliderHum;
    private double sliderLight;

    private HashMap<RoomProfile, Button> profileList;

    Slider slider;



    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_profile);

        //slider = findViewById(R.id.sldTemp);
       // slider.setSty(R.style.Widget_MaterialComponents_Slider);

        // Get a reference to the ViewFlipper
        viewFlipper = findViewById(R.id.view_flipper);

        profileList = new HashMap<>();
        createProfiles();

        // Set current profile to the profile associated with the button pressed
        profileList.get(profile1).setOnClickListener(v -> currentProfile = profile1);
        profileList.get(profile2).setOnClickListener(v -> currentProfile = profile2);
        profileList.get(profile3).setOnClickListener(v -> currentProfile = profile3);
        profileList.get(profile4).setOnClickListener(v -> currentProfile = profile4);

        // Buttons to toggle between settings and profile page.
        Button settingsButton = findViewById(R.id.btnSettings);
        Button backButton = findViewById(R.id.back_button);
        Button homeButton = findViewById(R.id.btnHome);

        settingsButton.setOnClickListener(v -> {viewFlipper.showNext();});
        backButton.setOnClickListener(v -> viewFlipper.showPrevious());
        homeButton.setOnClickListener(v -> {
            Intent home = new Intent(ProfileActivity.this, MainActivity.class);
            startActivity(home);
        });

        // Sliders and adjusters.
        Slider tempSlider = findViewById(R.id.sldTemp);
        Slider humSlider = findViewById(R.id.sldHum);
        Slider lightSlider = findViewById(R.id.sldLight);
        EditText nameEditor = findViewById(R.id.editName);

        // Methods to store the slider values in temporary variables
        tempSlider.addOnChangeListener((slider, value, fromUser) -> sliderTemp = value);
        humSlider.addOnChangeListener((slider, value, fromUser) -> sliderHum = value);
        lightSlider.addOnChangeListener((slider, value, fromUser) -> sliderLight = value);

        // Button to save changes to current profile and go back to profiles view.
        Button saveButton = findViewById(R.id.btnSave);
        saveButton.setOnClickListener(v -> {
            currentProfile.setTemperature(sliderTemp);
            currentProfile.setHumidity(sliderHum);
            currentProfile.setLightLevel(sliderLight);
            currentProfile.setRoomName(nameEditor.getText().toString());
            profileList.get(currentProfile).setText(updateButtonTxt(currentProfile));
            viewFlipper.showPrevious();
        });
    }

    // Create profiles, later it will be replaced by reading from database.
    public void createProfiles(){

        currentProfile = new RoomProfile();
        profile1 = new RoomProfile();
        profile2 = new RoomProfile();
        profile3 = new RoomProfile();
        profile4 = new RoomProfile();

        // Buttons to set active profile
        Button profile1Button = findViewById(R.id.btnProfile1);
        Button profile2Button = findViewById(R.id.btnProfile2);
        Button profile3Button = findViewById(R.id.btnProfile3);
        Button profile4Button = findViewById(R.id.btnProfile4);

        // Assign buttons to their respective profiles
        profileList.put(profile1, profile1Button);
        profileList.put(profile2, profile2Button);
        profileList.put(profile3, profile3Button);
        profileList.put(profile4, profile4Button);

        // Current profile is set to a button on start
        profileList.put(currentProfile, profile1Button);
    }

    // Method to update the values and display them on the button.
    public String updateButtonTxt(RoomProfile profile){
        return profile.getRoomName() + "\n Temp: " + profile.getTemperature()
                + "\n Hum: " + profile.getHumidity() + "\n Lux: " + profile.getLightLevel();
    }

    public void goToViewOne(View view) {
        viewFlipper.setDisplayedChild(0);
    }

    public void goToViewTwo(View view) {
        viewFlipper.setDisplayedChild(1);
    }


}