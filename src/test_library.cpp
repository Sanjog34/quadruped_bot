
#include"quadruped.h"

Leg front_right(3.0,2.5,13,14,0);
void setup(){

Serial.begin(115200);
front_right.base_height_stepSize(3,1.5);
front_right.move_vertical();
}
void loop(){
    
}
