<h1>Plymouth University Module Support Board</h1>
<h2>An Mbed library to provide high level access to the features of the Module Support Board</h2>
<div>Including :-   </div>
<div>SD Card</div>
<div>LED Matrix</div>
<div>Latched LED Bar</div>
<div>Latched Seven Segment Display</div>
<div>ADCs and DACs</div>
<div>Inertial Measurement Unit- </div>
<div>Environmental Sensor</div>
<div>Switches</div>
<div>Buzzer</div>
<div>LCD</div>


This repository contains the library files only.
The demo code which exercises and tests all the peripherals can be found at
<a href>https://github.com/PlymouthUniversityEEER/UOP_MSB_Test</a>


# **Usage** 

## Adding the library
- Open a new or existing Mbed project in Mbed Studio.
- Open the "file" tab and click "Add Library To Active Program"
-In the URL field enter "https://github.com/PlymouthUniversityEEER/PlymouthUniversityModuleSupportBoard/"
-Name the library whatever you like and click "Next"
-On the next window select "main" from the "Branch or tag" field and click next.
-Mbed should now download the library from the Github repository and add it into your project in a folder with the name you gave to the library the the previous steps

## Setting up the library
To set up the library for use with your particular Nucleo board and module support board, you should set the correct values in the file "MSB_Config.h"
This file sets up the prerequisites to link your board version and module support board version.
There are currently two defines which you may need to edit.
These are "MSB_VER" and "USE_SD_CARD". The MSB_VER define requires a numerical value of your module support board to be passed to it.
Current versions available are 2 and 4 (most people will have V4).
"USE_SD_CARD" just needs to be defined if you wish to use the SD card. If not, this line should be commented out.

In order to use the SD Card and I2C, they need to be enabled in mbed_app.json.
This library contains an mbed_app.json file which can be copied into your project directory.
NB. Ensure that it is placed in the root of the project directory.
If you receive a compilation error relating to SDBlockDevice, you have probably forgotten to copy mbed_app.json from the library folder to the project folder.


```
#define MSB_VER 4
//#define MSB_VER 2

// If you do not wish to use the SD Card, comment this line out
#define USE_SD_CARD
```

Besides these two configuration steps, you should not need to do anything else with the "MSB_Config.h" file


To use the library in your project you must include the "uop_msb.h" header file where you wish to use it. (eg main.cpp)
This file includes the header files required by the rest of the library, along with predefined and instantiated objects for you to use.
By default the library creates all of the objects. If you do not wish for a particular object to be instantiated, you can comment it out in this file.
```
    // Precreated library objects for you to use
    AnalogIn pot(AN_POT_PIN);
    AnalogIn ldr (AN_LDR_PIN);
    AnalogIn signal_in (BNC_AN_PIN);
    AnalogIn mic (MIC_AN_PIN);
    AnalogOut dac_out_1 (DAC1_AN_PIN);
    AnalogOut dac_out_2 (DAC2_AN_PIN);
    LatchedLED latchedLEDs(LEDMODE::STRIP);     
    Buttons buttons;
    // LCD_16X2_DISPLAY disp; <- LCD will not be instantiated as it is commented out
    EnvSensor env (ENV_MOSI,ENV_MISO,ENV_SCLK,ENV_CS);
    Buzzer buzz;
    DIPSwitches dipSwitches (DIP0_PIN,DIP1_PIN,DIP2_PIN,DIP3_PIN);
    MotionSensor motion (MEMS_SDA,MEMS_SCL,MEMS_FREQ);
    Matrix matrix (MATRIX_MOSI,MATRIX_MISO,MATRIX_SCLK,MATRIX_CS,MATRIX_OE);
    TrafficLights traffic(TRAF_RED1_PIN,TRAF_YEL1_PIN,TRAF_GRN1_PIN,TRAF_RED2_PIN,TRAF_YEL2_PIN,TRAF_GRN2_PIN,TRAF_WHITE_PIN);
```

# **How to use the library objects**


## AnalogIn and AnalogOut

The AnalogIn and AnalogOut objects are just basic Mbed objects. 
More info can be found <a href="https://os.mbed.com/docs/mbed-os/v6.16/apis/i-o-apis.html">here </a>  and <a href="https://os.mbed.com/docs/mbed-os/v6.16/apis/analogout.html">here.</a>


## SD Card

The SDCard class puts a wrapper around Mbed's SDBlockDevice class.
It contains methods for writing to a file, printing data contained within a file and copying the contents of a file to an array.
The user can select whether or not they wish debug messages to be displayed during read/write attempts.
This is done my passing "true" as the final argument to the function calls.

```
    // Create test string 
    char test_string[] = "This is the Plymouth University EEER Module Support Board Test\n";
    // Write the string to a file
    sd.write_file("msb_test.txt", test_string,true);
    // print the contents of the file we've just written to
    sd.print_file("msb_test.txt",true);
    // Copy the contents back into another array
    char array_to_copy_to[256];
    sd.copy_file("msb_test.txt", array_to_copy_to,sizeof(array_to_copy_to),true);
    // Print the new array
    printf("%s\n",array_to_copy_to);
```

## Latched LEDs (RGB Strip and Seven-Segment Display)

RGB Strip and the 7 segment display both contain latches. That is to say that when the relevant latch-enable (LE) pin transitions from low-to-high,
the data which is on a 8 bit data bus is latched. If the data changes, the output will not change unless the latch IC sees another rising edge on the LE pin.
This allows us to link multiple devices to the same data bus, providing we have seperate latch pins. This is what has been done with the RGB Strip and The seven segment display.
There are 8 data pins, 5 latch pins(red,green,blue,tens,units) and one master output enable pin (OE).

To write to the RGB strips you must use the write_strip function and specify the binary data that you wish to write, followed by the group you wish to write to.
LEDGROUP is a typedef which allows you to specify the bar you wish to write to.

```
// LEDGROUP typedef
typedef enum {TENS, UNITS, RED, GREEN, BLUE} LEDGROUP;
// function to write to the RGB strips
int write_strip(uint8_t dat, LEDGROUP grp);

// Example
for(int i=0;i<255;i++){
    latchedLEDs.write_strip(i,LEDGROUP::RED);
    latchedLEDs.write_strip(i,LEDGROUP::GREEN);
    latchedLEDs.write_strip(i,LEDGROUP::BLUE);
    ThisThread::sleep_for(20);
}
```

The seven segment display works in the same way. However here the API has been simplified so that the write_seven_seg function can be passed numerical value and that will be placed on the display.
The write_seven_seg function has been overloaded so that either numerical values (0-99) of floating point numbers (0.0-9.9) can be passed as an argument

```
// Write decimal number to seven segment display
int write_seven_seg(uint8_t dat);
// Write floating point number to seven segment display
int write_seven_seg(float dat);

// Example
    unsigned char counter=0;
    float fl_counter = 0.0f;
    while(true){
        while(fl_counter<10.0f){
            latchedLEDs.enable(true);
            latchedLEDs.write_seven_seg((float)fl_counter);
            fl_counter= fl_counter + 0.1f;
            thread_sleep_for(250);
        }
        fl_counter = 0.0f;

        while(counter<100){
            latchedLEDs.enable(true);
            latchedLEDs.write_seven_seg(counter);
            counter++;
            thread_sleep_for(250);
        }
        counter = 0;
    }
```

There is also a function to enable or disable either the seven segment display, RGB strips, or both.
Similar to the LEDGROUP typedef, There is an LEDMODE typedef which allows us to select whether we are changing the state of the seven segment display, or the RGB strips

```
typedef enum {STRIP, SEVEN_SEG} LEDMODE;

// Enable or disable all groups
void enable(bool en);
// Enable or disable a strip or seven seg
void enable(bool en, LEDMODE mod);

// Example
latchedLEDS.enable(false,SEVEN_SEG);    //  Turn off the seven seg display
latchedLEDs.enable(true,STRIP);

while(1){    // Repeatidly flash all displays
    latchedLEDS.enable(false);
    ThisThread::sleep_for(100ms);
    latchedLEDS.enable(true);
    ThisThread::sleep_for(100ms);
}

```

## Matrix

The LED matrix is configured in a way that allows only one row at a time to be illuminated, but this can contain any number of LEDs on that row.
If you wish to give the impression that multiple rows are illuminated, you must use a fast scan to trick your eyes into thinking that more than one row is on at once.
There are 3 main APIs for writing to the matrix

```
// Display a dot at coordinates row,col
void dot(uint8_t row, uint8_t col);
// Display a horizontal line on row
void line(uint8_t row);
// Display col_data binary value on row
void write(uint8_t row, uint16_t col_data );

// Example

// Go through rows one at a time
for(int i=0;i<8;i++){
    matrix.line(i);
    ThisThread::sleep_for(100ms);
}

// Make dot which scrolls left to right one row at a time
for(int y=0;y<8;y++){
    for(int x=0;x<16;x++){
        matrix.dot(x,y);
        ThisThread::sleep_for(10ms);
    }
}

// Count in binary on row 0
for(uint16_t i=0;i<0xffff;i++){
    matrix.write(0,i);
    ThisThread::sleep_for(10ms);
}


```

## LCD

The LCD_16X2_DISPLAY class performs all the neccesary set up commands to initialise the display when an object is instantiated (in it's constructor).
The class implements the virtual function _putc from the c stdio.h library. This allows us to use printf on the LCD in the same way we use it in the terminal.
Public functions also exist for clearing the screen and changing the cursor position.

```
// Set the cursor to position (row,column)
void locate(uint8_t row, uint8_t column);
// Set the cursor to 0,0
void home();
// Clear the screen and set the cursor to 0,0
void cls();

virtual int _putc(int value);

// Example

disp.cls();                  // clear the display and home the cursor
disp.printf("Hello World");  // Write some text on line 0
disp.locate(1,5);            // Set the cursor the 1,5 
int val=42;                  // any integer
float fval=42.0;             // any float
disp.printf("%d %2.2f",val, fval);

```

## Traffic Lights

The traffic lights are two different Mbed BusOut objects and a DigitalOut object for the white pedestrian LED.
To write to the LEDs, the set function is used. This requires the set of lights to be selected and the binary value of the corresponding bus.
The trafficSet_t typedef is used to aid the selection

```
typedef enum{TRAF_SET_1,TRAF_SET_2,TRAF_PED}trafficSet_t;

// Example
while (true)
{
    traffic.set(TRAF_PED,1);
    traffic.set(TRAF_SET_1,1);
    traffic.set(TRAF_SET_2,3);
    ThisThread::sleep_for(TRAFFIC_LIGHT_DELAY);

    traffic.set(TRAF_PED,0);
    traffic.set(TRAF_SET_1,3);
    traffic.set(TRAF_SET_2,5);
    ThisThread::sleep_for(TRAFFIC_LIGHT_DELAY);

    traffic.set(TRAF_SET_1,4);
    traffic.set(TRAF_SET_2,6);
    ThisThread::sleep_for(TRAFFIC_LIGHT_DELAY);

    traffic.set(TRAF_SET_1,2);
    traffic.set(TRAF_SET_2,4);
    ThisThread::sleep_for(TRAFFIC_LIGHT_DELAY);
}

```
## Buttons

The Buttons class just holds basic DigitalIn objects named Button1..4 and BlueButton.

```
// Example

bool state1 = buttons.Button1.read();
bool state2 = buttons.Button2.read();
bool state_blue = buttons.BlueButton.read();
```

## DIP Switches

The DIPSwitches class puts a wrapper around an Mbed BusIn object.
To read the switch values, either the whole bus can be read and the binary returned, or the state of a single switch can be read.
The read() function has ben overloaded so that it returns a uint8_t value for the whole bus if no argumement is passed.
Alternatively a switch number can be passed and a 0 or 1 is returned relating to the value of that one switch.
The [] operator has been overloaded to allow single switch reading similar to read(pin), but using square brackets.

```
// Read all swwitches and return binary
uint8_t read();
// Read single switch and return value
uint8_t read(int pin);
// [] Overload for reading single switch
uint8_t operator[](const uint8_t pin);

uint8_t bus_value = dipSwitches.read();
bool dip1_state dipSwitches.read(1);
bool dip1_state_2 = dipSwitches[1];    // same as line above

```

## Buzzer

The Buzzer uses an Mbed PwmOut object. To change the tone which is being played, we change the frequency of the PWM.
This is hidden away and the playTone() function can be used which converts a note into the correct frequency. 
The note is passed as a char array with the alphanumeric value of the note.
Notes can be in one of three octaves. The OCTAVE_REGISTER typedef helps us to specify which octave we want the tone to be in.
To stop playing the tone we use the rest() function.

```
typedef enum {LOWER_OCTAVE, MIDDLE_OCTAVE, HIGHER_OCTAVE} OCTAVE_REGISTER;
// Constructor
Buzzer(PinName p = BUZZER_PIN);
// Plays note in octave
void playTone(const char* note, OCTAVE_REGISTER octave=MIDDLE_OCTAVE);
// Stops the tone
void rest();

// Example
buzz.playTone("C",MIDDLE_OCTAVE);  // Play a C in middle octave
ThisThread::sleep_for(1000ms);    // Keep playing the note for 1 sec
buzz.rest();                      // Stop the tone playing
buzz.playTone("C#",HIGHER_OCTAVE);  // Play a C# in higher octave
ThisThread::sleep_for(1000ms);    // Keep playing the note for 1 sec
buzz.rest();                      // Stop the tone playing

```

## Environmental Sensor

The EnvSensor class communicates using SPI with an environmental sensor on the Module Support Board.
It takes readings of temperature, humidity and pressure.
The sensor is initialised in the constructor so you don't have to do any setup yourself.
The get functions can be used to retrieve float values for each

```
float getTemperature();    // Temp in deg C
float getPressure();       // Pressure in mbar
float getHumidity();       // Humidity in percent

// Example
float temperature = env.getTemperature();
float pressure=env.getPressure();
float humidity = env.getHumidity();
```

Two different types of environmental sensors have been used on the Module Support Boards. V2 used the BMP280, while V4 uses the SPL06-001.
In the constructor, the library will work out which sensor is being used on your board.
We can use the getSensorType() function to fins out which sensor we have if we are not sure. This returns a typedef ENV_SENSOR_TYPE

```
typedef enum {NONE, BMP280, SPL06_001} ENV_SENSOR_TYPE;

// Example
ENV_SENSOR_TYPE type =  env.getSensorType();
```

## Motion Sensor

The Module Support Board contains an MPU6050 interial measurment unit (IMU). 
This can be used to give acceleration and angular velocity readings from an accelerometer and gyroscope.
It also contains a temperature sensor.
The class has a typedefed struct Motion_t which can hold 3 float values for x,y and z
We can sample the sensor we wish to read and it returns a version of this struct containg the relevant values.

```
typedef struct {
    float x;
    float y;
    float z;
} Motion_t;

Motion_t getAcceleration();

// Read x,y and z values from the gyroscope
Motion_t getGyro();

// Read temperature using the MPU6050
float getTemperatureC();

// Example

while(1){
    Motion_t acc = motion.getAcceleration();
    Motion_t gyro = motion.getGyro();
    printf("Gyro x:%3.3f Gyro y:%3.3f Gyro z:%3.3f\n", gyro.x,gyro.y,gyro.z);
    printf("Acc x:%3.3f Acc y:%3.3f Acc z:%3.3f\n", acc.x,acc.y,acc.z);
    ThisThread::sleep_for(500ms);
}

```


