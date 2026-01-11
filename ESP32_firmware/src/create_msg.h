#ifndef CREATE_MSG_H
#define CREATE_MSG_H



// Libraries
#include <Arduino.h>



#include "config.h"



// Function declarations
/*#############################################################################################################*/
/**
 * @brief Create training mode message
 * @param rssi_data RSSI data array
 * @param imu_data IMU sensor data
 * @param training_counter Training mode counter
 * @param message Pointer to output message string
 */
/*#############################################################################################################*/
void get_training_msg(RSSI_Data* rssi_data, IMU_Data imu_data, int training_counter, String* message);



/*#############################################################################################################*/
/**
 * @brief Create reality mode message
 * @param rssi_data RSSI data array
 * @param imu_data IMU sensor data
 * @param valve_status Valve analog status
 * @param mode_status Mode analog status
 * @param message Pointer to output message string
 */
/*#############################################################################################################*/
void get_real_msg(RSSI_Data* rssi_data, IMU_Data imu_data, int valve_status, bool mode_status, String* message, unsigned long time);



// create_msg.h - add this function declaration:
/*#############################################################################################################*/
/**
 * @brief Create device information message
 * @param training_topic Training topic string
 * @param real_topic Reality topic string
 * @param device_id Device identifier
 * @param message Pointer to output message string
 */
/*#############################################################################################################*/
void get_infor_msg(String training_topic, String real_topic, int device_id, String* message);



#endif