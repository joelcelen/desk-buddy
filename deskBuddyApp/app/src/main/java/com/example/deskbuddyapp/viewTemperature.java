package com.example.deskbuddyapp;

import androidx.appcompat.app.AppCompatActivity;

import android.graphics.Color;
import android.graphics.Typeface;
import android.os.Bundle;
import android.view.View;

import com.github.mikephil.charting.charts.LineChart;
import com.github.mikephil.charting.components.Legend;
import com.github.mikephil.charting.components.XAxis;
import com.github.mikephil.charting.components.YAxis;
import com.github.mikephil.charting.data.Entry;
import com.github.mikephil.charting.data.LineData;
import com.github.mikephil.charting.data.LineDataSet;
import com.github.mikephil.charting.formatter.ValueFormatter;
import com.github.mikephil.charting.interfaces.datasets.ILineDataSet;

import java.util.ArrayList;
import java.util.Calendar;

import java.util.Date;
import java.util.Locale;

//Credits to https://www.youtube.com/watch?v=DD1CxoVONFE&ab_channel=KGPTalkie

public class viewTemperature extends AppCompatActivity {
    int preference = 24;    //hard coded preference
    ArrayList <Double> hourlyReading = new ArrayList<>();     //hard coded data
    ArrayList <Double> weeklyReading = new ArrayList<>();     // hard coded data
    Date currentTime;
    LineChart temperatureChart;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_view_temp);

        temperatureChart = findViewById(R.id.tempChart);

        temperatureChart.setDragEnabled(true);

        temperatureChart.setScaleEnabled(false);



        ArrayList<Entry> tempData = new ArrayList<>();

        tempData.add(new Entry(0,60f));
        tempData.add(new Entry(1,40f));
        tempData.add(new Entry(2,30f));
        tempData.add(new Entry(3,70f));
        tempData.add(new Entry(4,10f));
        tempData.add(new Entry(5,35f));

        //currentTime = Calendar.getInstance().getTime();

        LineDataSet set1 = new LineDataSet(tempData,"Temperature Data Set");

        set1.setColor(Color.BLUE);
        set1.setLineWidth(2f);
        set1.setDrawCircles(true);
        set1.setCircleColor(Color.BLUE);
        set1.setCircleRadius(5f);
        set1.setValueTextSize(18f);


        ArrayList<ILineDataSet> dataSets = new ArrayList<>();
        dataSets.add(set1);

        LineData data = new LineData(dataSets);

        XAxis xAxis = temperatureChart.getXAxis();
        xAxis.setTextSize(12);
        xAxis.setTextColor(Color.BLACK);
        xAxis.setTypeface(Typeface.DEFAULT_BOLD);
        xAxis.setPosition(XAxis.XAxisPosition.BOTTOM);

        YAxis yAxis = temperatureChart.getAxisLeft();
        yAxis.setTextSize(12);
        yAxis.setTextColor(Color.BLACK);
        yAxis.setTypeface(Typeface.DEFAULT_BOLD);
        temperatureChart.getAxisRight().setEnabled(false);

        Legend legend = temperatureChart.getLegend();
        legend.setEnabled(true);
        legend.setTextSize(12);
        legend.setTextColor(Color.BLACK);

        temperatureChart.setDragEnabled(true);
        temperatureChart.setScaleEnabled(true);

        temperatureChart.setData(data);
    }

    private void dailyGraph(View view){

    }
    private void weeklyGraph(View view){

    }
}