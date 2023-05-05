package com.example.deskbuddyapp;

import androidx.appcompat.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.TextView;
import android.widget.Toast;
import org.eclipse.paho.client.mqttv3.IMqttMessageListener;
import org.eclipse.paho.client.mqttv3.MqttMessage;

public class MainActivity extends AppCompatActivity {
    private static final String TOPIC = "deskBuddy/light";
    private MqttHandler client;

    //method for creating and starting the app
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        client = MqttHandler.getInstance(); //gets singleton instance
        client.connect();
        subscribeTopic(TOPIC);

    }

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
                TextView textView = findViewById(R.id.text_view);
                textView.setText(payload);
            }
        });
    }
}