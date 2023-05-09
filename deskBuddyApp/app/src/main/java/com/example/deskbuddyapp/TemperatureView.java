package com.example.deskbuddyapp;



import android.content.Intent;
import android.graphics.Color;
import android.graphics.Typeface;
import android.os.Bundle;
import android.view.View;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

import com.github.mikephil.charting.charts.LineChart;
import com.github.mikephil.charting.components.Legend;
import com.github.mikephil.charting.components.XAxis;
import com.github.mikephil.charting.components.YAxis;
import com.github.mikephil.charting.data.Entry;
import com.github.mikephil.charting.data.LineData;
import com.github.mikephil.charting.data.LineDataSet;
import com.github.mikephil.charting.interfaces.datasets.ILineDataSet;

import org.eclipse.paho.client.mqttv3.IMqttMessageListener;
import org.eclipse.paho.client.mqttv3.MqttMessage;


import java.time.LocalTime;
import java.util.ArrayList;

//Credits to https://www.youtube.com/watch?v=DD1CxoVONFE&ab_channel=KGPTalkie

public class TemperatureView extends AppCompatActivity {
    private MqttHandler client;
    static ArrayList<Entry> tempData = new ArrayList<>();
    static LineChart temperatureChart;
    static LineDataSet temperatureDataSet = new LineDataSet(tempData,"Temperature Data Set");
    static ArrayList<ILineDataSet> dataSets = new ArrayList<>();
    static LineData data = new LineData(dataSets);

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_temp_view);
        temperatureChart = findViewById(R.id.tempChart);
        client = MqttHandler.getInstance();
        subscribeTopic(Topics.TEMP_SUB.getTopic());
        generateGraph();
    }

    public void generateGraph(){

        temperatureChart.refreshDrawableState();
        temperatureChart.setDragEnabled(true);
        temperatureChart.setScaleEnabled(false);


        temperatureDataSet.setColor(Color.BLUE);
        temperatureDataSet.setLineWidth(2f);
        temperatureDataSet.setDrawCircles(true);
        temperatureDataSet.setCircleColor(Color.BLUE);
        temperatureDataSet.setCircleRadius(5f);


        dataSets.add(temperatureDataSet);


        XAxis xAxis = temperatureChart.getXAxis();
        xAxis.setTextSize(30);
        xAxis.setDrawLabels(false);
        xAxis.setTextColor(Color.BLACK);
        xAxis.setTypeface(Typeface.DEFAULT_BOLD);
        xAxis.setPosition(XAxis.XAxisPosition.BOTTOM);

        xAxis.setEnabled(false);

        YAxis yAxis = temperatureChart.getAxisLeft();
        yAxis.setTextSize(30);
        yAxis.setTextColor(Color.BLACK);
        yAxis.setTypeface(Typeface.DEFAULT_BOLD);
        temperatureChart.getAxisRight().setEnabled(false);
        temperatureChart.getDescription().setEnabled(false);

        Legend legend = temperatureChart.getLegend();
        legend.setEnabled(false);
        legend.setTextSize(12);
        legend.setTextColor(Color.BLACK);

        temperatureChart.setDragEnabled(true);
        temperatureChart.setScaleEnabled(true);

        temperatureChart.setData(data);
    }

    private void subscribeTopic(String topic) {
        Toast.makeText(this, "subscribing to topic: " + topic, Toast.LENGTH_SHORT).show();
        client.subscribe(topic, new IMqttMessageListener() {
            @Override
            public void messageArrived(String topic, MqttMessage message) throws Exception {
                //Log.d("Is the message arrived","will see");
                String payload = new String(message.getPayload());
                Float temperature = Float.parseFloat(payload);

                LocalTime currentTime = LocalTime.now();
                int seconds = currentTime.toSecondOfDay();
                float time = (float) seconds;

                Entry entry = new Entry(time,temperature);
                tempData.add(entry);
                temperatureDataSet.setValues(tempData);
                temperatureDataSet.setValueTextSize(0);

                // Notify the LineData that the data has changed
                data.notifyDataChanged();

                // Notify the chart that the data has changed
                temperatureChart.notifyDataSetChanged();
                temperatureChart.setData(data);
                temperatureChart.invalidate();
                System.out.println(tempData);
            }
        });
    }

    public void mainActivity(View view) {
        Intent intent = new Intent(this, MainActivity.class);
        startActivity(intent);
    }

    public void dailyGraph(View view){
    }
    public void weeklyGraph(View view){

    }
}