#include "quadruped.h"

Leg back_left(8, 10.25, 14, 13, left);
Leg back_right(8, 10.25, 19, 18, right);
Leg front_left(8, 10.25, 27, 26, left);
Leg front_right(8, 10.25, 23, 21, right);

Quadruped quadruped(front_right, front_left, back_right, back_left);
int loop_once = 1;
void setup()
{

    Serial.begin(115200);
    quadruped.init(15, 7);
    //    back_left.set_offset(-10.00,6.00);
    //    back_right.set_offset(3.00,-10.00);        //these are the noted offset for previous setup
    //    front_right.set_offset(7.00,-4.00);
    //    front_left.set_offset(-6.00,0.00);
    quadruped.moveTo_base_pos();
    delay(1000);
}
void loop()
{ // note down the offsets and pass them to leg.calibrate(); after leg.init();
    while (loop_once == 1)
    {
        back_right.calibrate("back_right");
        delay(500);
        back_left.calibrate("back_left");
        delay(500);
        front_right.calibrate("front_right");
        delay(500);
        front_left.calibrate("front_left");
        delay(500);
        quadruped.move_vert();
        loop_once++;
    }
}
