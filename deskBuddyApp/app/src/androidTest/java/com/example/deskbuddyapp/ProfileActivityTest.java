package com.example.deskbuddyapp;

import static androidx.test.espresso.action.ViewActions.*;
import static androidx.test.espresso.matcher.ViewMatchers.withId;

import androidx.test.core.app.ActivityScenario;
import androidx.test.espresso.Espresso;
import androidx.test.espresso.ViewInteraction;
import androidx.test.espresso.assertion.ViewAssertions;
import androidx.test.espresso.matcher.ViewMatchers;
import androidx.test.ext.junit.runners.AndroidJUnit4;
import org.junit.Test;
import org.junit.runner.RunWith;


@RunWith(AndroidJUnit4.class)
public class ProfileActivityTest {

    @Test
    public void testSliderChanges() {
        ActivityScenario<ProfileActivity> scenario = ActivityScenario.launch(ProfileActivity.class);

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
        Espresso.onView(withId(R.id.btnProfile1))
                .check(ViewAssertions.matches(ViewMatchers.withText("TEST CASE\n TEMP: 30.0\n HUM: 60.0\n LUX: 1000.0")));

        scenario.close();
    }
}
