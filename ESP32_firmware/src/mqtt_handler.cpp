#include "mqtt_handler.h"



/*#############################################################################################################*/
// Global variables initialization
/*#############################################################################################################*/

WiFiClient wifi_client;
PubSubClient mqtt_client(wifi_client);

unsigned long last_publish_time = 0;



/*#############################################################################################################*/
/**
 * @brief Connect to WiFi network
 * @return Connection status
 * 
 * Attempts to connect to WiFi network using credentials from config.h
 * Times out after WIFI_TIMEOUT milliseconds
 */
/*#############################################################################################################*/

bool connect_wifi() {
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    unsigned long start_time = millis();
    
    while (WiFi.status() != WL_CONNECTED) {
        if (millis() - start_time > WIFI_TIMEOUT) {
            return false;
        }
        delay(100);
    }
    return true;
}



/*#############################################################################################################*/
/**
 * @brief Connect to MQTT broker
 * @return Connection status
 * 
 * Sets up MQTT broker connection using credentials from config.h
 * Attempts reconnection if connection lost
 */
/*#############################################################################################################*/

bool connect_mqtt() {
    mqtt_client.setServer(MQTT_BROKER, MQTT_PORT);
    
    if (!mqtt_client.connected()) {
        return mqtt_client.connect(MQTT_CLIENT_ID);
    }
    return true;
}



/*#############################################################################################################*/
/**
 * @brief Publish MQTT message
 * @param topic Target MQTT topic
 * @param message Message content
 * @return Publish status
 * 
 * Publishes message to specified MQTT topic
 * Attempts reconnection if connection lost
 */
/*#############################################################################################################*/

bool publish_message(String topic, String message) {
    if (!mqtt_client.connected()) {
        if (!connect_mqtt()) {
            return false;
        }
    }
    return mqtt_client.publish(topic.c_str(), message.c_str());
}



/*#############################################################################################################*/
/**
 * @brief Check if enough time has passed since last publish
 * @return true if ready to publish new message
 * 
 * Controls message publishing rate using PUBLISH_INTERVAL from config
 */
/*#############################################################################################################*/

bool is_ready_to_publish() {
    unsigned long current_time = millis();
    if (current_time - last_publish_time >= PUBLISH_INTERVAL) 
    {
        last_publish_time = current_time;
        return true;
    }
    return false;
}

void mqtt_callback(char* topic, byte* payload, unsigned int length){
    Serial.println("received");
    //Chuẩn hóa message
    char message[length + 1];
    memcpy(message, payload, length);
    message[length] = '\0';
    // Gọi hàm callback riêng xử lý với từng topic
    if (strcmp(topic, "real_data_id_14/map_data") == 0) {
        handle_map_data_topic(message);
    }
    else if(strcmp(topic, "real_data_id_14/user_data") == 0){
        handle_user_data_topic(message);
    }
    else if(strcmp(topic, "real_data_id_14/fire_data") == 0){
        handle_fire_data_topic(message);
    }
}

//Tách dữ liệu về ID các ô trong map
void parse_not_passable_IDs(const char* payload) {
    String s = String(payload);
    int index = 0;
    int element_count = 0;

    while(index >= 0){
        int commaIndex = s.indexOf(',', index);
        String token;
        if (commaIndex == -1){
            token = s.substring(index);
            index = -1;
        }else{
            token = s.substring(index, commaIndex);
            index = commaIndex + 1;
        }
        //Phần tử đầu là góc north_offset
        if(element_count == 0){
            north_offset = token.toFloat();  
        }else{
            not_passable_map_id.push_back(token.toInt()); 
        }
        element_count++;
    }
}


//Callback xử lý khi có tin nhắn tới real_data_id_14/map_data
void handle_map_data_topic(const char* payload){
    //Lưu lại layout map cũ
    last_map_grid = map_grid;
    last_not_map_grid = not_map_grid;

    //Xóa các vector chứa thông tin map hiện tại
    passable_map_id.clear();
    not_passable_map_id.clear();
    map_grid.clear();
    not_map_grid.clear();

    //Parse message từ mqtt về thông tin map
    parse_not_passable_IDs(payload);

    //Cập nhật vector passable_map_id
    bool appeared[101];
    // Khởi tạo tất cả các ô là đi được (true)
    for(int i = 0; i <= 100; i++) {
        appeared[i] = true;
    }
    
    //Đánh dấu các ô không đi được là false
    for (int id_num : not_passable_map_id) {
        if (id_num >= 1 && id_num <= 100) {
            appeared[id_num] = false;
        }
    }
    //Vector not_passable_map_id chứa các ô không đi được
    for (int i = 1; i <= 100; ++i) {
        if (appeared[i]) {
            passable_map_id.push_back(i);
        }
    }

    //Cập nhật vector chứa thông tin map
    id_to_coordinate(passable_map_id, &map_grid);
    id_to_coordinate(not_passable_map_id, &not_map_grid);
    
    //Cập nhật cờ để sửa map
    Serial.println("Map recieve");
    map_updated = true;
}


//Callback xử lý khi có tin nhắn tới real_data_id_14/user_data
void handle_user_data_topic(const char* payload){
    last_user_data.user_x = user_data.user_x;
    last_user_data.user_y = user_data.user_y;
    last_user_data.user_score = user_data.user_score;

    String data = String(payload);
    int commaIndex1 = data.indexOf(',');
    int commaIndex2 = data.indexOf(',', commaIndex1 + 1);

    if (commaIndex1 != -1 && commaIndex2 != -1) {
        String x_str = data.substring(0, commaIndex1);
        String y_str = data.substring(commaIndex1 + 1, commaIndex2);
        String score_str = data.substring(commaIndex2 + 1);

        user_data.user_x = x_str.toFloat();
        user_data.user_y = y_str.toFloat();
        user_data.user_score = score_str.toInt();
    }
    //Cờ user_updated
    Serial.println("User recieve");
    user_updated = true;
}


//Callback xử lý khi có tin nhắn tới real_data_id_14/fire_data
void handle_fire_data_topic(const char* payload){
    vector<String> tokens;
    String msg(payload);
    int start = 0, end;

    last_fire = fire;
    // Xóa dữ liệu cũ
    for(int i = 0; i < 99; i++) {
        fire.fire_data[i].fire_id = -1; 
        fire.fire_data[i].fire_x = 0;
        fire.fire_data[i].fire_y = 0;
        fire.fire_data[i].fire_lvl = 0;
    }

    // Tách chuỗi thành vector chứa các thành phần
    while ((end = msg.indexOf(',', start)) != -1) {
        tokens.push_back(msg.substring(start, end));
        start = end + 1;
    }
    tokens.push_back(msg.substring(start)); 

    // Lặp qua từng cụm 4 phần tử
    for (size_t i = 0; i < tokens.size(); i += 4) {
        int id = tokens[i].toInt();
        float x = tokens[i + 1].toFloat();
        float y = tokens[i + 2].toFloat();
        int lvl = tokens[i + 3].toInt();

        if (id < 0 || id >= 99) continue;
        //Lưu data liên quan tới ngọn lửa
        fire.fire_data[id].fire_id = id;
        fire.fire_data[id].fire_x = x;
        fire.fire_data[id].fire_y = y;
        fire.fire_data[id].fire_lvl = lvl;
    }
    //Cờ fire_updated
    Serial.println("Fire recieve");
    fire_updated = true;
}
