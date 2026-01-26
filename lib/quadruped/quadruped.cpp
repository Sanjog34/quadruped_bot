#include <Arduino.h>
#include <ESP32Servo.h>
#include <math.h>
#include <quadruped.h>

#define DEBUG
#ifdef DEBUG
#define DEBUG_PRINT(x) Serial.print(x)
#else
#define DEBUG_PRINT(x)
#endif

Leg::Leg(double humerus, double radius, int hip_pin, int knee_pin, int Leg_id)
    : Humerus(humerus),
      Radius(radius),
      hip_Pin(hip_pin),
      knee_Pin(knee_pin),
      leg_id(Leg_id)
{

    if (leg_id == left)
    {
        hip_angle = 180;
        knee_angle = 0;
    }
    else if (leg_id == right)
    {
        hip_angle = 0;
        knee_angle = 180;
    }
}

void Leg::init(double height, double step_size)
{
    servohip.attach(hip_Pin);
    servoknee.attach(knee_Pin);
    this->height = height;
    this->step_size = step_size;
}

void Leg ::moveTo_base_pos()
{
    get_angles(height, 0);
    move_leg();
    pos = 0;
}

void Leg::set_offset(double hip_offset, double knee_offset)
{
    this->hip_offset = hip_offset;
    this->knee_offset = knee_offset;
}

void Leg::get_angles(double height, double length)
{
    double d = sqrt(length * length + height * height);

    double hip_arg = (Humerus * Humerus + d * d - Radius * Radius) / (2 * Humerus * d);
    hip_arg = constrain(hip_arg, -1, 1);

    double knee_arg = (Humerus * Humerus - d * d + Radius * Radius) / (2 * Humerus * Radius);
    knee_arg = constrain(knee_arg, -1, 1);
    if (leg_id == right)
    {
        hip_angle = int(90 - (180.0 / M_PI) * (atan2(length, height) + acos(hip_arg)));
        int knee_deg = int((180.0 / M_PI) * (acos(knee_arg)));
        knee_angle = map(knee_deg, 0, 180, 180, 0);
    }
    else if (leg_id == left)
    {
        int hip_deg = int(90 - (180.0 / M_PI) * (atan2(length, height) + acos(hip_arg)));
        knee_angle = int((180.0 / M_PI) * (acos(knee_arg)));
        hip_angle = map(hip_deg, 0, 180, 180, 0);
        // knee_angle = map(knee_deg, 0, 180, 180, 0);
    }
    hip_angle = constrain(hip_angle, 0, 180);
    knee_angle = constrain(knee_angle, 0, 180);
}

void Leg::move_vertical()
{

    for (float i = height; i >= 5; i = i - 0.1)
    {
        get_angles(i, step_size);
        print_angles(step_size, i);
        move_leg();
    }
    for (float i = 5; i <= height; i = i + 0.1)
    {
        get_angles(i, step_size);
        print_angles(step_size, i);
        move_leg();
    }
}

void Leg::move_horizontal()
{
    // for (float i = 0; i >= -1*step_size; i = i - 0.1)
    // {
    //     get_angles(height, i);
    //     print_angles(i,height);
    //     move_leg();
    // }
    for (float i = -1 * step_size; i <= 0; i = i + 0.1)
    {
        get_angles(height, i);
        // print_angles(i,height);
        move_leg();
    }
}
void Leg ::move_arc()
{
    const int frames = 40;
    double y_arc;
    double x_arc, x_horiz = 0;
    double step = step_size / frames;
    for (int i = 0; i <= frames; i++)
    {
        double t = (double)i / frames;
        double x_arc = -t * step_size;
        // Serial.println(x_arc);
        y_arc = height - (sqrt((step_size / 2) * (step_size / 2) - (x_arc + step_size / 2) * (x_arc + step_size / 2)));
        // x_horiz = x_horiz + t * step_size;
        get_angles(y_arc, x_arc);
        // print_angles(x_arc,y_arc);
        move_leg();
    }
}

void Leg ::crawl_forward()
{
    // Serial.println(pos);
    if (pos == 0)
    {
        move_arc();
        pos = 1;
        return;
    }
    else if (pos == 1)
    {
        // Serial.println(pos);
        move_horizontal();
        pos = 0;
        return;
    }
}

void Leg::move_leg()
{
    servohip.write(hip_angle + hip_offset);
    servoknee.write(knee_angle + knee_offset);
}

void Leg ::move_angles(double hipangle, double kneeangle)
{

    servohip.write(hipangle + hip_offset);
    servoknee.write(kneeangle + knee_offset);
}

double Leg::get_hipoffset()
{
    return hip_offset;
}

double Leg::get_kneeoffset()
{
    return knee_offset;
}

double Leg::get_hip_angle()
{
    return hip_angle;
}

double Leg::get_knee_angle()
{
    return knee_angle;
}

void Leg ::print_angles(double length, double height)
{
    DEBUG_PRINT(hip_angle);
    DEBUG_PRINT(",");
    DEBUG_PRINT(knee_angle);
    DEBUG_PRINT(",");
    DEBUG_PRINT(length);
    DEBUG_PRINT(",");
    DEBUG_PRINT(height);
    DEBUG_PRINT("\n");
}

void Quadruped::init(double height, double step_size)
{
    this->height = height;
    this->step_size = step_size;
    Front_Left.init(this->height, this->step_size);
    Front_Right.init(this->height, this->step_size);
    Back_Left.init(this->height, this->step_size);
    Back_Right.init(this->height, this->step_size);
}
void Leg ::calibrate(String name)
{
    DEBUG_PRINT("calibrating ");
    DEBUG_PRINT(name);
    DEBUG_PRINT("\n\n");
    char c;
    double anglehip = 180;
    double angleknee = 0;
    if (leg_id == 1)
    {
        double anglehip = 0;
        double angleknee = 180;
    }
    servohip.write(anglehip);
    servoknee.write(angleknee);
    DEBUG_PRINT("setting hip offset\n");
    DEBUG_PRINT("PRESS 'a' TO INCREASE OFFSET AND 's' TO DECREASE OFFSET\n AND 'x' TO CONFIRM\n\n");
    do
    {
        if (Serial.available())
        {
            c = Serial.read();
            if (c == 'a')
            {
                hip_offset++;
                servohip.write(anglehip + hip_offset);
            }
            else if (c == 's')
            {
                hip_offset--;
                servohip.write(anglehip + hip_offset);
            }
            DEBUG_PRINT("hip offset = ");
            DEBUG_PRINT(hip_offset);
            DEBUG_PRINT("\n");
        }
    } while (c != 'x');
    servohip.write(90);
    c = 'o';

    DEBUG_PRINT("setting knee offset\n\n");
    DEBUG_PRINT("PRESS 'a' TO INCREASE OFFSET AND 's' TO DECREASE OFFSET\n AND 'x' TO CONFIRM\n\n");
    do
    {
        if (Serial.available())
        {
            c = Serial.read();
            if (c == 'a')
            {
                knee_offset++;
                servoknee.write(angleknee + knee_offset);
            }
            else if (c == 's')
            {
                knee_offset--;
                servoknee.write(angleknee + knee_offset);
            }
            DEBUG_PRINT("knee offset = ");
            DEBUG_PRINT(knee_offset);
            DEBUG_PRINT("\n");
        }
    } while (c != 'x');

    DEBUG_PRINT("done calibrating leg!!!\n");
}

void Quadruped::move_bot()
{
    Front_Left.move_leg();
    Front_Right.move_leg();
    Back_Left.move_leg();
    Back_Right.move_leg();
    delay(8);
}

void Quadruped::moveTo_base_pos()
{
    Front_Right.get_angles(height, 0);
    Front_Left.get_angles(height, 0);
    Back_Right.get_angles(height, 0);
    Back_Left.get_angles(height, 0);
    initial_position = -1;
    move_bot();
}

void Quadruped::move_forward()
{
    Serial.println(initial_position);
    if (initial_position == -1)
    {
        for (double i = 0; i >= -step_size; i = i - 0.5)
        {
            Front_Left.get_angles(height, i);
            Back_Right.get_angles(height, i);
            Back_Right.print_angles(height, i);
            move_bot();
        }
        initial_position = 0; // FL and BR 's next move should be an arc(forward)
                              // AND FR and BL 's next move should be horizontal(backward)
    }
    else
    {
        const int frames = 40;
        double y_arc;
        double x_arc, x_horiz = -step_size;
        double step = step_size / frames;
        // Forward arc
        for (int i = 0; i <= frames; i++)
        {
            double t = (double)i / frames;
            double x_arc = -t * step_size;

            y_arc = height - (sqrt((step_size / 2) * (step_size / 2) - (x_arc + step_size / 2) * (x_arc + step_size / 2)));
            x_horiz = i * step - step_size;
            Serial.println(x_horiz);

            if (initial_position == 0)
            {
                Front_Left.get_angles(height, x_horiz);
                Back_Right.get_angles(height, x_horiz);
                Back_Right.print_angles(height, x_horiz);
                Front_Right.get_angles(y_arc, x_arc);
                Back_Left.get_angles(y_arc, x_arc);
                // FL and BR 's next move should be an horizontal(backward)
                // AND FR and BL 's next move should be arc(forward)
            }
            else if (initial_position == 1)
            {
                Front_Left.get_angles(y_arc, x_arc);
                Back_Right.get_angles(y_arc, x_arc);
                Back_Right.print_angles(y_arc, x_arc);
                Front_Right.get_angles(height, x_horiz);
                Back_Left.get_angles(height, x_horiz);
                // FL and BR 's next move should be an arc(forward)
                // AND FR and BL 's next move should be horizontal(backward)
            }
            move_bot();
        }
        initial_position = initial_position == 0 ? 1 : 0;
    }
    Serial.println(initial_position);
    Serial.println("\n\n");
}

void Quadruped::move_backward()
{
    if (initial_position == -1)
    {
        for (double i = 0; i <= step_size; i = i + 0.5)
        {
            Front_Left.get_angles(height, i);
            Back_Right.get_angles(height, i);
            move_bot();
        }
        initial_position = 0; // FL and BR 's next move should be an horizontal(forward)
                              // AND FR and BL 's next move should be arc(backward)
    }
    else
    {
        const int frames = 40;
        double y_arc;
        double x_arc, x_horiz;
        double step = step_size / frames;
        for (int i = 0; i <= frames; i++)
        {
            double t = (double)i / frames;
            double x_arc = i * step - step_size;
            y_arc = height - (sqrt((step_size / 2) * (step_size / 2) - (x_arc + step_size / 2) * (x_arc + step_size / 2)));
            x_horiz = -t * step_size;
            if (initial_position == 1)
            {
                Front_Left.get_angles(height, x_horiz);
                Back_Right.get_angles(height, x_horiz);
                Front_Right.get_angles(y_arc, x_arc);
                Back_Left.get_angles(y_arc, x_arc);
                // FL and BR 's next move should be an arc(backward)
                // AND FR and BL 's next move should be horizontal(forward)
            }
            else if (initial_position == 0)
            {
                Front_Left.get_angles(y_arc, x_arc);
                Back_Right.get_angles(y_arc, x_arc);
                Front_Right.get_angles(height, x_horiz);
                Back_Left.get_angles(height, x_horiz);
                // FL and BR 's next move should be an horizontal(forward)
                // AND FR and BL 's next move should be arc(backward)
            }
            move_bot();
        }
        initial_position = initial_position == 0 ? 1 : 0;
    }
}

void Quadruped::shake_hand()
{
    moveTo_base_pos();
    delay(200);
    Front_Right.servohip.write(180);
    Front_Right.servoknee.write(0);
    delay(500);
    for (int times = 0; times < 3; times++)
    {
        for (float i = 180; i >= 160; i = i - 2)
        {
            Front_Right.servohip.write(i);
            delay(8);
        }
        for (float i = 160; i <= 180; i = i + 2)
        {
            Front_Right.servohip.write(i);
            delay(8);
        }
    }
    moveTo_base_pos();
}

void Quadruped ::move_vert()
{
    for (float i = height; i >= 6.5; i = i - 0.1)
    {
        Back_Right.get_angles(i, 0);
        // Back_Left.print_angles()
        Front_Left.get_angles(i, 0);
        Front_Right.get_angles(i, 0);
        Back_Left.get_angles(i, 0);
        move_bot();
    }
    for (float i = 6.5; i <= height; i = i + 0.1)
    {
        Back_Right.get_angles(i, 0);
        Back_Left.get_angles(i, 0);
        Front_Left.get_angles(i, 0);
        Front_Right.get_angles(i, 0);

        move_bot();
    }
    moveTo_base_pos();
}

void Quadruped :: calibrate(){
    Back_Left.calibrate("Back left");
    Back_Right.calibrate("Back right");
    Front_Left.calibrate("Front left");
    Front_Right.calibrate("Front right");
}
