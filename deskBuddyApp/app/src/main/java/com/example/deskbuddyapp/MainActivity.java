package com.example.deskbuddyapp;

import androidx.appcompat.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;
import org.eclipse.paho.client.mqttv3.IMqttMessageListener;
import org.eclipse.paho.client.mqttv3.MqttMessage;
import java.io.InputStream;
import java.util.Scanner;

public class MainActivity extends AppCompatActivity implements AdapterView.OnItemSelectedListener {
    private static final String TOPIC = "deskBuddy/app/setLight";
    private static final String CLIENT_ID = "androidDeskBuddy";
    private String brokerUrl;
    private String username;
    private String password;
    private InputStream inputStream;
    private Scanner scanBrokerInfo;
    private MqttHandler client;

    //method for creating and starting the app
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        //Inflate our layout with setContentView and turn all views in the layout into java objects.
        setContentView(R.layout.activity_main);

        Spinner settingsSpinner = findViewById(R.id.spinner_settings_profile);
        ArrayAdapter<CharSequence>  adapter = ArrayAdapter.createFromResource(this, R.array.profile_settings, android.R.layout.simple_spinner_item);
        adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        settingsSpinner.setAdapter(adapter);
        settingsSpinner.setOnItemSelectedListener(this);




        //gets the broker info from a file that is in the .gitignore,
        //connects based on the info in the txt file with the necessary information to connect to secure broker
        inputStream = getResources().openRawResource(R.raw.brokerinfo);
        {
            try {
                scanBrokerInfo = new Scanner(inputStream);
                while (scanBrokerInfo.hasNextLine()) {
                    brokerUrl = scanBrokerInfo.next();
                    username = scanBrokerInfo.next();
                    password = scanBrokerInfo.next();
                }
                inputStream.close();
            } catch (Exception e) {
                throw new RuntimeException(e);
            }
        }

        client = new MqttHandler();
        client.connect(brokerUrl, CLIENT_ID, username, password);
        //subscribeTopic(TOPIC);
        publishMsg(TOPIC, "TCP_GREEN");
        scanBrokerInfo.close();

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

    @Override
    public void onItemSelected(AdapterView<?> adapterView, View view, int i, long l) {
        String text = adapterView.getItemAtPosition(i).toString();
        Toast.makeText(adapterView.getContext(), text, Toast.LENGTH_SHORT).show();
    }

    @Override
    public void onNothingSelected(AdapterView<?> adapterView) {

    }
}