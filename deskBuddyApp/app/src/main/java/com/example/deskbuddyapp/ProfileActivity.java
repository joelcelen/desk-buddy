package com.example.deskbuddyapp;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
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
    private RoomProfile currentRoomProfile;
    private RoomProfile roomProfile1;
    private RoomProfile roomProfile2;
    private RoomProfile roomProfile3;
    private RoomProfile roomProfile4;
    private double sliderTemp;
    private double sliderHum;
    private double sliderLight;

    FirebaseDatabase database = FirebaseDatabase.getInstance();
    DatabaseReference profilesRef = database.getReference("Profiles");

    private HashMap<RoomProfile, Button> profileList;



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

            nameEditor.setText(currentRoomProfile.getProfileName());
            tempSlider.setValue((float) currentRoomProfile.getTemperature());
            humSlider.setValue((float) currentRoomProfile.getHumidity());
            lightSlider.setValue((float) currentRoomProfile.getLightLevel());
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

            currentRoomProfile.setTemperature(sliderTemp);
            currentRoomProfile.setHumidity(sliderHum);
            currentRoomProfile.setLightLevel(sliderLight);
            currentRoomProfile.setProfileName(nameEditor.getText().toString());
            profilesRef.child(currentRoomProfile.getId().toString()).setValue(currentRoomProfile);
            profileList.get(currentRoomProfile).setText(updateButtonTxt(currentRoomProfile));
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
                    RoomProfile roomProfile = new RoomProfile(profileName,temperature, humidity, lightLevel,index+1);

                    // Assign profile to the appropriate index
                    switch (index) {
                        case 0:
                            roomProfile1 = roomProfile;
                            break;
                        case 1:
                            roomProfile2 = roomProfile;
                            break;
                        case 2:
                            roomProfile3 = roomProfile;
                            break;
                        case 3:
                            roomProfile4 = roomProfile;
                            break;
                    }
                    index++;
                }

                // Buttons to set active profile
                Button profile1Button = findViewById(R.id.btnProfile1);
                Button profile2Button = findViewById(R.id.btnProfile2);
                Button profile3Button = findViewById(R.id.btnProfile3);
                Button profile4Button = findViewById(R.id.btnProfile4);
                profile1Button.setText(updateButtonTxt(roomProfile1));
                profile2Button.setText(updateButtonTxt(roomProfile2));
                profile3Button.setText(updateButtonTxt(roomProfile3));
                profile4Button.setText(updateButtonTxt(roomProfile4));

                // Assign buttons to their respective profiles
                profileList.put(roomProfile1, profile1Button);
                profileList.put(roomProfile2, profile2Button);
                profileList.put(roomProfile3, profile3Button);
                profileList.put(roomProfile4, profile4Button);

                // Current profile is set to a button on start
                profileList.put(currentRoomProfile, profile1Button);

                // Set current profile to the profile associated with the button pressed
                profileList.get(roomProfile1).setOnClickListener(v -> currentRoomProfile = roomProfile1);
                profileList.get(roomProfile2).setOnClickListener(v -> currentRoomProfile = roomProfile2);
                profileList.get(roomProfile3).setOnClickListener(v -> currentRoomProfile = roomProfile3);
                profileList.get(roomProfile4).setOnClickListener(v -> currentRoomProfile = roomProfile4);
            }

            @Override
            public void onCancelled(DatabaseError databaseError) {
                // Handle database error
            }
        });
    }




    // Method to update the values and display them on the button.
    public String updateButtonTxt(RoomProfile roomProfile){
        return roomProfile.getProfileName() + "\n Temp: " + roomProfile.getTemperature()
                + "\n Hum: " + roomProfile.getHumidity() + "\n Lux: " + roomProfile.getLightLevel();
    }

    public void goToViewOne(View view) {
        viewFlipper.setDisplayedChild(0);
    }

    public void goToViewTwo(View view) {
        viewFlipper.setDisplayedChild(1);
    }


}