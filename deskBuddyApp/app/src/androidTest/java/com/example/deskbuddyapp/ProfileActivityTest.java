package com.example.deskbuddyapp;

import static androidx.test.espresso.action.ViewActions.*;
import static androidx.test.espresso.matcher.ViewMatchers.withId;
import static org.junit.Assert.assertTrue;
import android.widget.Button;
import androidx.test.core.app.ActivityScenario;
import androidx.test.espresso.Espresso;
import androidx.test.espresso.ViewInteraction;
import androidx.test.espresso.assertion.ViewAssertions;
import androidx.test.espresso.matcher.ViewMatchers;
import androidx.test.ext.junit.runners.AndroidJUnit4;
import org.junit.Test;
import org.junit.runner.RunWith;

import java.util.ArrayList;


@RunWith(AndroidJUnit4.class)
public class ProfileActivityTest {

    private ArrayList<RoomProfile> profileList;
    private ArrayList<Button> buttonList;
    private Button activeButton;
    private Button inactiveButton;
    private RoomProfile activeProfile;
    private RoomProfile inactiveProfile;

    @Test
    public void testActiveProfile(){
        ActivityScenario<ProfileActivity> scenario = ActivityScenario.launch(ProfileActivity.class);

        // Get information from ProfileActivity class.
        scenario.onActivity(activity -> {
            profileList = activity.getProfileList();
            buttonList = activity.getButtonList();
            activeProfile = activity.findActiveProfile();
        });

        // Assert that the profile found is active.
        assertTrue(activeProfile.isActive());

        scenario.close();
    }
    @Test
    public void testSliderChanges() {
        ActivityScenario<ProfileActivity> scenario = ActivityScenario.launch(ProfileActivity.class);

        // Get information from ProfileActivity class.
        scenario.onActivity(activity -> {
            profileList = activity.getProfileList();
            buttonList = activity.getButtonList();
            activeButton = buttonList.get(activity.findActiveProfile().getId());
        });

        // Preform the sequence of clicks.
        Espresso.onView(withId(R.id.btnSettings))
                    .perform(click());

        Espresso.onView(withId(R.id.editName))
                .perform(clearText());

        Espresso.onView(withId(R.id.editName))
                .perform(typeText("Test Case")).perform(closeSoftKeyboard());

        // Get a reference to the temperature slider and set it to max value
        ViewInteraction tempSlider = Espresso.onView(withId(R.id.sldTemp));
        tempSlider.perform(click());
        tempSlider.perform(swipeRight());

        // Get a reference to the humidity slider and set it to max value
        ViewInteraction humSlider = Espresso.onView(withId(R.id.sldHum));
        humSlider.perform(click());
        humSlider.perform(swipeRight());

        // Get a reference to the lightLevel slider and set it to max value
        ViewInteraction lightSlider = Espresso.onView(withId(R.id.sldLight));
        lightSlider.perform(click());
        lightSlider.perform(swipeRight());

        // Goes back to profile page
        Espresso.onView(withId(R.id.btnSave))
                .perform(click());

        // Asserts that the right changes have been made to the button text
        Espresso.onView(withId(activeButton.getId()))
                .check(ViewAssertions.matches(ViewMatchers.withText("TEST CASE\n TEMP: 30.0\n HUM: 60.0\n LUX: 1000.0")));

        scenario.close();
    }

    @Test
    public void assertProfileChange(){

        ActivityScenario<ProfileActivity> scenario = ActivityScenario.launch(ProfileActivity.class);

        // Get information from ProfileActivity class.
        scenario.onActivity(activity -> {
            profileList = activity.getProfileList();
            buttonList = activity.getButtonList();

            // Find the last instance of an inactive profile;
            for(RoomProfile profile : profileList){
                if(!(profile.isActive())){
                    inactiveButton = buttonList.get(profile.getId());
                    inactiveProfile = profile;
                }
            }
        });

        Espresso.onView(withId(inactiveButton.getId())).perform(click());

        // Assert that the profile is now set to active.
        assertTrue(inactiveProfile.isActive());

        scenario.close();

    }
}
