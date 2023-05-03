package com.example.deskbuddyapp;

import androidx.appcompat.app.AppCompatActivity;

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

import org.eclipse.paho.client.mqttv3.IMqttMessageListener;
import org.eclipse.paho.client.mqttv3.MqttMessage;

import java.io.InputStream;
import java.time.LocalTime;
import java.util.ArrayList;

import java.util.Scanner;

//Credits to https://www.youtube.com/watch?v=DD1CxoVONFE&ab_channel=KGPTalkie

public class viewTemperature extends AppCompatActivity {
    private static final String TOPIC = "deskBuddy/light";
    private static final String CLIENT_ID = "androidDeskBuddy";
    private String brokerUrl;
    private String username;
    private String password;
    private InputStream inputStream;
    private Scanner scanBrokerInfo;
    private MqttHandler client;
    ArrayList<Entry> tempData = new ArrayList<>();
    LineChart temperatureChart;
    LineDataSet temperatureDataSet = new LineDataSet(tempData,"Temperature Data Set");
    ArrayList<ILineDataSet> dataSets = new ArrayList<>();
    LineData data = new LineData(dataSets);

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_view_temp);
        temperatureChart = findViewById(R.id.tempChart);
        initializeConnection();
        subscribeTopic();
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
        //set1.setValueTextSize(18f);


        dataSets.add(temperatureDataSet);



        XAxis xAxis = temperatureChart.getXAxis();
        xAxis.setTextSize(30);
        xAxis.setDrawLabels(false);
        xAxis.setTextColor(Color.BLACK);
        xAxis.setTypeface(Typeface.DEFAULT_BOLD);
        xAxis.setPosition(XAxis.XAxisPosition.BOTTOM);

        //xAxis.setEnabled(false);

        YAxis yAxis = temperatureChart.getAxisLeft();
        yAxis.setTextSize(12);
        yAxis.setTextColor(Color.BLACK);
        yAxis.setTypeface(Typeface.DEFAULT_BOLD);
        temperatureChart.getAxisRight().setEnabled(false);

        Legend legend = temperatureChart.getLegend();
        legend.setEnabled(false);
        legend.setTextSize(12);
        legend.setTextColor(Color.BLACK);

        temperatureChart.setDragEnabled(true);
        temperatureChart.setScaleEnabled(true);

        temperatureChart.setData(data);
    }

    public void initializeConnection(){
        inputStream = getResources().openRawResource(R.raw.brokerinfo);
        {
            try {
                scanBrokerInfo = new Scanner(inputStream);
                while (scanBrokerInfo.hasNextLine()) {
                    brokerUrl = scanBrokerInfo.next();
                    username = scanBrokerInfo.next();
                    password = scanBrokerInfo.next();
                }
                inputStream.close();
            } catch (Exception e) {
                throw new RuntimeException(e);
            }
        }

        client = new MqttHandler();
        client.connect(brokerUrl, CLIENT_ID, username, password);
        scanBrokerInfo.close();
    }
    public void subscribeTopic(){
        subscribeTopic(TOPIC);
    }

    private void subscribeTopic(String topic) {
        Toast.makeText(this, "subscribing to topic: " + topic, Toast.LENGTH_SHORT).show();
        client.subscribe(topic, new IMqttMessageListener() {
            @Override
            public void messageArrived(String topic, MqttMessage message) throws Exception {
                Log.d("Is the message arrived","will see");
                String payload = new String(message.getPayload());
                Float temperature = Float.parseFloat(payload);
                //GregorianCalendar calendar = new GregorianCalendar();
                //long currentTime = calendar.getTimeInMillis();
                //SimpleDateFormat sdf = new SimpleDateFormat("HH:mm:ss");
                //String formattedTime = sdf.format(currentTime);

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
                //generateGraph();
                System.out.println(tempData);
            }
        });
    }

    private void dailyGraph(View view){

    }
    private void weeklyGraph(View view){

    }
}