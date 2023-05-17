package com.example.deskbuddyapp;

import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.SwitchCompat;


import android.annotation.SuppressLint;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;

import android.widget.TextView;
import android.widget.Toast;

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
    private SwitchCompat switchButton;
    private ArrayList<RoomProfile> profileList;
    private TextView tempView;
    private TextView lightView;
    private TextView humView;
    DatabaseReference databaseNode;     //Firebase database
    

    //method for creating and starting the app
    @SuppressLint("MissingInflatedId")
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

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
        switchButton = findViewById(R.id.switch_button);

        //Initialise listeners for if button is clicked --> call corresponding method
        tempButton.setOnClickListener(view -> openTemperatureView());
        lightButton.setOnClickListener(view -> openLightView());
        humButton.setOnClickListener(view -> openHumidityView());
        profilesButton.setOnClickListener(view ->openProfilesView());
        switchButton.setOnCheckedChangeListener((buttonView, isChecked) -> handleSwitchStateChange(isChecked));

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
        startActivity(intentLight);
    }

    public void openHumidityView() {
        Intent intentHumidity = new Intent(MainActivity.this, HumidityView.class);
        startActivity(intentHumidity);
    }

    public void openProfilesView() {
        Intent intentProfiles = new Intent(MainActivity.this, ProfileActivity.class);
        startActivity(intentProfiles);
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
                    databaseNode = FirebaseDatabase.getInstance().getReference().child("temperature_data");
                    addSensorData("temperature_value", Double.parseDouble(payload));
                    payload = payload + " \u00B0C";
                    tempView.setText(payload);
                    break;
                case "deskBuddy/humidity":
                    databaseNode = FirebaseDatabase.getInstance().getReference().child("humidity_data");
                    addSensorData("humidity_value", Double.parseDouble(payload));
                    payload = payload + " %";
                    humView.setText(payload);
                    break;
                case "deskBuddy/light":
                    databaseNode = FirebaseDatabase.getInstance().getReference().child("light_data");
                    addSensorData("light_value", Double.parseDouble(payload));
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

    //publishes message to Wio terminal depending on if silent mode is on/off, to set the timing interval of notifications received to on/off
    public void handleSwitchStateChange(boolean isChecked) {
        // Handle the switch button changes to publish message to Wio
        if (isChecked) {
            // Switch is ON
            publishMsg(Topics.TIMING_PUB.getTopic(), "0");
        } else {
            // Switch is OFF
            publishMsg(Topics.TIMING_PUB.getTopic(), "7");

        }
    }
}