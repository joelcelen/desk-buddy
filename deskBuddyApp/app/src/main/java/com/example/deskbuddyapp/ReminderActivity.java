package com.example.deskbuddyapp;

import androidx.appcompat.app.AppCompatActivity;

import com.google.android.material.slider.Slider;

import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.widget.Button;
import android.widget.EditText;

public class ReminderActivity extends AppCompatActivity {

  private MqttHandler client;
  private Slider sliderRemind;
  private Slider sliderStandUp;
  private Slider sliderMotivation;
  private Button saveButtonReminder;
  private Button saveButtonMotivation;
  private Button saveButtonStandUp;
  private Button resetButtonReminder;
  private Button resetButtonStandUp;
  private Button resetButtonMotivation;
  private Button homeButton;
  private EditText reminderText;
  String reminderMsg = "";
    final private int MIN_TO_MILLI_SEC = 60_000;
    final private int REMINDER_DEFAULT = 360 * MIN_TO_MILLI_SEC;
    final private int MOTIVATION_DEFAULT = 30 * MIN_TO_MILLI_SEC;
    final private int STAND_UP_DEFAULT = 60 * MIN_TO_MILLI_SEC;
    private float reminderInterval = 360;
    private float motivationInterval = 30;
    private float standUpInterval = 60;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_reminder);
        client = MqttHandler.getInstance();

        // Initiate background animation.
        BackgroundAnimator animator = new BackgroundAnimator();
        animator.animateBackground(findViewById(R.id.activity_reminder));

        //set all variables to their corresponding elements in the xml file
        sliderRemind = findViewById(R.id.slider_rem);
        sliderStandUp = findViewById(R.id.stand_up_slider);
        sliderMotivation = findViewById(R.id.motivation_slider);

        reminderText = findViewById(R.id.editReminderMsg);

        saveButtonReminder = findViewById(R.id.save_btn_rem);
        saveButtonMotivation = findViewById(R.id.save_btn_motivation);
        saveButtonStandUp = findViewById(R.id.save_btn_stand_up);
        resetButtonReminder = findViewById(R.id.reset_button_rem);
        resetButtonStandUp = findViewById(R.id.reset_btn_stand_up);
        resetButtonMotivation = findViewById(R.id.reset_btn_motivation);
        homeButton = findViewById(R.id.back_button);

        //saves initial default values and loads them
        saveData();
        loadData();

        //sets initial values of the sliders to what was saved and loaded previously
            sliderRemind.setValue(reminderInterval);
            sliderMotivation.setValue(motivationInterval);
            sliderStandUp.setValue(standUpInterval);

        //if user clicks any of the save buttons for the sliders, save data and publish new time interval to Wio
        saveButtonReminder.setOnClickListener(v -> {
            reminderMsg = reminderText.getText().toString();
            reminderInterval = sliderRemind.getValue();
            handleSaveReminder(reminderInterval, reminderMsg);
            saveReminderData(reminderInterval);
        });

        saveButtonMotivation.setOnClickListener(v -> {
            motivationInterval = sliderMotivation.getValue();
            handleSaveMotivation(motivationInterval);
            saveMotivationData(motivationInterval);
        });

        saveButtonStandUp.setOnClickListener(v -> {
            standUpInterval = sliderStandUp.getValue();
            handleSaveStandUp(standUpInterval);
            saveStandUpData(standUpInterval);
        });

        //if user clicks on the reset button for any slider, default value is saved and publish new time interval to Wio
        resetButtonReminder.setOnClickListener(v -> {
            reminderMsg = reminderText.getText().toString();
            sliderRemind.setValue((float) REMINDER_DEFAULT / MIN_TO_MILLI_SEC);
            handleReminderReset();
            saveReminderData((float) REMINDER_DEFAULT / MIN_TO_MILLI_SEC);
        });

        resetButtonMotivation.setOnClickListener(v -> {
            sliderMotivation.setValue((float) MOTIVATION_DEFAULT / MIN_TO_MILLI_SEC);
            handleMotivationReset();
            saveMotivationData((float) MOTIVATION_DEFAULT / MIN_TO_MILLI_SEC);
        });

        resetButtonStandUp.setOnClickListener(v -> {
            sliderStandUp.setValue((float) STAND_UP_DEFAULT / MIN_TO_MILLI_SEC);
            handleStandUpReset();
            saveStandUpData((float) STAND_UP_DEFAULT / MIN_TO_MILLI_SEC);
        });

        homeButton.setOnClickListener(v -> {
            Intent home = new Intent(ReminderActivity.this, MainActivity.class);
            startActivity(home);
        });
    }

    //method for publishing new time interval and message for the reminders to Wio if user presses save button for the slider
    public void handleSaveReminder(float interval, String message) {
        // send one time notification with new message
        String reminder = "" + message;
        String topic = Topics.NOTIFICATION_PUB.getTopic();
        client.publish(topic, reminder);
        //sends new interval, the 3 in the String message is for determining which type of notification to change in the Wio
        String timing = "3" + (int)(interval * MIN_TO_MILLI_SEC);
        topic = Topics.TIMING_PUB.getTopic();
        client.publish(topic, timing);
    }

    //method for publishing new time interval for the motivational messages to Wio if user presses save button for the slider
    public void handleSaveMotivation(float interval) {
        //sends new interval, the 2 in the String message is for determining which type of notification to change in the Wio
        String timing = "2" + (int)(interval * MIN_TO_MILLI_SEC);
        String topic = Topics.TIMING_PUB.getTopic();
        client.publish(topic, timing);
    }
    //method for publishing new time interval for the stand-ups to Wio if user presses save button for the slider
    public void handleSaveStandUp(float interval) {
        //sends new interval, the 1 in the String message is for determining which type of notification to change in the Wio
        String message = "1" + (int)(interval * MIN_TO_MILLI_SEC);
        String topic = Topics.TIMING_PUB.getTopic();
        client.publish(topic, message);
    }
    //Method for setting the correct value to publish to the Wio if the reset button for reminders is pressed
    public void handleReminderReset() {
        String reminder = "";
        String topic = Topics.NOTIFICATION_PUB.getTopic();
        client.publish(topic, reminder);
        //sends new interval, the 3 in the String message is for determining which type of notification to change in the Wio
        String timing = "3" + REMINDER_DEFAULT;
        topic = Topics.TIMING_PUB.getTopic();
        client.publish(topic, timing);
    }
    //Method for setting the correct value to publish to the Wio if the reset button for motivation is pressed
    public void handleMotivationReset() {
        String timing = "2" + MOTIVATION_DEFAULT;
        String topic = Topics.TIMING_PUB.getTopic();
        client.publish(topic, timing);
    }
    //Method for setting the correct value to publish to the Wio if the reset button for stand-ups is pressed
    public void handleStandUpReset() {
        String timing = "1" + STAND_UP_DEFAULT;
        String topic = Topics.TIMING_PUB.getTopic();
        client.publish(topic, timing);
    }
    //save initial default values for each slider
    private void saveData(){
        SharedPreferences sharedPreferences = getSharedPreferences("reminders", MODE_PRIVATE);
        SharedPreferences.Editor editor = sharedPreferences.edit();
        editor.putInt("reminder_default", REMINDER_DEFAULT);
        editor.putInt("motivation_default", MOTIVATION_DEFAULT);
        editor.putInt("stand_up_default", STAND_UP_DEFAULT);
        editor.apply();
    }
    //method for saving user defined time interval for reminders
    private void saveReminderData(float interval){
        SharedPreferences sharedPreferences = getSharedPreferences("reminders", MODE_PRIVATE);
        SharedPreferences.Editor editor = sharedPreferences.edit();
        editor.putFloat("reminder_saved", interval);
        editor.apply();
    }
    //method for saving user defined time interval for motivational messages
    private void saveMotivationData(float interval){
        SharedPreferences sharedPreferences = getSharedPreferences("reminders", MODE_PRIVATE);
        SharedPreferences.Editor editor = sharedPreferences.edit();
        editor.putFloat("motivation_saved", interval);
        editor.apply();
    }
    //method for saving user defined time interval for stand-up messages
    private void saveStandUpData(float interval){
        SharedPreferences sharedPreferences = getSharedPreferences("reminders", MODE_PRIVATE);
        SharedPreferences.Editor editor = sharedPreferences.edit();
        editor.putFloat("stand_up_saved", interval);
        editor.apply();
    }
    //method for loading user defined preferences for time intervals to publish Wio messages,
    // sets to default if no data has been saved previously
    private void loadData() {
        SharedPreferences sharedPreferences = getSharedPreferences("reminders", MODE_PRIVATE);
        reminderInterval = sharedPreferences.getFloat("reminder_saved", (float)STAND_UP_DEFAULT / MIN_TO_MILLI_SEC);
        motivationInterval = sharedPreferences.getFloat("motivation_saved", (float)MOTIVATION_DEFAULT / MIN_TO_MILLI_SEC);
        standUpInterval = sharedPreferences.getFloat("stand_up_saved", (float)STAND_UP_DEFAULT / MIN_TO_MILLI_SEC);
    }
}