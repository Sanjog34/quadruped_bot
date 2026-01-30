
#include "quadruped.h"
Leg back_left(8, 10.25, 14, 13, left);
Leg back_right(8, 10.25, 19, 18, right);
Leg front_left(8, 10.25, 27, 26, left);
Leg front_right(8, 10.25, 23, 21, right);

Quadruped quadruped(front_right, front_left, back_right, back_left);
void setup()
{

    Serial.begin(115200);
    quadruped.init(15, 7);
    back_left.set_offset(-10.00,6.00);
    back_right.set_offset(3.00,-12.00);
   front_right.set_offset(7.00,-4.00);
   front_left.set_offset(-6.00,0.00);
    quadruped.moveTo_base_pos();
    delay(3000);
    quadruped.move_vert();
}
void loop()
{
    if (Serial.available())
    {
        char c = Serial.read();
        if (c == 'a')
        {
            quadruped.move_forward();
        }
        else if (c == 'b')
        {
            quadruped.move_backward();
        }
        else if (c == 'c')
        {
            quadruped.moveTo_base_pos();
        }
        else if (c == 's')
        {
            quadruped.shake_hand();
        }
                else if (c == 'h')
        {
            quadruped.say_hi();
        }
    }
}
