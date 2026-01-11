#include <Arduino.h>
#include <stdio.h>
#include <Wire.h>
#include <SPI.h>



#include <wifi.h>
#include <PubSubClient.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>



#include "button_handler.h"
#include "create_msg.h"
#include "config.h"
#include "debug.h"
#include "lcd_algorithm.h"
#include "lcd_control.h"
#include "lcd_outline.h"
#include "mqtt_handler.h"
#include "read_pi_pico_data.h"
#include "read_imu_data.h"
#include "valve_data_handle.h"



// Set up Hardware Serial
/*#############################################################################################################*/

// Set up Hardware Serial for UART communication with Raspberry Pi Pico W
HardwareSerial uart_pi_pico(UART_ID_PI_PICO);

// Initialize button handler
ButtonHandler button_handler;

// Initialize Adafruit ILI9341 TFT display
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

/*#############################################################################################################*/
extern WiFiClient wifi_client;
extern PubSubClient mqtt_client;


// Variables declaration
/*#############################################################################################################*/

// Variable for system mode
int mode_of_system = DEFAULT_MODE;
bool change_mode = false;

// Variables for debugging
unsigned long last_debug_time = 0;

// Topic for MQTT
String device_info_topic = MQTT_DEVICE_INFOR_TOPIC;
String training_topic = MQTT_TRAINING_TOPIC;
String real_topic = MQTT_REALITY_TOPIC;
String map_data_topic = MQTT_REALITY_TOPIC + "/map_data";
String user_data_topic = MQTT_REALITY_TOPIC + "/user_data";
String fire_data_topic = MQTT_REALITY_TOPIC + "/fire_data";

// Message for MQTT
String message;
// Variables for saving BNO055 data
IMU_Data imu_data;
IMU_Raw_Data imu_raw_data;
IMU_Real_local_Data imu_real_local_data;
// Variable for training mode
int training_counter = 0;

// Variables for valve in reality mode 
int valve_open_status = 0;                    
int mode_status = 0;

//Vector lưu thông tin ngọn lửa
Fire fire;
Fire last_fire;

// Vector for saving map 
vector<int> passable_map_id;
vector<int> not_passable_map_id;
//Vector lưu thông tin map
vector<Coordinate> map_grid;
vector<Coordinate> not_map_grid;
vector<Coordinate> last_map_grid;
vector<Coordinate> last_not_map_grid;

//Flag check update
bool map_updated = false;
bool user_updated = false;
bool fire_updated = false;

// Góc offset giữa cực Bắc thực tế và cực bắc của map 
float north_offset = 0;

// Dữ liệu liên quan tới người dùng
User_data user_data;
User_data last_user_data;
// Variables for saving RSSI data
RSSI_Data rssi_data[4] = {
    {"RSSI1", 1, -100, 0},
    {"RSSI2", 2, -100, 0},
    {"RSSI3", 3, -100, 0},
    {"RSSI4", 4, -100, 0}};


// Variables for reality mode
// int valve_status = 0;


/*#############################################################################################################*/

unsigned long time_millis;

void setup()
{

    // Set up Hardware Serial communication
    /*###########################################################################################################*/

    // Set up Serial Monitor
    Serial.begin(BAUD_RATE_SERIAL);

    // Set up UART communication with Raspberry Pi Pico W
    uart_pi_pico.begin(BAUD_RATE_PI_PICO, SERIAL_8N1, UART_RX_PIN_PI_PICO, UART_TX_PIN_PI_PICO);

    // Set up I2C communication with BNO055
    Wire.begin(I2C_SDA, I2C_SCL);

    // Configure BNO055
    Wire.beginTransmission(BNO055_ADDRESS);
    Wire.write(BNO055_OPR_MODE_ADDR); // Select operation mode register
    Wire.write(BNO055_MODE_NDOF);     // Set NDOF fusion mode
    Wire.endTransmission();
    delay(BNO055_CONFIG_DELAY); // Wait for mode switch

    // Initialize 
    //valve_status = button_handler.get_valve_status();
    mode_status = button_handler.get_mode_status();

    // Initialize button handler
    button_handler.init();

    /*###########################################################################################################*/
    


    // Set up TFT display
    /*###########################################################################################################*/
    
    tft.begin();
    tft.setRotation(LCD_ROTATION);
    lcd_setup_intro(tft);

    /*###########################################################################################################*/



    // Set up WiFi and MQTT connection
    /*###########################################################################################################*/

    if (!connect_wifi())
    {
        Serial.println("WiFi connection failed!");
        return;
    }

    if (!connect_mqtt())
    {
        Serial.println("MQTT connection failed!");
        return;
    }

    /*###########################################################################################################*/
    mqtt_client.subscribe(map_data_topic.c_str());
    mqtt_client.subscribe(user_data_topic.c_str());
    mqtt_client.subscribe(fire_data_topic.c_str());
    mqtt_client.setCallback(mqtt_callback);
    
    lcd_setup_outline(tft);
   
    Serial.print("set up complete");

}



void loop()
{
    mqtt_client.loop();
    // Handle system mode
    /*#########################################################################################################*/
    if (button_handler.is_training_mode())
    {
        // Training mode
        if (button_handler.is_send_enable())
        {
            // Create and send training message with counter
            if (button_handler.is_counter_pressed())
            {
                training_counter++; // Increment k when counter pressed
            }
            if (mode_of_system != TRAINING_MODE)
            {
                mode_of_system = TRAINING_MODE;
                change_mode = true;
            }
        }
        else
        {
            if (mode_of_system != DEFAULT_MODE)
            {
                mode_of_system = DEFAULT_MODE;
                change_mode = true;
            }
            training_counter = 0; // Reset counter when send is disabled
        }
    }
    else
    {
        // Reality mode
        if (button_handler.is_send_enable())
        {
            if (button_handler.is_counter_pressed())
            {
                training_counter++; // Increment k when counter pressed
            }
            if (mode_of_system != REALITY_MODE)
            {
                mode_of_system = REALITY_MODE;
                change_mode = true;
            }
        }
        else
        {
            if (mode_of_system != DEFAULT_MODE)
            {
                mode_of_system = DEFAULT_MODE;
                change_mode = true;
            }   
        }
    }

    /*#########################################################################################################*/



    // Read Data
    /*#########################################################################################################*/

    // Read RSSI values from Raspberry Pi Pico W
    if (uart_pi_pico.available() > 0)
    {
        read_Pi_Pico_data(uart_pi_pico, rssi_data);
    }
    // Check last receive time of RSSI values
  //check_last_receive_time(rssi_data);

    // Read IMU data from BNO055
    read_IMU_data(&imu_data);
    /*#########################################################################################################*/



    // Create and send MQTT message
    /*#########################################################################################################*/

    if (mode_of_system == DEFAULT_MODE)
    {
        if (change_mode)
        {
            change_mode = false;
            lcd_setup_outline(tft);
        }
        get_infor_msg(training_topic, real_topic, DEVICE_ID, &message);
        if (is_ready_to_publish())
        {
            publish_message(device_info_topic, message);
        }
        lcd_display_default_mode(tft, rssi_data);
    }
    else if (mode_of_system == TRAINING_MODE)
    {
        if (change_mode)
        {
            change_mode = false;
            lcd_setup_training_mode_outline(tft);
        }
        get_training_msg(rssi_data, imu_data, training_counter, &message);
        if (is_ready_to_publish())
        {
            publish_message(training_topic, message);
        }
        if (lcd_is_ready_to_refresh_training_mode())
        {
            lcd_display_training_mode(tft, rssi_data, imu_data, training_counter);
        }
    }
    else if (mode_of_system == REALITY_MODE)
    {
        time_millis = millis();
        read_valve_open_status(&valve_open_status);
        get_real_msg(rssi_data, imu_data, valve_open_status, mode_status, &message, time_millis);
        if (is_ready_to_publish())
        {
            publish_message(training_topic, message);
        }
        if (change_mode)
        {
            change_mode = false;
            tft.fillScreen(BLACK);
        }
            // lcd_setup_outline(tft);
            read_valve_open_status(&valve_open_status);
            // Vẽ map ở chế độ Reality
            // Xóa hiển thị người dùng và ngọn lửa
            if(fire_updated){
                fire_display_delete(tft, last_fire);
                fire_updated = false;
            }
            
            if(user_updated){
                user_display_delete(tft, last_user_data);
                user_updated = false;
            }else{
                user_display_delete(tft, user_data);
            }

            //Check map_updated, vẽ lại layout map
            if (map_updated) {
                draw_reality_map(tft, last_map_grid, BLACK);
                draw_reality_not_map(tft, last_not_map_grid, BLACK);
                draw_reality_map(tft, map_grid, WHITE);
                draw_reality_not_map(tft, not_map_grid,BLUE);
                lcd_setup_reality_map_outline(tft);
                map_updated = false;
            } else{
                draw_reality_map(tft, map_grid, WHITE);
                draw_reality_not_map(tft, not_map_grid,BLUE);
                lcd_setup_reality_map_outline(tft);
            }
            //Hiện thị ngọn lửa
            fire_handle(tft,fire);
            //Hiển thị người dùng
            user_handle(tft, user_data, north_offset, imu_data);
            //Hiển thị thông tin
            lcd_setup_reality_text_outline(tft);
            lcd_update_reality_text_outline(tft, valve_open_status, user_data);
    }

    /*#########################################################################################################*/
    


    // Serial Monitor for debugging
    /*#########################################################################################################*/

    unsigned long currentMillis = millis();
    if (currentMillis - last_debug_time >= DEBUG_INTERVAL)
    {
        last_debug_time = currentMillis;
        // print_RSSI_data(rssi_data, 4);
        // print_IMU_data(imu_data);
        // print_topic_and_message(message, mode_of_system);
    }

    /*#########################################################################################################*/

}