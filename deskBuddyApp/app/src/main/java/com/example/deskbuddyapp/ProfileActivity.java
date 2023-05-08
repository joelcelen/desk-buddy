package com.example.deskbuddyapp;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ViewFlipper;
import com.google.android.material.slider.Slider;
import com.google.firebase.database.DatabaseReference;
import com.google.firebase.database.FirebaseDatabase;
import com.google.firebase.database.DataSnapshot;
import com.google.firebase.database.DatabaseError;
import com.google.firebase.database.ValueEventListener;

import java.util.HashMap;

public class ProfileActivity extends AppCompatActivity {

    private ViewFlipper viewFlipper;
    private Profile currentProfile;
    private Profile profile1;
    private Profile profile2;
    private Profile profile3;
    private Profile profile4;
    private double sliderTemp;
    private double sliderHum;
    private double sliderLight;

    FirebaseDatabase database = FirebaseDatabase.getInstance();
    DatabaseReference profilesRef = database.getReference("Profiles");

    private HashMap<Profile, Button> profileList;



    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_profile);

        // Get a reference to the ViewFlipper
        viewFlipper = findViewById(R.id.view_flipper);

        profileList = new HashMap<>();
        createProfiles();


        // Buttons to toggle between settings and profile page.
        Button settingsButton = findViewById(R.id.btnSettings);
        Button backButton = findViewById(R.id.back_button);
        Button homeButton = findViewById(R.id.btnHome);

        settingsButton.setOnClickListener(v -> {
            Slider tempSlider = findViewById(R.id.sldTemp);
            Slider humSlider = findViewById(R.id.sldHum);
            Slider lightSlider = findViewById(R.id.sldLight);
            EditText nameEditor = findViewById(R.id.editName);

            nameEditor.setText(currentProfile.getProfileName());
            tempSlider.setValue((float) currentProfile.getTemperature());
            humSlider.setValue((float) currentProfile.getHumidity());
            lightSlider.setValue((float) currentProfile.getLightLevel());
            viewFlipper.showNext();
        });
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
            currentProfile.setProfileName(nameEditor.getText().toString());
            profilesRef.child(currentProfile.getId().toString()).setValue(currentProfile);
            profileList.get(currentProfile).setText(updateButtonTxt(currentProfile));
            viewFlipper.showPrevious();
        });
    }

    // Create profiles, later it will be replaced by reading from database.
    public void createProfiles(){
        profilesRef.addListenerForSingleValueEvent(new ValueEventListener() {
            @Override
            public void onDataChange(DataSnapshot dataSnapshot) {
                // Iterate over the profiles
                int index = 0;
                for (DataSnapshot profileSnapshot : dataSnapshot.getChildren()) {
                    // Get the profile data
                    int humidity = profileSnapshot.child("humidity").getValue(Integer.class);
                    int lightLevel = profileSnapshot.child("lightLevel").getValue(Integer.class);
                    String profileName = profileSnapshot.child("profileName").getValue(String.class);
                    int temperature = profileSnapshot.child("temperature").getValue(Integer.class);

                    // Create a new Profile object and add it to the appropriate list
                    Profile profile = new Profile(profileName,temperature, humidity, lightLevel,index+1);

                    // Assign profile to the appropriate index
                    switch (index) {
                        case 0:
                            profile1 = profile;
                            break;
                        case 1:
                            profile2 = profile;
                            break;
                        case 2:
                            profile3 = profile;
                            break;
                        case 3:
                            profile4 = profile;
                            break;
                    }
                    index++;
                }

                // Buttons to set active profile
                Button profile1Button = findViewById(R.id.btnProfile1);
                Button profile2Button = findViewById(R.id.btnProfile2);
                Button profile3Button = findViewById(R.id.btnProfile3);
                Button profile4Button = findViewById(R.id.btnProfile4);
                profile1Button.setText(updateButtonTxt(profile1));
                profile2Button.setText(updateButtonTxt(profile2));
                profile3Button.setText(updateButtonTxt(profile3));
                profile4Button.setText(updateButtonTxt(profile4));

                // Assign buttons to their respective profiles
                profileList.put(profile1, profile1Button);
                profileList.put(profile2, profile2Button);
                profileList.put(profile3, profile3Button);
                profileList.put(profile4, profile4Button);

                // Current profile is set to a button on start
                profileList.put(currentProfile, profile1Button);

                // Set current profile to the profile associated with the button pressed
                profileList.get(profile1).setOnClickListener(v -> currentProfile = profile1);
                profileList.get(profile2).setOnClickListener(v -> currentProfile = profile2);
                profileList.get(profile3).setOnClickListener(v -> currentProfile = profile3);
                profileList.get(profile4).setOnClickListener(v -> currentProfile = profile4);
            }

            @Override
            public void onCancelled(DatabaseError databaseError) {
                // Handle database error
            }
        });
    }




    // Method to update the values and display them on the button.
    public String updateButtonTxt(Profile profile){
        return profile.getProfileName() + "\n Temp: " + profile.getTemperature()
                + "\n Hum: " + profile.getHumidity() + "\n Lux: " + profile.getLightLevel();
    }

    public void goToViewOne(View view) {
        viewFlipper.setDisplayedChild(0);
    }

    public void goToViewTwo(View view) {
        viewFlipper.setDisplayedChild(1);
    }


}