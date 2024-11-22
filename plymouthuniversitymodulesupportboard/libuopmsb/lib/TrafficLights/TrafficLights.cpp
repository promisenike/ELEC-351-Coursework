
#include "TrafficLights.h"


TrafficLights::TrafficLights(PinName r1,PinName y1, PinName g1,PinName r2,PinName y2 ,PinName g2, PinName ped) : _bus1(r1,y1,g1),_bus2(r2,y2,g2),Pedestrian(ped,PIN_OUTPUT,OpenDrain,1){
    _bus2.output();
    _bus2.mode(OpenDrainNoPull);
}


void TrafficLights::set(trafficSet_t set, uint8_t val){
    switch(set){
        case TRAF_SET_1:{
            _bus1 = val;
            break;
        }
        case TRAF_SET_2:{
            _bus2 = val;
            break;
        }
        
        case TRAF_PED:{
            int state = (val>0);
            Pedestrian = !state;
            break;
        }
    }
}

