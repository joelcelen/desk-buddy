package com.example.deskbuddyapp;

import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.SwitchCompat;
import androidx.constraintlayout.widget.ConstraintLayout;

import android.annotation.SuppressLint;
import android.content.Intent;
import android.content.SharedPreferences;
import android.graphics.drawable.AnimationDrawable;
import android.os.Bundle;
import android.widget.Button;
import android.widget.TextView;
import com.google.firebase.database.DatabaseReference;
import com.google.firebase.database.FirebaseDatabase;
import java.util.ArrayList;


public class MainActivity extends AppCompatActivity {
    private MqttHandler client;

    //Initialize buttons on home screen.
    private Button tempButton;
    private Button lightButton;
    private Button humButton;
    private Button profilesButton;
    private Button reminderButton;
    private ArrayList<RoomProfile> profileList;
    private TextView tempView;
    private TextView lightView;
    private TextView humView;
    private int temperatureCounter = 0;
    private int humidityCounter = 0;
    private int lightCounter = 0;
    private final int TEMPERATURE_COUNT_MAX = 60; // 1 minute = 60 seconds
    private final int LIGHT_COUNT_MAX = 60; // 1 minute = 60 seconds
    private final int HUMIDITY_COUNT_MAX = 60; // 1 minute = 60 seconds
    private double temperatureSum = 0.0;
    private double lightSum = 0.0;
    private double humiditySum = 0.0;

    DatabaseReference databaseNode;     //Firebase database
    

    //method for creating and starting the app
    @SuppressLint("MissingInflatedId")
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Initiate background animation.
        BackgroundAnimator animator = new BackgroundAnimator();
        animator.animateBackground(findViewById(R.id.activity_main));

        tempView = findViewById(R.id.temp_view);
        humView = findViewById(R.id.hum_view);
        lightView = findViewById(R.id.light_view);

        //gets the broker info from a file that is in the .gitignore,
        //connects based on the info in the txt file with the necessary information to connect to secure broker

        client = MqttHandler.getInstance(); //gets singleton instance
        client.connect();

        //Call methods for subscribing to topics for sensor values
        subscribeTopic(Topics.TEMP_SUB.getTopic());
        subscribeTopic(Topics.HUMIDITY_SUB.getTopic());
        subscribeTopic(Topics.LIGHT_SUB.getTopic());

        //Locate the correct button entities from the xml file
        tempButton = findViewById(R.id.temp_button);
        lightButton = findViewById(R.id.light_button);
        humButton = findViewById(R.id.hum_button);
        profilesButton = findViewById(R.id.profiles_button);
        reminderButton = findViewById(R.id.reminders_button);

        //Initialise listeners for if button is clicked --> call corresponding method
        tempButton.setOnClickListener(view -> openTemperatureView());
        lightButton.setOnClickListener(view -> openLightView());
        humButton.setOnClickListener(view -> openHumidityView());
        profilesButton.setOnClickListener(view ->openProfilesView());
        reminderButton.setOnClickListener(view -> openReminderView());

        //Fetch current profile from ProfileActivity and set active profile to publish values.
        ProfileActivity profileActivity = new ProfileActivity();
        SharedPreferences sharedPreferences = getSharedPreferences("profile", MODE_PRIVATE);
        profileList = profileActivity.loadData(sharedPreferences, profileList);
        TextView currentProfile = findViewById(R.id.current_profile);
        String nameOfProfile = findActiveProfile().getProfileName();
        currentProfile.setText(nameOfProfile);
        publishMsg(Topics.TEMP_PUB.getTopic(), String.valueOf(findActiveProfile().getTemperature()));
        publishMsg(Topics.HUMIDITY_PUB.getTopic(), String.valueOf(findActiveProfile().getHumidity()));
        publishMsg(Topics.LIGHT_PUB.getTopic(), String.valueOf(findActiveProfile().getLightLevel()));
    }

    /** Method for finding the current active profile. **/
    protected RoomProfile findActiveProfile(){
        for(RoomProfile profile : profileList){
            if(profile.isActive()){
                return profile;
            }
        }
        return null;
    }

    //Specific behavior for each button that when clicked takes you to corresponding page in the app
    public void openTemperatureView() {
        Intent intentTemp = new Intent(MainActivity.this, TemperatureView.class);
        int currentProfileId = profileList.get(findActiveProfile().getId()).getId();
        intentTemp.putExtra("profileId", currentProfileId);
        startActivity(intentTemp);
    }
    public void openLightView() {
        Intent intentLight = new Intent(MainActivity.this, LightView.class);
        int currentProfileId = profileList.get(findActiveProfile().getId()).getId();
        intentLight.putExtra("profileId", currentProfileId);
        startActivity(intentLight);
    }

    public void openHumidityView() {
        Intent intentHumidity = new Intent(MainActivity.this, HumidityView.class);
        int currentProfileId = profileList.get(findActiveProfile().getId()).getId();
        intentHumidity.putExtra("profileId", currentProfileId);
        startActivity(intentHumidity);
    }

    public void openProfilesView() {
        Intent intentProfiles = new Intent(MainActivity.this, ProfileActivity.class);
        startActivity(intentProfiles);
    }
    public void openReminderView(){
        Intent intentReminder = new Intent(this, ReminderActivity.class);
        startActivity(intentReminder);
    }
    protected void onDestroy() {
        super.onDestroy();
        client.disconnect();
    }

    //method for publishing a message to a topic and showing a message when the method has run
    private void publishMsg(String topic, String message) {
        //Toast.makeText(this, "publishing message: " + message, Toast.LENGTH_LONG).show();
        client.publish(topic, message);

    }

    //method for subscribing to sensor topics, and inserting values into Firebase Realtime database
    private void subscribeTopic(String topic) {
        //Toast.makeText(this, "subscribing to topic: " + topic, Toast.LENGTH_SHORT).show();
        client.subscribe(topic, (topic1, message) -> {
            String payload = new String(message.getPayload());

            //Conditions for handling incoming topic payloads depending on the current subscribed-to topic
            switch (topic1) {
                case "deskBuddy/temperature":
                    temperatureCounter++;
                    System.out.println(temperatureCounter);
                    temperatureSum+= Double.parseDouble(payload);

                    // populate database every second (live)
                    databaseNode = FirebaseDatabase.getInstance().getReference().child("temperature_liveData");
                    addSensorData("temperature_value", Double.parseDouble(payload));

                    // populate database averaging every minute (aggregate)
                    if(temperatureCounter == TEMPERATURE_COUNT_MAX){
                        databaseNode = FirebaseDatabase.getInstance().getReference().child("temperature_aggregateData");
                        addSensorData("temperature_value", temperatureSum/ temperatureCounter);
                        System.out.println("Added averaged temperature value to database.");
                        temperatureCounter = 0;
                        temperatureSum = 0.0;
                    }

                    // update textview in live dashboard of main activity
                    payload = payload + " \u00B0C";
                    tempView.setText(payload);
                    break;

                case "deskBuddy/humidity":
                    humidityCounter++;
                    humiditySum+=Double.parseDouble(payload);

                    // populate database every second (live)
                    databaseNode = FirebaseDatabase.getInstance().getReference().child("humidity_liveData");
                    addSensorData("humidity_value", Double.parseDouble(payload));

                    // populate database averaging every minute (aggregate)
                    if (humidityCounter == HUMIDITY_COUNT_MAX){
                    databaseNode = FirebaseDatabase.getInstance().getReference().child("humidity_aggregateData");
                    addSensorData("humidity_value", humiditySum/humidityCounter);
                    humidityCounter = 0;
                    humiditySum = 0.0;
                    }

                    // update textview in live dashboard of main activity
                    payload = payload + " %";
                    humView.setText(payload);
                    break;

                case "deskBuddy/light":
                    lightCounter++;
                    lightSum+=Double.parseDouble(payload);

                    // populate database every second (live)
                    databaseNode = FirebaseDatabase.getInstance().getReference().child("light_liveData");
                    addSensorData("light_value", Double.parseDouble(payload));

                    // populate database averaging every minute (aggregate)
                    if (lightCounter == LIGHT_COUNT_MAX){
                        databaseNode = FirebaseDatabase.getInstance().getReference().child("light_aggregateData");
                        addSensorData("light_value", lightSum / lightCounter);
                        lightCounter = 0;
                        lightSum = 0.0;

                    }
                    // update textview in live dashboard of main activity
                    payload = payload + " lx";
                    lightView.setText(payload);
                    break;
            }
        });
    }
    // INSERT operation to Firebase Realtime DB
    private void addSensorData(String pathString, double sensorValue) {
        String key = databaseNode.push().getKey(); // Generate a new unique key
        String timeStamp = String.valueOf(System.currentTimeMillis()); // Generate UNIX timestamp
        databaseNode.child(key).child("timestamp").setValue(timeStamp); //insert child timestamp
        databaseNode.child(key).child(pathString).setValue(sensorValue); //insert child sensor_value

        // Save the current profile in the database with the sensor readings.
        int currentProfileId = profileList.get(findActiveProfile().getId()).getId();
        databaseNode.child(key).child("profile").setValue(currentProfileId);

    }
}