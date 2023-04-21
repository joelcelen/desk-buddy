package com.example.deskbuddyapp;

import org.eclipse.paho.client.mqttv3.IMqttMessageListener;
import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence;

//Credit to author of this class which we used: https://www.youtube.com/@CoffeeProgrammer
//Link to video: https://www.youtube.com/watch?v=2ucv0IZgT1E&ab_channel=CoffeeProgrammer
public class MqttHandler {

    private MqttClient client;

    //Method for connecting to broker
    public void connect(String brokerUrl, String clientId) {
        try {
            // Set up the persistence layer
            MemoryPersistence persistence = new MemoryPersistence();

            // Initialize the MQTT client
            client = new MqttClient(brokerUrl, clientId, persistence);

            // Set up the connection options
            MqttConnectOptions connectOptions = new MqttConnectOptions();
            connectOptions.setCleanSession(true);

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