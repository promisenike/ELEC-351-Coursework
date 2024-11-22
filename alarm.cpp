#include "alarm.hpp"
#include "uop_msb.h"
#include <cstdio>

void monitor_sensors(SensorData& sensor_data, Thresholds& thresholds) {
    bool alarm_active = false;
    while (true) {
        // Print the samples to the terminal
        printf("\n----- Sample -----\n");
        printf("Temperature: %.1f°C\n", sensor_data.temperature);
        printf("Pressure: %.1f mbar\n", sensor_data.pressure);
        printf("Light Level: %.2f\n", sensor_data.light_level);

        // Handle alarm based on thresholds with hysteresis
        if (sensor_data.temperature > thresholds.temp_upper) {
            alarm_active = true;
            buzz.playTone("C");
        } else if (sensor_data.temperature < thresholds.temp_lower) {
            alarm_active = true;
            buzz.playTone("C");
        } else if (sensor_data.pressure > thresholds.press_upper) {
            alarm_active = true;
            buzz.playTone("C");
        } else if (sensor_data.pressure < thresholds.press_lower) {
            alarm_active = true;
            buzz.playTone("C");
        } else if (sensor_data.light_level > thresholds.light_upper) {
            alarm_active = true;
            buzz.playTone("C");
        } else if (sensor_data.light_level < thresholds.light_lower) {
            alarm_active = true;
            buzz.playTone("C");
        } else {
            alarm_active = false;
            buzz.rest();
        }

        // LED settings based on alarm status
        if (alarm_active) {
            latchedLEDs.write_strip(0xFF, RED);
            latchedLEDs.write_strip(0xFF, GREEN);
            latchedLEDs.write_strip(0xFF, BLUE);
        } else {
            latchedLEDs.write_strip(0x0, RED);
            latchedLEDs.write_strip(0x0, GREEN);
            latchedLEDs.write_strip(0x0, BLUE);
        }

        // Wait before the next iteration
        ThisThread::sleep_for(5s);
    }
}
