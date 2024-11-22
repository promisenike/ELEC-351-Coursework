#include "mbed.h"
#include "MSB_Config.h"


typedef enum{TRAF_SET_1,TRAF_SET_2,TRAF_PED}trafficSet_t;

class TrafficLights{
    private:
        BusOut _bus1;
        BusInOut _bus2; 
        DigitalInOut Pedestrian;

    public:
        TrafficLights(PinName r1,PinName y1, PinName g1,PinName r2,PinName y2 ,PinName g2, PinName ped);
        void set(trafficSet_t set, uint8_t val);
   
};