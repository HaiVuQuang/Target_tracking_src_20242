#ifndef LCD_OUTLINE_H
#define LCD_OUTLINE_H



//Libraries

#include <Arduino.h>



#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <string>
#include <math.h>
#include <cmath>
#include <vector>


#include "config.h"
#include "lcd_algorithm.h"



//Function declarations

void lcd_setup_intro(Adafruit_ILI9341& tft);



void lcd_setup_outline(Adafruit_ILI9341& tft);



void lcd_setup_training_mode_outline(Adafruit_ILI9341& tft);


void TFT_setup(Adafruit_ILI9341& tft);

void lcd_setup_reality_map_outline(Adafruit_ILI9341& tft);

void lcd_update_reality_text_outline(Adafruit_ILI9341& tft, int valve_open_status, User_data &user_data);

void lcd_setup_reality_text_outline(Adafruit_ILI9341& tft);

void draw_progress_bar(Adafruit_ILI9341& tft, int x, int y, int width, int height, int percentage);

#endif