package com.example.deskbuddyapp;

import androidx.appcompat.app.AppCompatActivity;


import android.annotation.SuppressLint;
import android.content.Intent;
import android.os.Bundle;
import android.widget.Button;

import android.widget.TextView;
import android.widget.Toast;

import com.google.firebase.database.DatabaseReference;
import com.google.firebase.database.FirebaseDatabase;


public class MainActivity extends AppCompatActivity {
    private MqttHandler client;

    //Initialize buttons on homescreen
    private Button tempButton;
    private Button lightButton;
    private Button humButton;
    private Button profilesButton;

    private RoomProfile roomProfile;

    private TextView currentProfile;

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

        //Initialise listeners for if button is clicked --> call corresponding method
        tempButton.setOnClickListener(view -> openTemperatureView());
        lightButton.setOnClickListener(view -> openLightView());
        humButton.setOnClickListener(view -> openHumidityView());
        profilesButton.setOnClickListener(view ->openProfilesView());

        //Current room profile
        RoomProfile roomProfile = new RoomProfile();
        TextView currentProfile = findViewById(R.id.current_profile);
        String nameOfProfile = roomProfile.getProfileName();
        currentProfile.setText(nameOfProfile);
    }

    //Specific behavior for each button that when clicked takes you to corresponding page in the app
    public void openTemperatureView() {
        Intent intentTemp = new Intent(this, TemperatureView.class);
        startActivity(intentTemp);
    }
    public void openLightView() {
        Intent intentLight = new Intent(this, LightView.class);
        startActivity(intentLight);
    }

    public void openHumidityView() {
        Intent intentHumidity = new Intent(this, HumidityView.class);
        startActivity(intentHumidity);
    }

    public void openProfilesView() {
        Intent intentProfiles = new Intent(this, ProfileActivity.class);
        startActivity(intentProfiles);
    }

    protected void onDestroy() {
        super.onDestroy();
        client.disconnect();
    }

    //method for publishing a message to a topic and showing a message when the method has run
    private void publishMsg(String topic, String message) {
        Toast.makeText(this, "publishing message: " + message, Toast.LENGTH_LONG).show();
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
    }
}