package com.example.deskbuddyapp;

import android.content.Context;

import org.eclipse.paho.client.mqttv3.IMqttMessageListener;
import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence;

import java.io.InputStream;
import java.util.Scanner;

//Credit to author of this class which we used: https://www.youtube.com/@CoffeeProgrammer
//Link to video: https://www.youtube.com/watch?v=2ucv0IZgT1E&ab_channel=CoffeeProgrammer
public class MqttHandler {

    private static MqttHandler singleInstance;
    private MqttClient client;

    private final String CLIENT_ID = "androidDeskBuddy";


    private String brokerUrl;
    private String username;
    private String password;
    private InputStream inputStream;
    private Scanner scanBrokerInfo;



    // Credit to this Stack overflow entry and user: LuckyLuke in regards to getting resources from a raw folder in a class that is not an activity class
    //https://stackoverflow.com/questions/7666589/using-getresources-in-non-activity-class


    // Credit to this article: https://www.geeksforgeeks.org/singleton-class-java/
    // for info on how to implement singleton pattern
    private MqttHandler(){}

    //creates instance of Mqtthandler if one is not already existing,
    // otherwise returns the instance already created

    public static synchronized MqttHandler getInstance() {
        if (singleInstance == null) {
            singleInstance= new MqttHandler();
        }
        return singleInstance;
    }

    public InputStream getResource(){
        return ContextHelper.getContext().getResources().openRawResource(R.raw.brokerinfo);
    }
    //Method for connecting to broker
    public void connect() {
        //gets the broker info from a file that is in the .gitignore,
        //connects based on the info in the txt file with the necessary information to connect to secure broker
        inputStream = getResource();
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
        scanBrokerInfo.close();
        try {
            // Set up the persistence layer
            MemoryPersistence persistence = new MemoryPersistence();

            // Initialize the MQTT client
            client = new MqttClient(brokerUrl, CLIENT_ID, persistence);

            // Set up the connection options
            MqttConnectOptions connectOptions = new MqttConnectOptions();
            connectOptions.setCleanSession(true);
            connectOptions.setUserName(username);
            connectOptions.setPassword(password.toCharArray());

            // Connect to the broker
            client.connect(connectOptions);
        } catch (MqttException e) {
            e.printStackTrace();
        }

    }

    //method for disconnecting
    public void disconnect() {
        try {
            client.disconnect();
        } catch (MqttException e) {
            e.printStackTrace();
        }
    }

    //method for publishing message
    public void publish(String topic, String message) {
        try {
            MqttMessage mqttMessage = new MqttMessage(message.getBytes());
            client.publish(topic, mqttMessage);
        } catch (MqttException e) {
            e.printStackTrace();
        }
    }

    //method for subscribing to a topic
    public void subscribe(String topic, IMqttMessageListener listener) {
        try {
            client.subscribe(topic, listener);
        } catch (MqttException e) {
            e.printStackTrace();
        }
    }
}