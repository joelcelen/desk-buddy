package com.example.deskbuddyapp;
import androidx.appcompat.app.AppCompatActivity;
import android.os.Bundle;
import android.content.Intent;
import android.graphics.Color;
import android.graphics.Typeface;
import android.view.View;
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

public class HumidityView extends AppCompatActivity {
    private MqttHandler client;
    ArrayList<Entry> humData = new ArrayList<>();
    LineChart humidityChart;
    LineDataSet humidityDataSet = new LineDataSet(humData,"Humidity Data Set");
    ArrayList<ILineDataSet> dataSets = new ArrayList<>();
    LineData data = new LineData(dataSets);

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_humidity_view);
        humidityChart = findViewById(R.id.humChart);
        client = MqttHandler.getInstance();
        subscribeTopic(Topics.HUMIDITY_SUB.getTopic());
        generateGraph();
    }

    public void generateGraph(){
        humidityChart.refreshDrawableState();
        humidityChart.setDragEnabled(true);
        humidityChart.setScaleEnabled(false);


        humidityDataSet.setColor(Color.GREEN);
        humidityDataSet.setLineWidth(2f);
        humidityDataSet.setDrawCircles(true);
        humidityDataSet.setCircleColor(Color.BLUE);
        humidityDataSet.setCircleRadius(5f);
        humidityDataSet.setValueTextColor(Color.WHITE);


        dataSets.add(humidityDataSet);


        XAxis xAxis = humidityChart.getXAxis();
        xAxis.setTextSize(30);
        xAxis.setDrawLabels(false);
        xAxis.setTextColor(Color.BLACK);
        xAxis.setTypeface(Typeface.DEFAULT_BOLD);
        xAxis.setPosition(XAxis.XAxisPosition.BOTTOM);

        xAxis.setEnabled(false);

        YAxis yAxis = humidityChart.getAxisLeft();
        yAxis.setTextSize(30);
        yAxis.setTextColor(Color.WHITE);
        yAxis.setTypeface(Typeface.DEFAULT_BOLD);
        humidityChart.getAxisRight().setEnabled(false);
        humidityChart.getDescription().setEnabled(false);

        Legend legend = humidityChart.getLegend();
        legend.setEnabled(true);
        legend.setTextSize(12);
        legend.setTextColor(Color.WHITE);

        humidityChart.setDragEnabled(true);
        humidityChart.setScaleEnabled(true);

        humidityChart.setData(data);
    }

    private void subscribeTopic(String topic) {
        client.subscribe(topic, new IMqttMessageListener() {
            @Override
            public void messageArrived(String topic, MqttMessage message) throws Exception {
                String payload = new String(message.getPayload());
                Float humidity = Float.parseFloat(payload);

                LocalTime currentTime = LocalTime.now();
                int seconds = currentTime.toSecondOfDay();
                float time = (float) seconds;

                Entry entry = new Entry(time,humidity);
                humData.add(entry);
                humidityDataSet.setValues(humData);
                humidityDataSet.setValueTextSize(0);

                // Notify the LineData that the data has changed
                data.notifyDataChanged();

                // Notify the chart that the data has changed
                humidityChart.notifyDataSetChanged();
                humidityChart.setData(data);
                humidityChart.invalidate();
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
