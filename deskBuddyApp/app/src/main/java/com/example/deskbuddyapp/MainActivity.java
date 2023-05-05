package com.example.deskbuddyapp;

import androidx.appcompat.app.AppCompatActivity;

import android.annotation.SuppressLint;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;
import org.eclipse.paho.client.mqttv3.IMqttMessageListener;
import org.eclipse.paho.client.mqttv3.MqttMessage;

public class MainActivity extends AppCompatActivity {
    private static final String TOPICTEMP = "deskBuddy/temperature";
    private static final String TOPICHUM = "deskBuddy/humidity";
    private static final String TOPICLIGHT = "deskBuddy/light";
    private MqttHandler client;

    /*
    //Initialize buttons on homescreen
    private Button tempButton;
    private Button lightButton;
    private Button humButton;
    private Button profilesButton;
    */
    //method for creating and starting the app
    @SuppressLint("MissingInflatedId")
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        client = new MqttHandler(this);
        client.connect();

        //Call methods for subscribing to topics for sensor values
        subscribeTopic(TOPICTEMP);
        subscribeTopic(TOPICHUM);
        subscribeTopic(TOPICLIGHT);

        //publishMsg(TOPIC, "TCP_GREEN");
        /*
        //Locate the correct button entities from the xml file
        tempButton = (Button) findViewById(R.id.temp_button);
        lightButton = (Button) findViewById(R.id.light_button);
        humButton = (Button) findViewById(R.id.hum_button);
        profilesButton = (Button) findViewById(R.id.profiles_button);


        //Initialise listeners for if button is clicked --> call corresponding method
        tempButton.setOnClickListener(view -> openTemperatureView());
        lightButton.setOnClickListener(view -> openLightView());
        humButton.setOnClickListener(view -> openHumidityView());
        profilesButton.setOnClickListener(view ->openProfilesView());
        */

        RoomProfile roomProfile = new RoomProfile();
        TextView currentProfile = (TextView) findViewById(R.id.current_profile);
        String nameOfProfile = roomProfile.getRoomName();
        currentProfile.setText(nameOfProfile);
    }

    /*
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
    */


    protected void onDestroy() {
        client.disconnect();
        super.onDestroy();
    }

    //method for publishing a message to a topic and showing a message when the method has run
    private void publishMsg(String topic, String message) {
        Toast.makeText(this, "publishing message: " + message, Toast.LENGTH_LONG).show();
        client.publish(topic, message);

    }

    //method for subscribing to topic and showing published messages to the topic the app is subscribing to
    private void subscribeTopic(String topic) {
        Toast.makeText(this, "subscribing to topic: " + topic, Toast.LENGTH_SHORT).show();
        client.subscribe(topic, new IMqttMessageListener() {
            @Override
            public void messageArrived(String topic, MqttMessage message) throws Exception {
                String payload = new String(message.getPayload());
                //TextView textView = findViewById(R.id.text_view);
                //TextView tempView = findViewById(R.id.temp_view);
                //tempView.setText(payload);

                //Conditions for handling incoming topic payloads depending on the current subscribed-to topic
                if(topic.equals("deskBuddy/temperature")){
                    TextView tempView = findViewById(R.id.temp_view);
                    tempView.setText(payload);
                }
                else if(topic.equals("deskBuddy/light")) {
                    TextView lightView = findViewById(R.id.light_view);
                    lightView.setText(payload);
                }
                else if(topic.equals("deskBuddy/humidity")) {
                    TextView humView = findViewById(R.id.hum_view);
                    humView.setText(payload);
                }
            }
        });
    }
}