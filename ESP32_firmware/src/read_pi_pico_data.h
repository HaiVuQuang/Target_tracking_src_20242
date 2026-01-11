#ifndef READ_PI_PICO_DATA_H
#define READ_PI_PICO_DATA_H



//Libraries
#include <Arduino.h>



#include "config.h"



//Function declarations

/*#############################################################################################################*/
/**
 * @brief Read RSSI values from Raspberry Pi Pico via UART
 * @param uart HardwareSerial reference for UART communication
 * @param rssi_data Array to store RSSI data
 * @return None
 */
/*#############################################################################################################*/
void read_Pi_Pico_data(HardwareSerial& uart, RSSI_Data* rssi_data);



/*#############################################################################################################*/
/**
 * @brief Monitor and update RSSI timeout status
 * @param rssi_data Array of RSSI values to check
 * @return None
 */
/*#############################################################################################################*/
void check_last_receive_time(RSSI_Data* rssi_data);



#endif