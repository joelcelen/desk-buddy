package com.example.deskbuddyapp;


/** Class for saving user preferences. **/

public class RoomProfile {

    private int id;
    private String profileName;
    private double temperature;
    private double humidity;
    private double lightLevel;
    private boolean isActive;

    // Constructor
    public RoomProfile(int id, String profileName, double temperature, double humidity, double lightLevel){
        this.id = id;
        this.profileName = profileName;
        this.temperature = temperature;
        this.humidity = humidity;
        this.lightLevel = lightLevel;
        this.isActive = false;
    }

    // Getters and setters.
    public String getProfileName() {
        return profileName;
    }
    public Integer getId() {
        return id;
    }
    public double getTemperature() {return temperature;}
    public double getHumidity() {
        return humidity;
    }
    public double getLightLevel() {
        return lightLevel;
    }
    public boolean isActive() {return isActive;}
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
    public void setActive(boolean active) {isActive = active;}
}
