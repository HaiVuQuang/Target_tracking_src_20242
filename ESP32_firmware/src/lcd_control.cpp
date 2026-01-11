#include <lcd_control.h>



unsigned long last_refresh_time_training_mode = 0;



bool lcd_is_ready_to_refresh_training_mode()
{
    unsigned long current_time = millis();
    int refresh_time_training_mode = 1000/LCD_REFRESH_RATE_TRAINING_MODE;
    if (current_time - last_refresh_time_training_mode >= refresh_time_training_mode)
    {
        last_refresh_time_training_mode = current_time;
        return true;
    }
    return false;
}



void lcd_display_default_mode(Adafruit_ILI9341 &tft, RSSI_Data* rssi_data)
{
    //Setup out line
    tft.setCursor(40, 15);
    tft.setTextColor(WHITE);
    tft.setTextSize(1);
    tft.println("Default");

    //Information of device
    tft.setTextColor(WHITE);

    tft.setCursor(5, 30);
    tft.setTextSize(1);
    tft.println("Device ID");
    tft.setCursor(70, 30);
    tft.println(": " + String(DEVICE_ID));

    tft.setCursor(5, 40);
    tft.println("Device Name");
    tft.setCursor(70, 40);
    tft.println(": " + String(DEVICE_NAME));

    //Information of MQTT
    tft.setTextColor(WHITE);

    tft.setCursor(5, 55);
    tft.println("Wi-Fi connected");
    tft.setCursor(100, 55);
    tft.println(": " + String(WIFI_SSID));

    tft.setCursor(5, 65);
    tft.println("Broker connected");
    tft.setCursor(100, 65);
    tft.println(": " + String(MQTT_BROKER));

    tft.setCursor(5, 75);
    tft.println("Training topic");
    tft.setCursor(100, 75);
    tft.println(": " + String(MQTT_TRAINING_TOPIC));

    tft.setCursor(5, 85);
    tft.println("Reality topic");
    tft.setCursor(100, 85);
    tft.println(": " + String(MQTT_REALITY_TOPIC));

    //Information of RSSI
    tft.setTextColor(WHITE);

    tft.setCursor(5, 100);
    tft.println("SSID_ID: " + String(rssi_data[0].ssid_id));
    tft.setCursor(70, 100);
    tft.println(" | SSID: " + String(rssi_data[0].ssid));

    tft.setCursor(5, 110);  
    tft.println("SSID_ID: " + String(rssi_data[1].ssid_id));
    tft.setCursor(70, 110);
    tft.println(" | SSID: " + String(rssi_data[1].ssid));

    tft.setCursor(5, 120);
    tft.println("SSID_ID: " + String(rssi_data[2].ssid_id));
    tft.setCursor(70, 120);
    tft.println(" | SSID: " + String(rssi_data[2].ssid));

    tft.setCursor(5, 130);
    tft.println("SSID_ID: " + String(rssi_data[3].ssid_id));
    tft.setCursor(70, 130);
    tft.println(" | SSID: " + String(rssi_data[3].ssid));

    //Turtorial
    tft.setTextColor(WHITE);

    tft.setCursor(5, 145);
    tft.println("Guide: "+ String(YOUTUBE_LINK));
    draw_qr_code(tft, 5, 160, 2);

    tft.setTextColor(RED);
    tft.setCursor(80, 160);
    tft.println("SYSTEM MODE SWITCH");
    tft.setTextColor(WHITE);
    tft.setCursor(200, 160);
    tft.println(": Press to switch");
    tft.setCursor(80, 170);
    tft.println("Reality mode <-> Training mode");

    tft.setTextColor(GREEN);
    tft.setCursor(80, 185);
    tft.println("ENABLE MSG SWITCH");
    tft.setTextColor(WHITE);
    tft.setCursor(200, 185);
    tft.println(": Press to switch");
    tft.setCursor(80, 195);
    tft.println("Default mode <-> Training/Reality mode");

    
}



void lcd_display_training_mode(Adafruit_ILI9341& tft, RSSI_Data* rssi_data, IMU_Data imu_data, int training_counter)
{
    //RSSI data
    //Clear previous data
    tft.fillRect(70, 50, 14, 40, BLACK);
    tft.fillRect(140, 50, 200, 40, BLACK);

    tft.setTextColor(WHITE);
    tft.setCursor(70, 50);
    tft.println(String(rssi_data[0].ssid_id));
    tft.setCursor(140, 50);
    tft.println(String(rssi_data[0].rssi));
    evaluate_rssi_value(tft, rssi_data[0].rssi, 200, 50);

    tft.setCursor(70, 60);
    tft.println(String(rssi_data[1].ssid_id));
    tft.setCursor(140, 60);
    tft.println(String(rssi_data[1].rssi));
    evaluate_rssi_value(tft, rssi_data[1].rssi, 200, 60);

    tft.setCursor(70, 70);
    tft.println(String(rssi_data[2].ssid_id));
    tft.setCursor(140, 70);
    tft.println(String(rssi_data[2].rssi));
    evaluate_rssi_value(tft, rssi_data[2].rssi, 200, 70);

    tft.setCursor(70, 80);
    tft.println(String(rssi_data[3].ssid_id));
    tft.setCursor(140, 80);
    tft.println(String(rssi_data[3].rssi));
    evaluate_rssi_value(tft, rssi_data[3].rssi, 200, 80);

    //IMU data
    //Clear previous data
    tft.fillRect(120, 115, 39, 40, BLACK);
    tft.fillRect(195, 115, 39, 40, BLACK);
    tft.fillRect(270, 115, 39, 40, BLACK);

    tft.setCursor(120, 115);
    tft.println(String(imu_data.acc.x));
    tft.setCursor(195, 115);
    tft.println(String(imu_data.acc.y));
    tft.setCursor(270, 115);
    tft.println(String(imu_data.acc.z));

    tft.setCursor(120, 125);
    tft.println(String(imu_data.mag.x));
    tft.setCursor(195, 125);
    tft.println(String(imu_data.mag.y));
    tft.setCursor(270, 125);
    tft.println(String(imu_data.mag.z));

    tft.setCursor(120, 135);
    tft.println(String(imu_data.gyro.x));
    tft.setCursor(195, 135);
    tft.println(String(imu_data.gyro.y));
    tft.setCursor(270, 135);
    tft.println(String(imu_data.gyro.z));

    tft.setCursor(120, 145);
    tft.println(String(imu_data.euler.x));
    tft.setCursor(195, 145);
    tft.println(String(imu_data.euler.y));
    tft.setCursor(270, 145);
    tft.println(String(imu_data.euler.z));

    //Training counter
    tft.fillRect(120, 165, 30, 10, BLACK);
    tft.setCursor(120, 165);
    tft.println(String(training_counter));
}