#ifndef LCD_CONTROL_H
#define LCD_CONTROL_H



//Libraries

#include <Arduino.h>



#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>



#include "config.h"
#include "lcd_algorithm.h"



//Function declarations
void lcd_display_default_mode(Adafruit_ILI9341& tft, RSSI_Data* rssi_data);



bool lcd_is_ready_to_refresh_training_mode();



void lcd_display_training_mode(Adafruit_ILI9341& tft, RSSI_Data* rssi_data, IMU_Data imu_data, int training_counter);



#endif