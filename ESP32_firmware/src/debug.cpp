#include "debug.h"



void print_RSSI_data(RSSI_Data *rssi_data, int num_of_rssi_data)
{
    for (int i = 0; i < num_of_rssi_data; i++)
    {
        // Serial.print("SSID: ");
        // Serial.print(rssi_data[i].ssid);
        Serial.print(" | SSID_ID: ");
        Serial.print(rssi_data[i].ssid_id);
        Serial.print(" | RSSI: ");
        Serial.print(rssi_data[i].rssi);
        Serial.print(" | Last Received Time: ");
        Serial.println(rssi_data[i].last_receive_time);
    }
}



void print_IMU_data(IMU_Data imu_data)
{
    Serial.print("| Acc: ");
    Serial.print(imu_data.acc.x);
    Serial.print(" ");
    Serial.print(imu_data.acc.y);
    Serial.print(" ");
    Serial.print(imu_data.acc.z);
    Serial.print(" | Mag: ");
    Serial.print(imu_data.mag.x);
    Serial.print(" ");
    Serial.print(imu_data.mag.y);
    Serial.print(" ");
    Serial.print(imu_data.mag.z);
    Serial.print(" | Gyro: ");
    Serial.print(imu_data.gyro.x);
    Serial.print(" ");
    Serial.print(imu_data.gyro.y);
    Serial.print(" ");
    Serial.print(imu_data.gyro.z);
    Serial.print(" | Euler: ");
    Serial.print(imu_data.euler.x);
    Serial.print(" ");
    Serial.print(imu_data.euler.y);
    Serial.print(" ");
    Serial.println(imu_data.euler.z);
}



void print_topic_and_message(String message, int mode_of_system)
{
    if (mode_of_system == DEFAULT_MODE)
    {
        Serial.print("| MODE ");
        Serial.print(mode_of_system);
        Serial.print(": DEFAULT ");
        Serial.print("| Topic: ");
        Serial.print(MQTT_DEVICE_INFOR_TOPIC);
    }
    else if (mode_of_system == TRAINING_MODE)
    {
        Serial.print("| MODE ");
        Serial.print(mode_of_system);
        Serial.print(": TRAINING ");
        Serial.print("| Topic: ");
        Serial.print(MQTT_TRAINING_TOPIC);
    }
    else if (mode_of_system == REALITY_MODE)
    {
        Serial.print("| MODE ");
        Serial.print(mode_of_system);
        Serial.print(": REALITY ");
        Serial.print("| Topic: ");
        Serial.print(MQTT_REALITY_TOPIC);
    }
    Serial.print(" | Message: ");
    Serial.println(message);
}