#include "valve_data_handle.h"


void read_valve_open_status(int *valve_open_status){
    int raw_value = analogRead(VALVE_PIN);
    *valve_open_status = int((raw_value / 700.0) * 100);
}