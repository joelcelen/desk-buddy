package com.example.deskbuddyapp;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.content.SharedPreferences;
import android.graphics.Color;
import android.graphics.Typeface;
import android.os.Bundle;
import android.text.Html;
import android.text.Spannable;
import android.text.SpannableStringBuilder;
import android.text.TextUtils;
import android.text.style.RelativeSizeSpan;
import android.text.style.StyleSpan;
import android.view.View;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.ViewFlipper;
import com.google.android.material.slider.Slider;
import com.google.gson.Gson;
import com.google.gson.reflect.TypeToken;
import java.lang.reflect.Type;
import java.util.ArrayList;

public class ProfileActivity extends AppCompatActivity {

    private ViewFlipper viewFlipper;
    private Button saveButton;
    private Button settingsButton;
    private Button backButton;
    private Button homeButton;
    private Slider tempSlider;
    private Slider humSlider;
    private Slider lightSlider;
    private EditText nameEditor;
    private ArrayList<RoomProfile> profileList;
    private ArrayList<Button> buttonList;
    private Button suggestionsButton;
    private TextView suggestionsTextView;
    private boolean isSuggestionsTextVisible;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_profile);
        viewFlipper = findViewById(R.id.view_flipper);
        profileList = new ArrayList<>();
        buttonList = new ArrayList<>();

        loadData();
        initializeUiElements();
        updateButtonText();

        // Set onClick listeners for the profile buttons to switch profile.
        buttonList.get(1).setOnClickListener(v -> switchProfile(profileList.get(1)));
        buttonList.get(2).setOnClickListener(v -> switchProfile(profileList.get(2)));
        buttonList.get(3).setOnClickListener(v -> switchProfile(profileList.get(3)));
        buttonList.get(4).setOnClickListener(v -> switchProfile(profileList.get(4)));

        // Set onClick listener that flips the active view to settings view.
        settingsButton.setOnClickListener(v -> {

            nameEditor.setText(findActiveProfile().getProfileName());
            tempSlider.setValue((float) findActiveProfile().getTemperature());
            humSlider.setValue((float) findActiveProfile().getHumidity());
            lightSlider.setValue((float) findActiveProfile().getLightLevel());
            viewFlipper.showNext();
        });

        // Set onClick listener to save the changes applied in the settings view.
        saveButton.setOnClickListener(v -> {

            // Set the profile values to the slider values.
            findActiveProfile().setProfileName(nameEditor.getText().toString());
            findActiveProfile().setTemperature((double)tempSlider.getValue());
            findActiveProfile().setHumidity((double)humSlider.getValue());
            findActiveProfile().setLightLevel((double)lightSlider.getValue());
            buttonList.get(findActiveProfile().getId()).setText(updateButtonTxt(findActiveProfile()));
            saveData();

            viewFlipper.showPrevious();

        });

        // Set onClick listener to go back to the profile view.
        backButton.setOnClickListener(v -> viewFlipper.showPrevious());

        // Set onClick listener to go back to the main menu.
        homeButton.setOnClickListener(v -> {
            Intent home = new Intent(ProfileActivity.this, MainActivity.class);
            startActivity(home);
        });

        //Set onClick listener for when the suggestions (questionmark) button is pressed,
        //when pressed, set the visibility of the corresponding textview to VISIBLE
        //When pressed again, set the visibility of the corresponding textview to INVISIBLE
        //Short fade in/out animation is called depending on if VISIBLE or INVISIBLE
        suggestionsButton.setOnClickListener(v -> {
            if(isSuggestionsTextVisible){
                Animation fadeOut = AnimationUtils.loadAnimation(getApplicationContext(), R.anim.fade_out);
                suggestionsTextView.startAnimation(fadeOut);
                suggestionsTextView.setVisibility(View.INVISIBLE);
                isSuggestionsTextVisible = false;
            }
            else {
                Animation fadeIn = AnimationUtils.loadAnimation(getApplicationContext(), R.anim.fade_in);
                suggestionsTextView.startAnimation(fadeIn);
                suggestionsTextView.setVisibility(View.VISIBLE);
                suggestionsTextView.setText(getSuggestionsText());
                isSuggestionsTextVisible = true;
            }
        });
    }

    /** This method uses the SharedPreferences class in combination with Gson class
     * to save the RoomProfile objects to a local json file. **/
    private void saveData(){
        SharedPreferences sharedPreferences = getSharedPreferences("profile", MODE_PRIVATE);
        SharedPreferences.Editor editor = sharedPreferences.edit();
        Gson gson = new Gson();
        String json = gson.toJson(profileList);
        editor.putString("Profile list", json);
        editor.apply();
    }

    /** This method uses the SharedPreferences class in combination with Gson class
     * to retrieve the RoomProfile objects from a local json file.
     * If the json file is currently empty, it will initialize a new ArrayList
     * with five default profiles to prevent any NullPointerExceptions. **/
    private void loadData(){
        SharedPreferences sharedPreferences = getSharedPreferences("profile", MODE_PRIVATE);
        Gson gson = new Gson();
        String json = sharedPreferences.getString("Profile list", null);
        Type type = new TypeToken<ArrayList<RoomProfile>>() {}.getType();
        profileList = gson.fromJson(json, type);

        if(profileList == null){
            profileList = new ArrayList<>();
            profileList.add(new RoomProfile(0, "Default Profile", 25.0,35.0,900.0));
            profileList.add(new RoomProfile(1, "Profile 1", 25.0,35.0,900.0));
            profileList.add(new RoomProfile(2, "Profile 2", 25.0,35.0,900.0));
            profileList.add(new RoomProfile(3, "Profile 3", 25.0,35.0,900.0));
            profileList.add(new RoomProfile(4, "Profile 4", 25.0,35.0,900.0));
            profileList.get(1).setActive(true);
            saveData();
        }
    }

    /** Overloaded method to access method from other activity. **/
    public ArrayList<RoomProfile> loadData(SharedPreferences sharedPreferences, ArrayList<RoomProfile> profiles) {
        Gson gson = new Gson();
        String json = sharedPreferences.getString("Profile list", null);
        Type type = new TypeToken<ArrayList<RoomProfile>>() {}.getType();
        profiles = gson.fromJson(json, type);

        if(profiles == null){
            profiles = new ArrayList<>();
            profiles.add(new RoomProfile(0, "Default Profile", 25.0,35.0,900.0));
            profiles.add(new RoomProfile(1, "Profile 1", 25.0,35.0,900.0));
            profiles.add(new RoomProfile(2, "Profile 2", 25.0,35.0,900.0));
            profiles.add(new RoomProfile(3, "Profile 3", 25.0,35.0,900.0));
            profiles.add(new RoomProfile(4, "Profile 4", 25.0,35.0,900.0));
            profiles.get(1).setActive(true);
        }
        return profiles;
    }

    /** Initializes all buttons in the UI and binds them to the correct ID's. **/
    private void initializeUiElements(){

        /* Add buttons to list.
        Button list has first index filled with a null value to pad it,
        this makes it so that the buttons and profile share indices*/
        buttonList.add(null);
        buttonList.add(findViewById(R.id.btnProfile1));
        buttonList.add(findViewById(R.id.btnProfile2));
        buttonList.add(findViewById(R.id.btnProfile3));
        buttonList.add(findViewById(R.id.btnProfile4));

        // Update color to indicate which profile that is active.
        buttonList.get(findActiveProfile().getId()).setBackgroundColor(Color.RED);

        // Utility buttons.
        saveButton = findViewById(R.id.btnSave);
        settingsButton = findViewById(R.id.btnSettings);
        backButton = findViewById(R.id.back_button);
        homeButton = findViewById(R.id.btnHome);
        suggestionsButton = findViewById(R.id.suggestionsButton);

        // Sliders and adjusters.
        tempSlider = findViewById(R.id.sldTemp);
        humSlider = findViewById(R.id.sldHum);
        lightSlider = findViewById(R.id.sldLight);
        nameEditor = findViewById(R.id.editName);

        //textView for Suggestions
        suggestionsTextView = findViewById(R.id.suggestionsTextView);
    }

    /** This method finds the currently active profile by going through the profileList
     * and returning the profile that is active. **/
    protected RoomProfile findActiveProfile(){
        for(RoomProfile profile : profileList){
            if(profile.isActive()){
                return profile;
            }
        }
        return null;
    }

    /** This method goes through the buttonList and updates every entry to the current text. **/
    public void updateButtonText(){
        for(int i = 1; i < buttonList.size(); i ++){
            buttonList.get(i).setText(updateButtonTxt(profileList.get(i)));
        }
    }

    /** This method is responsible for switching what profile is active.**/
    private void switchProfile(RoomProfile profile){

        // Go through the profileList to set all profiles to inactive.
        for(RoomProfile profileEntry : profileList){
            profileEntry.setActive(false);
        }
        // Reset button color.
        for(int i = 1; i<buttonList.size(); i++){
            //TODO change background color to default button color, grey is just placeholder.
            buttonList.get(i).setBackgroundColor(Color.LTGRAY);
        }
        // Set intended profile to active and switch button color to indicate active profile.
        profile.setActive(true);
        //TODO set color to whatever you want to represent the current active profile.
        buttonList.get(profile.getId()).setBackgroundColor(Color.RED);
        saveData();
    }

    /** Method to update the values and display them on the button. **/
    public String updateButtonTxt(RoomProfile roomProfile){
        return roomProfile.getProfileName() + "\n Temp: " + roomProfile.getTemperature()
                + "\n Hum: " + roomProfile.getHumidity() + "\n Lux: " + roomProfile.getLightLevel();
    }

    /** These methods keep track of the different views in the XML file.
     * Although they are marked as not used, they are necessary for the
     * ViewFlipper class to switch between the views in the XML file. **/
    public void goToViewOne(View view) {
        viewFlipper.setDisplayedChild(0);
    }

    public void goToViewTwo(View view) {
        viewFlipper.setDisplayedChild(1);
    }

    //Method for constructing and editing the text that shows up in the suggestionsTextView.
    //Utilizes spannableStringbuilder object which is constructe through sections, one for each paragraph
    //The object is then returned and used in the listener for the suggestionsTextView
    public SpannableStringBuilder getSuggestionsText() {
        SpannableStringBuilder suggestionsText = new SpannableStringBuilder();

        //Title
        String suggestionsTitle = "Suggestions and recommendations\n\n";
        suggestionsText.append(suggestionsTitle);
        suggestionsText.setSpan(new RelativeSizeSpan(1.4f), 0, suggestionsTitle.length(), Spannable.SPAN_EXCLUSIVE_EXCLUSIVE);
        suggestionsText.setSpan(new StyleSpan(Typeface.BOLD), 0, suggestionsTitle.length(), Spannable.SPAN_EXCLUSIVE_EXCLUSIVE);

        //Humidity
        String humTitle = "Humidity: ";
        String humText = "Maintaining optimal humidity levels (40-60%) is crucial for a comfortable and healthy workplace. It prevents dry skin, respiratory issues, and mold growth while reducing static electricity and improving air quality.\n\n";
        suggestionsText.append(humTitle);
        suggestionsText.setSpan(new StyleSpan(Typeface.BOLD), suggestionsText.length() - humTitle.length(), suggestionsText.length(), Spannable.SPAN_EXCLUSIVE_EXCLUSIVE);
        suggestionsText.append(humText);

        // Temperature
        String temperatureTitle = "Temperature: ";
        String temperatureContent = "The ideal temperature range (20-24°C) promotes productivity and well-being. Individual temperature control options and flexible solutions like fans or space heaters accommodate preferences and ensure employees can adjust their surroundings for comfort.\n\n";
        suggestionsText.append(temperatureTitle);
        suggestionsText.setSpan(new StyleSpan(android.graphics.Typeface.BOLD), suggestionsText.length() - temperatureTitle.length(), suggestionsText.length(), Spannable.SPAN_EXCLUSIVE_EXCLUSIVE);
        suggestionsText.append(temperatureContent);

        // Light levels
        String lightTitle = "Light levels: ";
        String lightContent = "Balanced lighting, preferably natural light, enhances productivity and well-being. Adequate artificial lighting should be used when natural light is limited, minimizing glare and eye strain. Aim for 500-1,000 lux for general office work and provide adjustable lighting options for individual needs, incorporating ergonomic design principles for optimal conditions.\n\n";
        suggestionsText.append(lightTitle);
        suggestionsText.setSpan(new StyleSpan(android.graphics.Typeface.BOLD), suggestionsText.length() - lightTitle.length(), suggestionsText.length(), Spannable.SPAN_EXCLUSIVE_EXCLUSIVE);
        suggestionsText.append(lightContent);

        //Remove trailing linebreak
        if(!TextUtils.isEmpty(suggestionsText)){
            suggestionsText.delete(suggestionsText.length() - 1, suggestionsText.length());
        }
        return suggestionsText;
    }

    public ArrayList<RoomProfile> getProfileList() {return profileList;}

    public ArrayList<Button> getButtonList() {return buttonList;}
}