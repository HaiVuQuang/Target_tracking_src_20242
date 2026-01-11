#ifndef MQTT_HANDLER_H
#define MQTT_HANDLER_H


// Libraries
#include <Arduino.h>
#include <WiFi.h>



#include <PubSubClient.h>



#include "config.h"
#include "lcd_algorithm.h"

// Global variables

extern WiFiClient wifi_client;
extern PubSubClient mqtt_client;

extern unsigned long last_publish_time;
extern Fire fire;
extern Fire last_fire;
extern User_data user_data;
extern User_data last_user_data;
// extern Last_user_server_location last_user_server_location;
extern float north_offset;


// Function declarations

/*#############################################################################################################*/
/**
 * @brief Connect to WiFi network using credentials from config
 * @return true if connection successful
 */
/*#############################################################################################################*/
bool connect_wifi();



/*#############################################################################################################*/
/**
 * @brief Connect to MQTT broker using credentials from config
 * @return true if connection successful
 */
/*#############################################################################################################*/
bool connect_mqtt();



/*#############################################################################################################*/
/**
 * @brief Publish message to MQTT topic
 * @param topic MQTT topic to publish to
 * @param message Message content to publish
 * @return true if publish successful
 */
/*#############################################################################################################*/
bool publish_message(String topic, String message);



/*#############################################################################################################*/
/**
 * @brief Check if enough time has passed since last publish
 * @return true if ready to publish new message
 * 
 * Controls message publishing rate using PUBLISH_INTERVAL from config
 */
/*#############################################################################################################*/
bool is_ready_to_publish();






void mqtt_callback(char* topic, byte* payload, unsigned int length);

void parse_not_passable_IDs(const char* payload);

void handle_map_data_topic(const char* payload);

void handle_user_data_topic(const char* payload);

void handle_fire_data_topic(const char* payload);


#endif