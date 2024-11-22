#ifndef ALARM_HPP
#define ALARM_HPP

#include "sensor_data.hpp"

// Thresholds for alarm
struct Thresholds {
    float temp_upper = 30.0f;    // Upper threshold for temperature
    float temp_lower = 10.0f;    // Lower threshold for temperature
    float press_upper = 1020.0f; // Upper threshold for pressure
    float press_lower = 1000.0f; // Lower threshold for pressure
    float light_upper = 0.8f;    // Upper threshold for light
    float light_lower = 0.2f;    // Lower threshold for light
};

void monitor_sensors(SensorData& sensor_data, Thresholds& thresholds);

#endif // ALARM_HPP
