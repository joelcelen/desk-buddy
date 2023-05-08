package com.example.deskbuddyapp;

//enums for the colors that will be published to the Wio terminal from the app
public enum WioDisplayColors {
    RED("TFT_RED"),
    ORANGE("TFT_ORANGE"),
    GREEN("TFT_GREEN");
    private final String color;
    WioDisplayColors(String color) {
        this.color = color;
    }
    public String getColor() {
        return color;
    }
}
