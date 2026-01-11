#ifndef DEBUG_H
#define DEBUG_H


#include <Arduino.h>



#include "config.h"



void print_RSSI_data(RSSI_Data* rssi_data, int num_of_rssi_data);



void print_IMU_data(IMU_Data imu_data);



void print_topic_and_message(String message, int mode_of_system);



#endif // DEBUG_H