#include "sensor_data.hpp"
#include "uop_msb.h"

void SensorData::sample() {
    temperature = env.getTemperature();  // Get temperature from the sensor
    pressure = env.getPressure();        // Get pressure from the sensor
    light_level = ldr.read();            // Get light level from the LDR
}
