#include "create_msg.h"



void get_training_msg(RSSI_Data* rssi_data, IMU_Data imu_data, int training_counter, String* message) {
    *message = String(rssi_data[0].rssi) + "," +
               String(rssi_data[1].rssi) + "," + 
               String(rssi_data[2].rssi) + "," +
               String(rssi_data[3].rssi) + "," +
            //    String(imu_data.acc.x) + "," +
            //    String(imu_data.acc.y) + "," +
            //    String(imu_data.acc.z) + "," +
            //    String(imu_data.mag.x) + "," +
               String(imu_data.mag.y) + "," +
               String(imu_data.mag.z) + "," +
               String(imu_data.gyro.x) + "," +
               String(imu_data.gyro.y) + "," +
               String(imu_data.gyro.z) + "," +
               String(imu_data.euler.x) + "," +
               String(imu_data.euler.y) + "," +
               String(imu_data.euler.z);
}



void get_real_msg(RSSI_Data* rssi_data, IMU_Data imu_data, int valve_status, bool mode_status, String* message, unsigned long time) {
    *message = String(rssi_data[0].rssi) + "," +
               String(rssi_data[1].rssi) + "," +
               String(rssi_data[2].rssi) + "," +
               String(rssi_data[3].rssi) + "," +
               String(imu_data.acc.x) + "," +
               String(imu_data.acc.y) + "," +
               String(imu_data.acc.z) + "," +
               String(imu_data.mag.x) + "," +
               String(imu_data.mag.y) + "," +
               String(imu_data.mag.z) + "," +
               String(imu_data.gyro.x) + "," +
               String(imu_data.gyro.y) + "," +
               String(imu_data.gyro.z) + "," +
               String(imu_data.euler.x) + "," +
               String(imu_data.euler.y) + "," +
               String(imu_data.euler.z) + "," +
               String(valve_status) + "," +
               String(mode_status) + "," +
               String(time);
}



void get_infor_msg(String training_topic, String real_topic, int device_id, String* message) {
    *message = String(device_id) + "," +
               training_topic + "," +
               real_topic;
}