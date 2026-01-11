#include <lcd_outline.h>



void lcd_setup_intro(Adafruit_ILI9341 &tft)
{
    // Intro
    tft.fillScreen(WHITE);
    tft.setCursor(40, 50);
    tft.setTextColor(RED);
    tft.setTextSize(5);
    tft.println("iPac Lab");

    tft.setCursor(30, 150);
    tft.setTextColor(BLACK);
    tft.setTextSize(3);
    tft.println("Target Tracking");

    tft.setCursor(85, 200);
    tft.setTextColor(BLACK);
    tft.setTextSize(1);
    tft.println("Set up hardware serial...");

    delay(INTRO_TIME);

    tft.fillRect(70, 200, 170, 10, WHITE);
    tft.setCursor(70, 200);
    tft.setTextColor(BLACK);
    tft.setTextSize(1);
    tft.println("Connecting to wifi and MQTT...");
}



void lcd_setup_outline(Adafruit_ILI9341& tft)
{
    tft.fillScreen(BLACK);
    tft.setCursor(85, 5);
    tft.setTextColor(WHITE);
    tft.setTextSize(1);
    tft.println("iPAC Lab | Target Tracking");
    tft.setCursor(5, 15);
    tft.setTextColor(WHITE);
    tft.setTextSize(1);
    tft.println("mode: ");
}



void lcd_setup_training_mode_outline(Adafruit_ILI9341& tft)
{
    lcd_setup_outline(tft);
    tft.setCursor(40, 15);
    tft.setTextColor(RED);
    tft.setTextSize(1);
    tft.println("TRAINING");

    tft.setTextColor(WHITE);
    tft.setCursor(5, 35);
    tft.println("RSSI data");
    tft.setCursor(15, 50);
    tft.println("SSID_ID:");
    tft.setCursor(85, 50);
    tft.println(" | RSSI:");
    tft.setCursor(15, 60);
    tft.println("SSID_ID:");
    tft.setCursor(85, 60);
    tft.println(" | RSSI:");
    tft.setCursor(15, 70);
    tft.println("SSID_ID:");
    tft.setCursor(85, 70);
    tft.println(" | RSSI:");
    tft.setCursor(15, 80);
    tft.println("SSID_ID:");
    tft.setCursor(85, 80);
    tft.println(" | RSSI:");

    tft.setCursor(5, 100);
    tft.println("IMU data");
    tft.setCursor(15, 115);
    tft.println("Acc[x,y,z]");
    tft.setCursor(85, 115);
    tft.println(" | x: ");
    tft.setCursor(160, 115);
    tft.println(" | y: ");
    tft.setCursor(235, 115);
    tft.println(" | z: ");
    tft.setCursor(15, 125);
    tft.println("Mag[x,y,z]");
    tft.setCursor(85, 125);
    tft.println(" | x: ");
    tft.setCursor(160, 125);
    tft.println(" | y: ");
    tft.setCursor(235, 125);
    tft.println(" | z: ");
    tft.setCursor(15, 135);
    tft.println("Gyro[x,y,z]");
    tft.setCursor(85, 135);
    tft.println(" | x: ");
    tft.setCursor(160, 135);
    tft.println(" | y: ");
    tft.setCursor(235, 135);
    tft.println(" | z: ");
    tft.setCursor(15, 145);
    tft.println("Euler[x,y,z]");
    tft.setCursor(85, 145);
    tft.println(" | x: ");
    tft.setCursor(160, 145);
    tft.println(" | y: ");
    tft.setCursor(235, 145);
    tft.println(" | z: ");

    tft.setCursor(5, 165);
    tft.println("Training counter: ");
}

//Setup tft display
void TFT_setup(Adafruit_ILI9341& tft){
    tft.begin();
    delay(100);
    tft.setRotation(LCD_ROTATION);
    tft.fillScreen(BLACK);
    Serial.println("TFT display ready!");
}

void lcd_setup_reality_map_outline(Adafruit_ILI9341& tft){
    tft.setTextColor(WHITE);
    tft.setTextSize(1);
    tft.setCursor(32, 5);
    tft.println("Map layout - Reality mode");
    //Vẽ các trục của map
    tft.drawFastVLine(9, 19, 202, WHITE);
    tft.drawFastHLine(9, 221, 202, WHITE);
    tft.drawFastVLine(217, 0, 240, WHITE);
    //Trục Y - Hướng north của map
    tft.setCursor(0, 16);
    tft.println("10");
    tft.setCursor(1, 36);
    tft.println("9");
    tft.setCursor(1, 56);
    tft.println("8");
    tft.setCursor(1, 76);
    tft.println("7");
    tft.setCursor(1, 96);
    tft.println("6");
    tft.setCursor(1, 116);
    tft.println("5");
    tft.setCursor(1, 136);
    tft.println("4");
    tft.setCursor(1, 156);
    tft.println("3");
    tft.setCursor(1, 176);
    tft.println("2");
    tft.setCursor(1, 196);
    tft.println("1");
    //Trục x
    tft.setCursor(27, 226);
    tft.println("1");
    tft.setCursor(47, 226);
    tft.println("2");
    tft.setCursor(67, 226);
    tft.println("3");
    tft.setCursor(87, 226);
    tft.println("4");
    tft.setCursor(107, 226);
    tft.println("5");
    tft.setCursor(127, 226);
    tft.println("6");
    tft.setCursor(147, 226);
    tft.println("7");
    tft.setCursor(167, 226);
    tft.println("8");
    tft.setCursor(187, 226);
    tft.println("9");
    tft.setCursor(207, 226);
    tft.println("10");
    tft.setCursor(1, 226);
    tft.println("0");
}

void lcd_setup_reality_text_outline(Adafruit_ILI9341& tft){
    tft.setTextColor(WHITE);
    tft.setTextSize(1);
    //Setup text tĩnh
    tft.setCursor(220, 5);
    tft.println("Device Id:");
    tft.setCursor(240, 24);
    tft.println("User score");
    tft.setCursor(220, 75);
    tft.println("Valve Opening:");
    tft.setCursor(220, 123);
    tft.println("User position:");
    tft.setCursor(220, 136);
    tft.println("X:");
    tft.setCursor(220, 149);
    tft.println("Y:");
    //Bảng User score
    tft.drawFastVLine(220, 21, 49, WHITE);
    tft.drawFastVLine(319, 21, 49, WHITE);
    tft.drawFastHLine(220, 21, 99, WHITE);
    tft.drawFastHLine(220, 35, 99, WHITE);
    tft.drawFastHLine(220, 70, 99, WHITE);
}

void lcd_update_reality_text_outline(Adafruit_ILI9341& tft, int valve_open_status, User_data &user_data){
    //Xóa các ô dữ liệu cũ
    //Ô device id
    tft.fillRect(285, 5, 14, 9, BLACK);
    //Ô user score
    tft.fillRect(229, 39, 92, 26, BLACK);
    //Ô User position
    tft.fillRect(237, 136, 38, 9, BLACK);
    tft.fillRect(237, 149, 38, 9, BLACK);
    tft.fillRect(261, 162, 14, 9, BLACK);

    //Cập nhật mới text
    tft.setTextColor(WHITE);
    tft.setTextSize(1);
    //Device id
    tft.setCursor(238, 136);
    tft.println(String(user_data.user_x));
    //User position
    tft.setCursor(238, 149);
    tft.println(String(user_data.user_y));
    tft.setCursor(262, 162);
    tft.println(String(user_data.user_speed));
    //User score
    tft.setTextSize(3);
    int text_width = String(user_data.user_score).length() * 18;
    int middle_x = 230 + (92 - text_width)/2;
    tft.setCursor(middle_x, 40);
    tft.println(String(user_data.user_score));
    tft.setTextSize(1);
    //Cập nhật mới progress bar độ mở van
    draw_progress_bar(tft, 220, 88, 100, 30, valve_open_status);

}

void draw_progress_bar(Adafruit_ILI9341& tft, int x, int y, int width, int height, int percentage) {
    if (percentage < 0) percentage = 0;
    if (percentage > 100) percentage = 100;
    //Đổi màu theo progress
    uint16_t color;
    if (percentage <= 33){
        color = RED;   // Đỏ
    }else if(percentage <= 66){
        color = YELLOW;  // Vàng
    }else{
        color = GREEN;    // Xanh lá
    }
    //
    int filled_width = round((percentage * width)/100);
    //Cập nhật thanh progress
    tft.drawRect(x, y, width, height, WHITE);
    tft.fillRect(x + 1, y + 1, width - 2, height - 2, BLACK);
    tft.fillRect(x + 1, y + 1, filled_width - 2, height - 2, color);
}