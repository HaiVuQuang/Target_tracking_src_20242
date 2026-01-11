#include "button_handler.h"



/*#############################################################################################################*/
/**
 * @brief Constructor for ButtonHandler class
 * Initializes button states and counter values
 */
/*#############################################################################################################*/

ButtonHandler::ButtonHandler() {
    last_counter_state = HIGH;
    is_counter_held = false;
    last_debounce_time = 0;
}



/*#############################################################################################################*/
/**
 * @brief Initialize GPIO pins for buttons and analog inputs
 */
/*#############################################################################################################*/

void ButtonHandler::init() {
    pinMode(MODE_SWITCH_PIN, INPUT_PULLUP);
    pinMode(SEND_ENABLE_PIN, INPUT_PULLUP);
    pinMode(COUNTER_BUTTON_PIN, INPUT_PULLUP);
    pinMode(VALVE_PIN, INPUT);
    pinMode(MODE_PIN, INPUT);
}



/*#############################################################################################################*/
/**
 * @brief Debounce button reading
 * @param pin GPIO pin to read
 * @param last_state Reference to last recorded state
 * @return true if button press detected
 */
/*#############################################################################################################*/

bool ButtonHandler::debounce_read(uint8_t pin, bool& last_state) {
    bool current_state = digitalRead(pin);
    unsigned long current_time = millis();
    
    if (current_time - last_debounce_time > DEBOUNCE_TIME) {
        if (current_state != last_state) {
            last_debounce_time = current_time;
            last_state = current_state;
            return (current_state == LOW);
        }
    }
    return false;
}



/*#############################################################################################################*/
/**
 * @brief Check if system is in training mode
 * @return true if in training mode
 */
/*#############################################################################################################*/

bool ButtonHandler::is_training_mode() {
    return !digitalRead(MODE_SWITCH_PIN);
}



/*#############################################################################################################*/
/**
 * @brief Get send enable button state
 * @return true if send is enabled
 */
/*#############################################################################################################*/

bool ButtonHandler::is_send_enable() {
    return !digitalRead(SEND_ENABLE_PIN);
}



/*#############################################################################################################*/
/**
 * @brief Check if counter button is pressed with rising edge detection
 * @return true if counter pressed and not held
 * 
 * This function uses debounce_read to detect button press and prevents multiple counts 
 * when button is held down.
 */
/*#############################################################################################################*/

bool ButtonHandler::is_counter_pressed() {
    bool is_pressed = debounce_read(COUNTER_BUTTON_PIN, last_counter_state);
    
    if (is_pressed && !is_counter_held) {
        is_counter_held = true;
        return true;
    }
    
    if (!is_pressed) {
        is_counter_held = false;
    }
    
    return false;
}



/*#############################################################################################################*/
/**
 * @brief Get valve status from analog input
 * @return true if valve value above threshold
 */
/*#############################################################################################################*/

int ButtonHandler::get_valve_status() {
    return analogRead(VALVE_PIN) ;
}



/*#############################################################################################################*/
/**
 * @brief Get mode status from analog input
 * @return true if mode value above threshold
 */
/*#############################################################################################################*/

int ButtonHandler::get_mode_status() {
    return analogRead(MODE_PIN);
}