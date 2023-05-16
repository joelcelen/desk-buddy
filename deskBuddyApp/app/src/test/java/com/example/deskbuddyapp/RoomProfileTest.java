package com.example.deskbuddyapp;

import static org.junit.Assert.*;
import org.junit.Test;

public class RoomProfileTest {
    @Test
    public void createCustomProfile(){
        RoomProfile profile = new RoomProfile(9,"Custom", 35.0, 60.0, 1000.0);
        assertEquals("Custom", profile.getProfileName());
        assertEquals(35.0, profile.getTemperature(), 0.1);
        assertEquals(60.0, profile.getHumidity(), 0.1);
        assertEquals(1000.0, profile.getLightLevel(), 0.1);
    }

}