#ifndef CONFIG_H
#define CONFIG_H

#include <vector>

#include <Arduino.h>
#include <stdio.h>
#include <Wire.h>
#include <SPI.h>
#include <wifi.h>
#include <PubSubClient.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

// Device ID and mode of system
/*#############################################################################################################*/

// Device ID
#define DEVICE_ID 14 // Device ID for MQTT

// Device Name
#define DEVICE_NAME "Fire steering"

// Mode of system
#define DEFAULT_MODE 0 // Default mode of system (Sending Information, Training/Reality Topic)
#define TRAINING_MODE 1 // Training mode (Sending data and counter)
#define REALITY_MODE 2 // Reality mode (Sending data and valve/mode of extinguish status)

//Link youtube for turtorial
#define YOUTUBE_LINK "https://www.youtube.com/watch?v=uHgt8giw1LY"

//QR code 
/*To convert QR code to array in C++, run "C:\Users\Admin\Project\other\qr_code_to_cpp_byte.py" 
with link of .png QR code file and copy the output to here*/
#define QR_SIZE 31
const uint8_t TUTORIAL_QR[QR_SIZE][QR_SIZE] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0},
    {0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0},
    {0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0},
    {0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0},
    {0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 0},
    {0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0},
    {0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0},
    {0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 0},
    {0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0},
    {0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0},
    {0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 1, 1, 0, 0},
    {0, 1, 1, 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 1, 0},
    {0, 1, 0, 0, 1, 1, 0, 1, 0, 1, 1, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 0, 0, 0, 0, 1, 1, 0},
    {0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0},
    {0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 1, 0},
    {0, 1, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0},
    {0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0},
    {0, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0, 1, 0, 0, 1, 1, 1, 0, 1, 0, 0, 0},
    {0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0},
    {0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0},
    {0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 1, 1, 0, 1, 0, 0},
    {0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0},
    {0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0},
    {0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0},
    {0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
}; 




// Debug Configuration
/*#############################################################################################################*/

// Serial Monitor Settings
#define BAUD_RATE_SERIAL 115200      // Serial monitor baud rate (bps)
#define DEBUG_INTERVAL 100           // Debug print interval (ms)

/*#############################################################################################################*/



// UART Configuration for Raspberry Pi Pico W
/*#############################################################################################################*/

// UART Pin Definitions
#define UART_TX_PIN_PI_PICO 17     // TX pin (GPIO1)
#define UART_RX_PIN_PI_PICO 16     // RX pin (GPIO3)

// UART Communication Settings  
#define UART_ID_PI_PICO 1          // UART0 peripheral
#define BAUD_RATE_PI_PICO 115200    // Must match Pi Pico's baud rate

// UART Buffer Size
#define UART_RX_BUFFER_SIZE 256     // Size of UART receive buffer

/*#############################################################################################################*/



// I2C Configuration for reading data from BNO055, and BNO055 setup
/*#############################################################################################################*/

// BNO055 I2C Configuration
#define I2C_SDA 21                     // Default SDA pin for ESP32
#define I2C_SCL 22                     // Default SCL pin for ESP32
#define BNO055_ADDRESS 0x29           // Default I2C address for BNO055

// BNO055 Register Addresses
#define ACC_DATA_START 0x28            // Accelerometer data registers 0x08-0x0D
#define MAG_DATA_START 0x0E            // Magnetometer data registers  
#define GYRO_DATA_START 0x14           // Gyroscope data registers
#define EULER_DATA_START 0x1A          // Euler angles data registers
#define QUATERNION_DATA_START 0x20     // Quaternion data registers

// BNO055 Operation Mode Registers
#define BNO055_OPR_MODE_ADDR 0x3D      // Operation mode register address
#define BNO055_MODE_CONFIG 0x00        // Configuration mode
#define BNO055_MODE_NDOF 0x0C          // Nine Degrees of Freedom fusion mode

// BNO055 Operation Delays
#define BNO055_CONFIG_DELAY 100        // Delay after changing modes (ms)

// BNO055 Calibration Status Register
#define BNO055_CALIB_STAT_ADDR 0x35    // Calibration status register
#define BNO055_CALIB_STAT_MASK 0xFF    // Mask for all calibration status bits

// BNO055 System Status Register  
#define BNO055_SYS_STATUS_ADDR 0x39    // System status register
#define BNO055_SYS_STATUS_RUNNING 0x05 // System status: running normally

// BNO055 Error Status Register
#define BNO055_SYS_ERR_ADDR 0x3A       // System error status register

/*#############################################################################################################*/



// SPI Configuration for LCD ILI9341, and LCD setup
/*#############################################################################################################*/

//Touch (Not use yet)
#define T_IRQ 0
#define T_DO  0
#define T_DIN 0
#define T_CS  0
#define T_CLK 0

//SPI show screen
#define TFT_MISO    19  // SDO - MASTER IN SLAVE OUT (MISO) : Not used, as TFT is write-only
#define TFT_LED     0   // LED : Control LCD backlight connects to 3.3V (could use PWM)
#define TFT_SCK     18  // SCK - CLK : Serial Clock (IMPORTANT)
#define TFT_MOSI    23  // SDI - MASTER OUT SLAVE IN (MOSI) : Serial Data Input (IMPORTANT)
#define TFT_DC      2   // DC : Data/Command control pin (IMPORTANT)
#define TFT_RST     4   // RST - RESET : Reset pin (IMPORTANT)
#define TFT_CS      5   // CS : Chip Select (IMPORTANT)

//LCD setup
#define INTRO_TIME                         5000    //LCD intro time
#define LCD_ROTATION                       1       //LCD rotation
#define LCD_REFRESH_RATE_TRAINING_MODE     10      //LCD refresh rate for training mode (Hz)
#define LCD_REFRESH_RATE_REALITY_MODE      10      //LCD refresh rate for reality mode (Hz)

/*#############################################################################################################*/



// Button Configuration
/*#############################################################################################################*/

// Button Pins
#define MODE_SWITCH_PIN     32    // Training/Reality mode switch
#define SEND_ENABLE_PIN     33    // Enable message sending and reset in training mode | Switch send infor/data in reality mode
#define COUNTER_BUTTON_PIN  25    // Counter increment button for training mode only

// Analog Input Pins
#define VALVE_PIN         34      // Valve control analog input in reality mode
#define MODE_PIN          35      // Mode control analog input in reality mode

// Button Debounce
#define DEBOUNCE_TIME      100     // Debounce time in milliseconds

// System Modes
#define MODE_REALITY       0      // Reality mode
#define MODE_TRAINING      1      // Training mode
 
// Analog Thresholds
#define ANALOG_THRESHOLD  2048    // Threshold for analog reading (4096/2)

/*#############################################################################################################*/



// WiFi and MQTT Configuration
/*#############################################################################################################*/

// WiFi Settings
#define WIFI_SSID       "RSSI1"
#define WIFI_PASSWORD   "11111111"
#define WIFI_TIMEOUT    10000   // Connection timeout (ms)

// MQTT Broker Settings
#define MQTT_BROKER     "192.168.0.101"
#define MQTT_PORT       1883    // Default MQTT port
#define MQTT_USERNAME   ""
#define MQTT_PASSWORD   ""
#define MQTT_CLIENT_ID  "ESP32_CLIENT_ID_14"

// MQTT Topics
#define MQTT_DEVICE_INFOR_TOPIC "device_info"
#define MQTT_TRAINING_TOPIC     "training_data_id_" + String(DEVICE_ID)
#define MQTT_REALITY_TOPIC      "real_data_id_" + String(DEVICE_ID)

// Message Publishing Configuration
#define PUBLISH_INTERVAL 20    // Minimum time between messages (ms)

/*#############################################################################################################*/

using namespace std;

// RSSI_Data defines
/*#############################################################################################################*/

// RSSI_Data struct
struct RSSI_Data
{
    String ssid;
    int ssid_id;
    int rssi;
    unsigned long last_receive_time;
};
// RSSI_Timeout defines
#define RSSI_Timeout 5000 // RSSI timeout (ms)

/*#############################################################################################################*/



// IMU_Data defines
/*#############################################################################################################*/

//Quarternion struct
struct Raw_quaternion
{
    int16_t w, x, y, z;
};
struct Real_quaternion
{
    float w, x, y, z;
};

//Vector3 struct
struct Raw_vector3
{
    int16_t x, y, z;
};
struct Real_vector3
{
    float x, y, z;
};

//IMU raw data defines
struct IMU_Raw_Data
{
    Raw_vector3 acc_raw;
    Raw_vector3 mag_raw;
    Raw_vector3 gyro_raw;
    Raw_vector3 euler_raw;
    Raw_quaternion quaternion_raw;
};

//IMU real local data defines
struct IMU_Real_local_Data
{
    Real_vector3 acc_local_real;
    Real_vector3 mag_local_real;
    Real_vector3 gyro_local_real;
    Real_vector3 euler_real;
    Real_quaternion quaternion_real;
};

//IMU real global data defines
struct IMU_Data
{
    Real_vector3 gyro;
    Real_vector3 mag;
    Real_vector3 acc;
    Real_vector3 euler;
};

/*#############################################################################################################*/



// Colour define for LCD
/*#############################################################################################################*/

#define BLUE        0x001F
#define WHITE       0xFFFF
#define BLACK       0x0000
#define GREEN       0x07E0
#define YELLOW      0xFFE0
#define ORANGE      0xFD20
#define RED         0xF800
#define DARK_RED    0x8800

/*#############################################################################################################*/

// LCD algorithms variables define
/*#############################################################################################################*/

//ID Các ô đi được trong bản đồ (được gửi về từ trainer)
extern vector<int> passable_map_id;
extern vector<int> not_passable_map_id;

//Tọa độ xử lý map_grid
struct Coordinate{
    int x, y;
};

//Tọa độ các ô đi được/không đi được trong map
extern vector<Coordinate> map_grid;
extern vector<Coordinate> not_map_grid;
extern vector<Coordinate> last_map_grid;
extern vector<Coordinate> last_not_map_grid;
extern bool map_updated;

// Góc offset giữa cực Bắc thực tế và cực bắc của map 
extern float north_offset;

// Vị trí người dùng
struct User_data{
    float user_x;
    float user_y;
    int user_score;
    int user_speed;
};
extern bool user_updated;

//Ngọn lửa 
struct Fire_properties{
    int fire_id;
    float fire_x;
    float fire_y;
    int fire_lvl;
};
struct Fire{
    Fire_properties fire_data[99];
};
extern bool fire_updated;

//Màu hiển thị ngọn lửa theo mức lvl 
const uint16_t fire_color[6] = {
    BLACK,          //lvl0       
    0xF800,          //lvl1
    0xFFF0,         //lvl2
    0xFD04,         //lvl3    
    0xA984,            //lvl4  
    0xA804        //lvl5                   
};
/*#############################################################################################################*/

// Variables for valve in reality mode 
extern int valve_open_status;
extern int mode_status;



#endif 