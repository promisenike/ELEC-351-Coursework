/* 
 * Filename: main.cpp
 * Author: Andrew Norris
 * Institution: Plymouth University
 * Date: 21/10/24
 * Description: Starter Code for ELEC351 Coursework 24/25
 * 
 * Notes:
 *
 * For documentation regarding the module support board library, please see the Readme.md on github
 * https://github.com/PlymouthUniversityEEER/PlymouthUniversityModuleSupportBoard/blob/main/README.md
 *
 * You will need to set the module support board version that you have in MSB_Config.h
 * The default if V4. If you are using a V2 comment out "#define MSB_VER 4" and uncomment "#define MSB_VER 2"
 * 
 * 
 */

#include "uop_msb.h"
#include "mbed.h"
#include <chrono>

#include "uop_msb.h"
#include "mbed.h"
#include <chrono>
#include <string>

// These objects have already been created for you in uop_msb.h
extern EnvSensor env;
extern LatchedLED latchedLEDs;
extern SDCard sd;
extern LCD_16X2_DISPLAY disp;

// Define a structure to hold sensor data
struct SensorData {
    float temperature;   // Temperature in degrees Celsius
    float pressure;      // Pressure in mbar
    float light_level;   // Light level from the LDR

    void sample() {
        temperature = env.getTemperature();  // Get temperature from the sensor
        pressure = env.getPressure();        // Get pressure from the sensor
        light_level = ldr.read();            // Get light level from the LDR
    }
};

// Mailbox for FIFO buffering
Mail<SensorData, 10> sensor_data_mailbox;  // FIFO mailbox with 10 slots


// Define the thresholds and hysteresis margin
struct Thresholds {
    float temp_upper = 30.0f;    // Upper threshold for temperature
    float temp_lower = 10.0f;    // Lower threshold for temperature
    float press_upper = 1020.0f; // Upper threshold for pressure
    float press_lower = 1000.0f; // Lower threshold for pressure
    float light_upper = 0.8f;    // Upper threshold for light
    float light_lower = 0.2f;    // Lower threshold for light
};

// Global variables
SensorData sensor_data;
uint32_t sample_num = 0;
Thresholds thresholds;

// Create an EventQueue for scheduling tasks
EventQueue queue;
Thread sample_thread(osPriorityHigh);

//  SD card write function
void write_to_sd() {
    char data_buffer[256];
    int buffer_len = 0;

    while (true) {
        // Collect data from the Mailbox (FIFO)
        int num_samples = 0;
        while (num_samples < 10) 
        {
            SensorData data;
            num_samples++;
            if (sensor_data_mailbox.try_get_for(4000ms))
            {
                // data for writing to SD card
                int len = snprintf(data_buffer + buffer_len, sizeof(data_buffer) - buffer_len,"Sample %d: Temperature = %.1f°C, Pressure = %.1f mbar, Light = %.2f\n", sample_num++, data.temperature, data.pressure, data.light_level);
                buffer_len += len;
                num_samples++;
            } else {
                break; // No data available in the mailbox
            }
        }

        // Write to SD card if enough data is accumulated
        if (buffer_len > 0) {
            if (sd.card_inserted()) {
                int err = sd.write_file("sensor_data.txt", data_buffer);
                if (err == 0) {
                    printf("Successfully written to SD card\n");
                } else {
                    printf("Error writing to SD card\n");
                }
            } else {
                printf("No SD Card Detected\n");
            }
            buffer_len = 0;  // Reset buffer length after writing
        }

        // Wait for the next write cycle (e.g., every 1 minute)
        ThisThread::sleep_for(1min);
    }
}

// Sample function running at high priority
void sample_data() {
    while (true) {
        sensor_data.sample();    // Sample the sensor data
        sample_num++;            // Increment the sample number

        // Push sampled data to the mailbox (FIFO buffer)
        if (!sensor_data_mailbox.try_get_for(10000ms)) {
            printf("Mailbox overflow: Data not written\n");
        }

        // After sampling, schedule the next sampling in 10 seconds
        queue.call_in(10s, sample_data);  // Schedule the next call of sample_data
    }
}

void monitor_sensors() {
    bool alarm_active = false;
    while (true) {
        // Print the samples to the terminal
        printf("\n----- Sample %d -----\n", sample_num);
        printf("Temperature: %3.1f°C\n", sensor_data.temperature);
        printf("Pressure: %4.1f mbar\n", sensor_data.pressure);
        printf("Light Level: %1.2f\n", sensor_data.light_level);

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

        // alarm settings
        if (alarm_active) {
            latchedLEDs.write_strip(0xFF, RED);
            latchedLEDs.write_strip(0xFF, GREEN);
            latchedLEDs.write_strip(0xFF, BLUE);
        } else {
            latchedLEDs.write_strip(0x0, RED);
            latchedLEDs.write_strip(0x0, GREEN);
            latchedLEDs.write_strip(0x0, BLUE);
        }

        // Print the time and date
        time_t time_now = time(NULL);   // Get a time_t timestamp from the RTC
        struct tm* tt;                  // Create empty tm struct
        tt = localtime(&time_now);      // Convert time_t to tm struct using localtime
        printf("%s\n", asctime(tt));    // Print in human readable format

        // Write the time and date on the LCD
        disp.cls();
        char lcd_line_buffer[17];
        
        strftime(lcd_line_buffer, sizeof(lcd_line_buffer), "%a %d-%b-%Y", tt); // Create string DDD dd-MM-YYYY
        disp.locate(0, 0);
        disp.printf("%s", lcd_line_buffer);
        
        strftime(lcd_line_buffer, sizeof(lcd_line_buffer), "     %H:%M", tt);  // Create string HH:mm
        disp.locate(1, 0);
        disp.printf("%s", lcd_line_buffer);

        // Wait 5 seconds before the next iteration
        ThisThread::sleep_for(std::chrono::seconds(5));
    }
}

int main() {
    // Set output enable on the latched LEDs.
    latchedLEDs.enable(true);

    // Set the time on the RTC
    uint64_t now = 1729519318;
    set_time(now);

    // Start the sampling immediately
    queue.call(sample_data);

    // Start the high-priority sampling thread
    sample_thread.start(sample_data);

    // Run the monitor thread in the background
    Thread sd_thread;
    sd_thread.start(write_to_sd);

    // Start monitoring sensors
    monitor_sensors();

    return 0;
}
