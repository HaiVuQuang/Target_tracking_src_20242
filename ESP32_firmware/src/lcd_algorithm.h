#ifndef LCD_ALGORITHM_H
#define LCD_ALGORITHM_H



//Libraries

#include <Arduino.h>



#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>



#include "config.h"

extern Fire fire;
extern Fire last_fire;
extern User_data user_data;
extern User_data last_user_data;


//Function declarations

void draw_qr_code(Adafruit_ILI9341& tft, int x, int y, int scale = 2);



void evaluate_rssi_value(Adafruit_ILI9341& tft, int rssi_value, int x, int y);


void id_to_coordinate(vector<int>& map_id, vector<Coordinate>* position);

void coordinate_to_pixel(float coor_x, float coor_y, int& pixel_x, int& pixel_y);

void draw_reality_map(Adafruit_ILI9341& tft, vector<Coordinate>& map_grid, uint16_t color);

void draw_reality_not_map(Adafruit_ILI9341& tft, vector<Coordinate>& not_map_grid, uint16_t color);

void user_handle(Adafruit_ILI9341& tft, User_data &user_data, float north_offset, IMU_Data &imu_data);

void fire_handle(Adafruit_ILI9341& tft, Fire fire);

void user_display_delete(Adafruit_ILI9341& tft, User_data &user_data);

void fire_display_delete(Adafruit_ILI9341& tft, Fire fire);
#endif