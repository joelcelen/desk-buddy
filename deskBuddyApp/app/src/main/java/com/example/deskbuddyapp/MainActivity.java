package com.example.deskbuddyapp;

import androidx.appcompat.app.AppCompatActivity;


import android.annotation.SuppressLint;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;

import android.widget.TextView;
import android.widget.Toast;


public class MainActivity extends AppCompatActivity {
    private MqttHandler client;

    //Initialize buttons on homescreen
    private Button tempButton;
    private Button lightButton;
    private Button humButton;
    private Button profilesButton;


    //method for creating and starting the app
    @SuppressLint("MissingInflatedId")
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        //gets the broker info from a file that is in the .gitignore,
        //connects based on the info in the txt file with the necessary information to connect to secure broker

        client = MqttHandler.getInstance();
        client = MqttHandler.getInstance(); //gets singleton instance
        client.connect();


        //Call methods for subscribing to topics for sensor values
        subscribeTopic(Topics.TEMP_SUB.getTopic());
        subscribeTopic(Topics.HUMIDITY_SUB.getTopic());
        subscribeTopic(Topics.LIGHT_SUB.getTopic());

        //publishMsg(TOPIC, "TCP_GREEN");

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


        Profile profile = new Profile();
        TextView currentProfile = findViewById(R.id.current_profile);
        String nameOfProfile = profile.getProfileName();
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

    //method for subscribing to topic and showing published messages to the topic the app is subscribing to
    private void subscribeTopic(String topic) {
        //Toast.makeText(this, "subscribing to topic: " + topic, Toast.LENGTH_SHORT).show();
        client.subscribe(topic, (topic1, message) -> {
            String payload = new String(message.getPayload());

            //Conditions for handling incoming topic payloads depending on the current subscribed-to topic
            switch (topic1) {
                case "deskBuddy/temperature":
                    TextView tempView = findViewById(R.id.temp_view);
                    tempView.setText(payload);
                    break;
                case "deskBuddy/light":
                    TextView lightView = findViewById(R.id.light_view);
                    lightView.setText(payload);
                    break;
                case "deskBuddy/humidity":
                    TextView humView = findViewById(R.id.hum_view);
                    humView.setText(payload);
                    break;
            }
        });
    }
}