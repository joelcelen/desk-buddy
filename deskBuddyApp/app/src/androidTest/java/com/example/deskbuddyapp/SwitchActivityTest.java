package com.example.deskbuddyapp;

import static androidx.test.espresso.Espresso.onView;
import static androidx.test.espresso.action.ViewActions.*;
import static androidx.test.espresso.intent.Intents.intended;
import static androidx.test.espresso.intent.matcher.IntentMatchers.hasComponent;
import static androidx.test.espresso.matcher.ViewMatchers.withId;
import androidx.test.core.app.ActivityScenario;
import androidx.test.espresso.intent.Intents;
import androidx.test.ext.junit.runners.AndroidJUnit4;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;

/** This tester class attempts to change from the main activity to a specified activity
 * and then return to the main page. **/

@RunWith(AndroidJUnit4.class)
public class SwitchActivityTest {

    private void switchToActivity(int buttonID, String className){
        ActivityScenario<MainActivity> scenario = ActivityScenario.launch(MainActivity.class);

        // Perform a click to go to the intended activity
        onView(withId(buttonID)).perform(click());

        // Assert that the intended activity is launched
        intended(hasComponent(className));

        // Simulate pressing the back button
        pressBack();

        // Assert that the initial activity is displayed again
        intended(hasComponent(MainActivity.class.getName()));

        // Close the scenario
        scenario.close();
    }

    @Before
    // Get the necessary methods for handling intents in Espresso testing framework.
    public void setup() {
        Intents.init();
    }

    @After
    public void teardown() {
        Intents.release();
    }

    @Test
    public void switchToProfileActivity(){
        switchToActivity(R.id.profiles_button, ProfileActivity.class.getName());
    }

    @Test
    public void switchToTempGraph(){
        switchToActivity(R.id.temp_button, TemperatureView.class.getName());
    }

    @Test
    public void switchToHumGraph(){
        switchToActivity(R.id.hum_button, HumidityView.class.getName());
    }

    @Test
    public void switchToLightGraph(){
        switchToActivity(R.id.light_button, LightView.class.getName());
    }

    /* Uncomment as soon as the ReminderActivity is in the program.
    @Test
    public void switchToReminderActivity(){
        switchToActivity(R.id.reminders_button, ReminderActivity.class.getName());
    }*/
}