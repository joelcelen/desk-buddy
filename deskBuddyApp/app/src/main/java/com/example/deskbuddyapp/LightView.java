package com.example.deskbuddyapp;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.graphics.Color;
import android.graphics.Typeface;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Toast;

import com.github.mikephil.charting.charts.LineChart;
import com.github.mikephil.charting.components.Legend;
import com.github.mikephil.charting.components.XAxis;
import com.github.mikephil.charting.components.YAxis;
import com.github.mikephil.charting.data.Entry;
import com.github.mikephil.charting.data.LineData;
import com.github.mikephil.charting.data.LineDataSet;
import com.github.mikephil.charting.interfaces.datasets.ILineDataSet;
import com.github.mikephil.charting.utils.Utils;

import org.eclipse.paho.client.mqttv3.IMqttMessageListener;
import org.eclipse.paho.client.mqttv3.MqttMessage;

import java.time.LocalTime;
import java.util.ArrayList;

public class LightView extends AppCompatActivity {
    private MqttHandler client;
    static ArrayList<Entry> lightData = new ArrayList<>();
    static LineChart lightChart;
    static LineDataSet lightDataSet = new LineDataSet(lightData,"Light Data Set");
    static ArrayList<ILineDataSet> dataSets = new ArrayList<>();
    static LineData data = new LineData(dataSets);

    static Legend legend;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(null);
        setContentView(R.layout.activity_light_view);
        lightChart = findViewById(R.id.lightChart);
        client = MqttHandler.getInstance();
        subscribeTopic(Topics.LIGHT_SUB.getTopic());
        generateGraph();
    }

    public void generateGraph(){

        lightChart.refreshDrawableState();
        lightChart.setDragEnabled(true);
        lightChart.setScaleEnabled(false);


        lightDataSet .setColor(Color.BLUE);
        lightDataSet .setLineWidth(2f);
        lightDataSet .setDrawCircles(true);
        lightDataSet .setCircleColor(Color.BLUE);
        lightDataSet .setCircleRadius(5f);


        dataSets.add(lightDataSet);


        XAxis xAxis = lightChart.getXAxis();
        xAxis.setTextSize(30);
        xAxis.setDrawLabels(false);
        xAxis.setTextColor(Color.BLACK);
        xAxis.setTypeface(Typeface.DEFAULT_BOLD);
        xAxis.setPosition(XAxis.XAxisPosition.BOTTOM);

        xAxis.setEnabled(false);

        YAxis yAxis = lightChart.getAxisLeft();
        yAxis.setTextSize(30);
        yAxis.setTextColor(Color.BLACK);
        yAxis.setTypeface(Typeface.DEFAULT_BOLD);
        lightChart.getAxisRight().setEnabled(false);
        lightChart.getDescription().setEnabled(false);

        legend = lightChart.getLegend();
        legend.setEnabled(false);
        legend.setTextSize(12);
        legend.setTextColor(Color.BLACK);

        lightChart.setDragEnabled(true);
        lightChart.setScaleEnabled(true);

        lightChart.setData(data);
    }

    private void subscribeTopic(String topic) {
        Toast.makeText(this, "subscribing to topic: " + topic, Toast.LENGTH_SHORT).show();
        client.subscribe(topic, new IMqttMessageListener() {
            @Override
            public void messageArrived(String topic, MqttMessage message) throws Exception {
                //Log.d("Is the message arrived","will see");
                String payload = new String(message.getPayload());
                Float light = Float.parseFloat(payload);

                LocalTime currentTime = LocalTime.now();
                int seconds = currentTime.toSecondOfDay();
                float time = (float) seconds;

                Entry entry = new Entry(time,light);
                lightData.add(entry);
                lightDataSet.setValues(lightData);
                lightDataSet.setValueTextSize(0);

                // Notify the LineData that the data has changed
                data.notifyDataChanged();

                // Notify the chart that the data has changed
                lightChart.notifyDataSetChanged();
                lightChart.setData(data);
                lightChart.invalidate();
                System.out.println(lightData);
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