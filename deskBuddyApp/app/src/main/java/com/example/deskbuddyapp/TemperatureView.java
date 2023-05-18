package com.example.deskbuddyapp;
import android.content.Intent;
import android.graphics.Color;
import android.graphics.Typeface;
import android.os.Bundle;
import android.view.View;
import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import com.github.mikephil.charting.charts.LineChart;
import com.github.mikephil.charting.components.Legend;
import com.github.mikephil.charting.components.XAxis;
import com.github.mikephil.charting.components.YAxis;
import com.github.mikephil.charting.data.Entry;
import com.github.mikephil.charting.data.LineData;
import com.github.mikephil.charting.data.LineDataSet;
import com.google.firebase.database.DataSnapshot;
import com.google.firebase.database.DatabaseError;
import com.google.firebase.database.DatabaseReference;
import com.google.firebase.database.FirebaseDatabase;
import com.google.firebase.database.ValueEventListener;
import java.util.ArrayList;

//Credits to https://www.youtube.com/watch?v=DD1CxoVONFE&ab_channel=KGPTalkie

public class TemperatureView extends AppCompatActivity {
    private ArrayList<Entry> tempData;
    private LineChart temperatureChart;
    private LineData data;
    private DatabaseReference databaseReference;
    private long timeInterval;
    private int currentProfile;

    private ValueEventListener valueEventListener; // Store the ValueEventListener instance


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_temperature_view);

        // Initiate background animation.
        BackgroundAnimator animator = new BackgroundAnimator();
        animator.animateBackground(findViewById(R.id.temp_view));

        timeInterval = 60000; //1 minute = 60000 milliseconds. This is the default setting(Live reading + 1 minute ago data).
        temperatureChart = findViewById(R.id.tempChart);

        Intent intent = getIntent();
        currentProfile = intent.getIntExtra("profileId",0);

        tempData = new ArrayList<>();

        readDataLive();
    }

    public void readDataLive(){

        databaseReference = FirebaseDatabase.getInstance().getReference("temperature_liveData");
        removeDataListener();
        valueEventListener = new ValueEventListener() {
            @Override
            public void onDataChange(@NonNull DataSnapshot snapshot) {
                tempData.clear(); //
                long startTime = System.currentTimeMillis() - timeInterval;
                for(DataSnapshot dataSnapshot : snapshot.getChildren()){
                    Double value = dataSnapshot.child("temperature_value").getValue(Double.class);
                    String generalTimeStamp = dataSnapshot.child("timestamp").getValue(String.class);
                    Integer profileId = dataSnapshot.child("profile").getValue(Integer.class);

                    if (value != null && generalTimeStamp!= null && profileId != null && profileId == currentProfile){ //Avoid getting empty value from the database
                        long timeStamp = Long.parseLong(generalTimeStamp);
                        if (timeStamp >= startTime){
                            timeStamp = timeStamp -startTime;
                            float time = (float) timeStamp;
                            float entryData = value.floatValue();
                            Entry entry = new Entry(time,entryData);
                            tempData.add(entry);
                        }
                    }
                }
                updateTemperatureChart();
            }
            @Override
            public void onCancelled(@NonNull DatabaseError error) {
                //TODO: Add logic for this method
            }
        };
        databaseReference.addValueEventListener(valueEventListener);
    }

    public void readDataAggregate(){

        databaseReference =  FirebaseDatabase.getInstance().getReference("temperature_aggregateData");
        removeDataListener();
        valueEventListener = new ValueEventListener() {
            @Override
            public void onDataChange(@NonNull DataSnapshot snapshot) {
                tempData.clear(); //
                long startTime = System.currentTimeMillis() - timeInterval;
                for(DataSnapshot dataSnapshot : snapshot.getChildren()){
                    Double value = dataSnapshot.child("temperature_value").getValue(Double.class);
                    String generalTimeStamp = dataSnapshot.child("timestamp").getValue(String.class);
                    Integer profileId = dataSnapshot.child("profile").getValue(Integer.class);

                    if (value != null && generalTimeStamp!= null && profileId != null && profileId == currentProfile){ //Avoid getting empty value from the database
                        long timeStamp = Long.parseLong(generalTimeStamp);
                        if (timeStamp >= startTime){
                            timeStamp = timeStamp -startTime;
                            float time = (float) timeStamp;
                            float entryData = value.floatValue();
                            Entry entry = new Entry(time,entryData);
                            tempData.add(entry);
                        }
                    }
                }
                updateTemperatureChart();
            }
            @Override
            public void onCancelled(@NonNull DatabaseError error) {
                //TODO: Add logic for this method
            }
        };
        databaseReference.addValueEventListener(valueEventListener);
    }


    public void updateTemperatureChart(){
        data = new LineData(getTemperatureDataSet());

        // Notify the LineData that the data has changed
        data.notifyDataChanged();

        temperatureChart.setData(data);
        temperatureChart.setDragEnabled(false);
        temperatureChart.setScaleEnabled(false);
        // Set the X-axis and Y-axis label position to bottom and enable grid lines
        XAxis xAxis = temperatureChart.getXAxis();
        xAxis.setPosition(XAxis.XAxisPosition.BOTTOM);
        xAxis.setDrawGridLines(false);
        xAxis.setTextSize(30);
        xAxis.setDrawLabels(false);
        xAxis.setTextColor(Color.WHITE);
        xAxis.setTypeface(Typeface.DEFAULT_BOLD);
        xAxis.setEnabled(false);

        YAxis yAxis = temperatureChart.getAxisLeft();
        yAxis.setTextSize(30);
        yAxis.setTextColor(Color.WHITE);
        yAxis.setTypeface(Typeface.DEFAULT_BOLD);
        temperatureChart.getAxisRight().setEnabled(false);
        temperatureChart.getDescription().setEnabled(false);

        Legend legend = temperatureChart.getLegend();
        legend.setEnabled(true);
        legend.setTextSize(12);
        legend.setTextColor(Color.WHITE);

        temperatureChart.setData(data);

        // Notify the chart that the data has changed
        temperatureChart.notifyDataSetChanged();
        temperatureChart.invalidate();
    }

    public LineDataSet getTemperatureDataSet(){

        LineDataSet temperatureDataSet = new LineDataSet(tempData,"Temperature (\u00B0C)");
        temperatureDataSet.setColor(Color.WHITE);
        temperatureDataSet.setDrawFilled(true);
        temperatureDataSet.setLineWidth(0f);
        temperatureDataSet.setDrawCircles(true);
        temperatureDataSet.setCircleRadius(1.3f);
        temperatureDataSet.setValueTextColor(Color.WHITE);
        temperatureDataSet.setDrawValues(false);

        return temperatureDataSet;
    }
    // Add a method to remove the event listener
    private void removeDataListener() {
        if (valueEventListener != null) {
            databaseReference.removeEventListener(valueEventListener);
            valueEventListener = null;
        }
    }

    // Override the onDestroy() method to remove the event listener
    @Override
    protected void onDestroy() {
        super.onDestroy();
        removeDataListener();
    }

    public void mainActivity(View view) {
        Intent intent = new Intent(this, MainActivity.class);
        startActivity(intent);
    }
    public void liveGraph(View view){
        timeInterval = 60000; // 1 minute = 60000 milliseconds
        removeDataListener();
        readDataLive();
    }
    public void hourlyGraph(View view){
        timeInterval = 3600000; // 1 hour = 3600000 milliseconds
        removeDataListener();
        readDataAggregate();
    }
    public void dailyGraph(View view){
        timeInterval = 3600000 * 24; // 1 day = 86400000 milliseconds
        removeDataListener();
        readDataAggregate();
    }
    public void weeklyGraph(View view){
        timeInterval= 3600000 * 24 * 7; // 1 week = 604800000 milliseconds
        removeDataListener();
        readDataAggregate();
    }
}