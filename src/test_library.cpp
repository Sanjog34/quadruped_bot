
#include "quadruped.h"

Leg front_right(8, 10.25, 13, 14, right);
void setup()
{

    Serial.begin(115200);
    front_right.init(10, 7);
    // Serial.println("start");
    // front_right.move_horizontal();
    // Serial.println("end");
}
void loop()
{
    if(Serial.available()){
        char c= Serial.read();
        if(c=='a'){
            Serial.println("start");
            front_right.crawl_forward();
            Serial.println("end");
        }
    }
}
