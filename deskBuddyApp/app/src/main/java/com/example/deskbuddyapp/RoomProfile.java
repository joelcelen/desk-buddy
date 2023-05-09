package com.example.deskbuddyapp;


/** Class for saving user preferences. **/

public class RoomProfile {

    private String profileName;
    private int id;
    private double temperature;
    private double humidity;
    private double lightLevel;

    // Constructor with specific values
    public RoomProfile(String profileName, double temperature, double humidity, double lightLevel, int id){
        this.profileName = profileName;
        this.temperature = temperature;
        this.humidity = humidity;
        this.lightLevel = lightLevel;
        this.id = id;
    }

    // Constructor with default values.
    public RoomProfile(){
        this.profileName = "Alfons";
        this.temperature = 23.0;
        this.humidity = 40.0;
        this.lightLevel = 700;
    }

    // Getters and setters.
    public String getProfileName() {
        return profileName;
    }
    public Integer getId() {
        return id;
    }

    public double getTemperature() {
        return temperature;
    }

    public double getHumidity() {
        return humidity;
    }

    public double getLightLevel() {
        return lightLevel;
    }

    public void setProfileName(String profileName) {
        this.profileName = profileName;
    }

    public void setTemperature(double temperature) {
        this.temperature = temperature;
    }

    public void setHumidity(double humidity) {
        this.humidity = humidity;
    }

    public void setLightLevel(double lightLevel) {
        this.lightLevel = lightLevel;
    }
    public void setId(Integer ID) {
        this.id = ID;
    }
}
