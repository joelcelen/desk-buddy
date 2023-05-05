package com.example.deskbuddyapp;



import java.io.InputStream;
import java.util.Scanner;

public class MqttReadings {
    private static final String TOPIC = "deskBuddy/temperature";
    private static final String CLIENT_ID = "androidDeskBuddy";
    private String brokerUrl;
    private String username;
    private String password;
    private InputStream inputStream;
    private Scanner scanBrokerInfo;
    private MqttHandler client;
}


