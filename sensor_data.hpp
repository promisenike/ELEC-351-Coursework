#ifndef SENSOR_DATA_HPP
#define SENSOR_DATA_HPP

#include "uop_msb.h"

// Structure to hold sensor data
struct SensorData {
    float temperature;   // Temperature in degrees Celsius
    float pressure;      // Pressure in mbar
    float light_level;   // Light level from the LDR

    void sample();       // Method to sample sensor data
};

#endif // SENSOR_DATA_HPP
