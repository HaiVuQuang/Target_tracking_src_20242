#ifndef BUTTON_HANDLER_H
#define BUTTON_HANDLER_H



// Libraries

#include <Arduino.h>



#include "config.h"



// ButtonHandler class
/*#############################################################################################################*/

class ButtonHandler {
private:
    bool last_counter_state;            // Last state of Counter button
    bool is_counter_held;               // Flag to detect Counter button hold
    unsigned long last_debounce_time;   // Last time button was pressed
    int training_counter;               // Counter value in Training mode
    
    // Private methods
    bool debounce_read(uint8_t pin, bool& last_state);
    
public:
    ButtonHandler();
    void init();
    
    // System mode
    bool is_training_mode();       // Check if system is in Training mode
    
    // Send/Reset button
    bool is_send_enable();        // Get Send button state
    
    // Training mode
    bool is_counter_pressed();     // Detect Counter button press (Training only)
    
    // Reality mode
    int get_valve_status();       // Read valve state (0/1)
    int get_mode_status();        // Read mode state (0/1)
};

/*#############################################################################################################*/



#endif