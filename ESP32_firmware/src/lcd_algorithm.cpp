#include <lcd_algorithm.h>



void draw_qr_code(Adafruit_ILI9341 &tft, int x, int y, int scale)
{
    for (int row = 0; row < QR_SIZE; row++)
    {
        for (int col = 0; col < QR_SIZE; col++)
        {
            uint16_t color = TUTORIAL_QR[row][col] ? BLACK : WHITE;
            tft.fillRect(x + (col * scale), y + (row * scale), scale, scale, color);
        }
    }
}



void evaluate_rssi_value(Adafruit_ILI9341 &tft, int rssi_value, int x, int y)
{
    if (rssi_value == -100)
    {
        tft.setCursor(x, y);
        tft.setTextColor(RED);
        tft.println("Lost signal");
        tft.setTextColor(WHITE);
    }
    else if (rssi_value > -100 && rssi_value < -80)
    {
        tft.setCursor(x, y);
        tft.setTextColor(ORANGE);
        tft.println("Weak");
        tft.setTextColor(WHITE);
    }
    else if (rssi_value >= -80 && rssi_value <= -30)
    {
        tft.setCursor(x, y);
        tft.setTextColor(YELLOW);
        tft.println("Normal");
        tft.setTextColor(WHITE);
    }
    else if (rssi_value > -30 && rssi_value < 0)
    {
        tft.setCursor(x, y);
        tft.setTextColor(GREEN);
        tft.println("Strong");
        tft.setTextColor(WHITE);
    }
    else
    {
        tft.setCursor(x, y);
        tft.setTextColor(RED);
        tft.println("Not Valid");
        tft.setTextColor(WHITE);
    }
}

void id_to_coordinate(vector<int>& map_id, vector<Coordinate>* position){
 for (int id : map_id) {
        Coordinate coord;
        if (id % 10 != 0){
            coord.x = id % 10;
            coord.y = (id / 10) + 1;
        }else{
            coord.x = 10;
            coord.y = id / 10;
        }
        position->push_back(coord);
    }
}

//Chuyển tọa độ  từ sever thành pixel hiển thị tft
void coordinate_to_pixel(float coor_x, float coor_y, int& pixel_x, int& pixel_y){
    if (coor_x < 0) coor_x = 0;
    if (coor_x > 10) coor_x = 10;
    if (coor_y < 0) coor_y = 0;
    if (coor_y > 10) coor_y = 10;
    // X: từ 0–10 ánh xạ thành pixel từ 9–211
    pixel_x = round(9 + (coor_x / 10.0f) * (211 - 9));
    // Y: từ 0–10 ánh xạ thành pixel từ 221–19 (ngược trục y TFT)
    pixel_y = round(221 - (coor_y / 10.0f) * (221 - 19));
}


//Vẽ map ở chế độ thực tế (các ô đi được)
void draw_reality_map(Adafruit_ILI9341& tft, vector<Coordinate>& map_grid, uint16_t color){
    for (Coordinate pos : map_grid){
        int x_coord = (10 + (pos.x-1)*20);
        int y_coord = (221 - pos.y*20);
        tft.drawRect(x_coord, y_coord, 20, 20, color);
    }
}
//Vẽ map ở chế độ thực tế (các ô không đi được)
void draw_reality_not_map(Adafruit_ILI9341& tft, vector<Coordinate>& not_map_grid, uint16_t color){
    for (Coordinate pos : not_map_grid){
        int x_coord = (10 + (pos.x-1)*20);
        int y_coord = (221 - pos.y*20);
        tft.drawLine(x_coord + 10, y_coord, x_coord, y_coord + 10, color);
        //tft.drawLine(x_coord + 20, y_coord, x_coord, y_coord + 20, WHITE);
        tft.drawLine(x_coord + 20, y_coord + 10, x_coord + 10, y_coord + 20, color);
        
    }
}

//Xử lý hiển thị người dùng
void user_handle(Adafruit_ILI9341& tft, User_data &user_data, float north_offset, IMU_Data &imu_data) {
    if (user_data.user_x == 0.0f && user_data.user_y == 0.0f) return;
    int user_coord_x, user_coord_y;
    coordinate_to_pixel(user_data.user_x, user_data.user_y, user_coord_x, user_coord_y);

    int vision_range = 28;
    float half_cone_angle = 30 * (PI / 180.0f);  // 30 độ
    float user_yaw_angle = (imu_data.euler.x - north_offset) * (PI / 180.0f);

    // Vẽ người dùng
    tft.fillCircle(user_coord_x, user_coord_y, 5, BLUE);

    // Vẽ tầm nhìn dạng hình quạt (nón)
    for (float a = -half_cone_angle; a <= half_cone_angle; a += PI / 180.0f) {
        float angle = user_yaw_angle + a;
        int x_edge = user_coord_x + round(vision_range * sin(angle));
        int y_edge = user_coord_y - round(vision_range * cos(angle));
        tft.drawLine(user_coord_x, user_coord_y, x_edge, y_edge, GREEN);
    }
}

//Xử lý hiển thị ngọn lửa
void fire_handle(Adafruit_ILI9341& tft, Fire fire){
    for (int i = 0; i < 99; i++){
        //Duyệt qua các ngọn lửa
        if (fire.fire_data[i].fire_id <= 0 || fire.fire_data[i].fire_x <= 0 || fire.fire_data[i].fire_y <= 0 || fire.fire_data[i].fire_lvl > 5) continue;
        //Đổi tọa độ server sang tọa độ pixel
        int fire_x_coord;
        int fire_y_coord;
        coordinate_to_pixel(fire.fire_data[i].fire_x, fire.fire_data[i].fire_y, fire_x_coord, fire_y_coord);
        //Vẽ lửa trên map theo lvl
        tft.fillRect(fire_x_coord - 9, fire_y_coord - 9 , 18, 18, fire_color[fire.fire_data[i].fire_lvl]);
    }
}

//Xóa hiển thị người dùng tại tọa độ (x, y) phía server
void user_display_delete(Adafruit_ILI9341& tft, User_data &user_data) {
    if (user_data.user_x == 0.0f && user_data.user_y== 0.0f) return;

    int vision_range = 28;
    int user_coord_x, user_coord_y;
    // Đổi tọa độ sever về tọa độ tft
    coordinate_to_pixel(user_data.user_x, user_data.user_y, user_coord_x, user_coord_y);
    // Xóa toàn bộ vùng người dùng + tầm nhìn (bằng hình tròn màu đen)
    tft.fillCircle(user_coord_x, user_coord_y, vision_range + 1, BLACK);
}


//Xóa hiển thị ngọn lửa
void fire_display_delete(Adafruit_ILI9341& tft, Fire fire){
    //Ngọn lửa
    for (int i = 0; i < 99; i++){
        //Duyệt qua các ngọn lửa
        if (fire.fire_data[i].fire_id < 0 || fire.fire_data[i].fire_x <= 0 || fire.fire_data[i].fire_y <= 0 || fire.fire_data[i].fire_lvl > 5) continue;
        //Đổi tọa độ server sang tọa độ pixel
        int fire_x_coord;
        int fire_y_coord;
        coordinate_to_pixel(fire.fire_data[i].fire_x, fire.fire_data[i].fire_y, fire_x_coord, fire_y_coord);
        //Vẽ lửa trên map theo lvl
        tft.fillRect(fire_x_coord - 9, fire_y_coord - 9, 18, 18, BLACK);
    }
}