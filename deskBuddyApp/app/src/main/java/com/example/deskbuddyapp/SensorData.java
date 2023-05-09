package com.example.deskbuddyapp;

 //Class to define environmental data object structure



public class SensorData {

    int profileId;
    long timeStamp; //variable to save Current timestamp in String
    double tempValue; // variable to save temprature value

    //Constructor with specified values
    public SensorData(int profileId, long timeStamp, double tempValue) {

        this.profileId = 1; // temporary data
        this.timeStamp = timeStamp;
        this.tempValue = tempValue;

    }

    //Getters

    public int getProfileId() {
        return profileId;
    }


    public long getTimeStamp() {
        return timeStamp;
    }


    public double getTempValue() {
        return tempValue;
    }


}
