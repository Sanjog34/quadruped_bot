#include <Arduino.h>
#include <ESP32Servo.h>
#include <math.h>
#include <quadruped.h>

// #define DEBUG
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
    }
    hip_angle = constrain(hip_angle, 0, 180);
    knee_angle = constrain(knee_angle, 0, 180);
}

void Leg::move_vertical()
{

    for (float i = height; i >= 5; i = i - 0.1)
    {
        get_angles(i, 0);
        print_angles(0, i);
        move_leg();
    }
    for (float i = 5; i <= height; i = i + 0.1)
    {
        get_angles(i, 0);
        print_angles(0, i);
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
        y_arc = height - (sqrt((step_size / 2) * (step_size / 2) - (x_arc + step_size / 2) * (x_arc + step_size / 2)));
        get_angles(y_arc, x_arc);
        move_leg();
    }
}

void Leg ::crawl_forward()
{
    if (pos == 0)
    {
        move_arc();
        pos = 1;
        return;
    }
    else if (pos == 1)
    {
        move_horizontal();
        pos = 0;
        return;
    }
}

void Leg::move_leg()
{
    servohip.write(hip_angle + hip_offset);
    servoknee.write(knee_angle + knee_offset);
    delay(8);
}

void Leg ::move_angles(double hipangle, double kneeangle)
{
    //    Serial.println("i'm here");
    servohip.write(hipangle + hip_offset);
    servoknee.write(kneeangle + knee_offset);
    delay(8);
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

void Leg ::calibrate(String name)
{
    double anglehip, angleknee;
    DEBUG_PRINT("calibrating ");
    DEBUG_PRINT(name);
    DEBUG_PRINT("\n\n");
    char c;
    if (leg_id == left)
    {
        anglehip = 180;
        angleknee = 0;
    }
    if (leg_id == right)
    {
        anglehip = 0;
        angleknee = 180;
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
    // servohip.write(90);
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
    DEBUG_PRINT("set_offset(");
    DEBUG_PRINT(hip_offset);
    DEBUG_PRINT(",");
    DEBUG_PRINT(knee_offset);
    DEBUG_PRINT(");\n");
    moveTo_base_pos();
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

void Quadruped::init(double height, double step_size)
{
    this->height = height;
    this->step_size = step_size;
    Front_Left.init(this->height, this->step_size);
    Front_Right.init(this->height, this->step_size);
    Back_Left.init(this->height, this->step_size);
    Back_Right.init(this->height, this->step_size);
}

void Quadruped::move_bot()
{
    Front_Left.servohip.write(Front_Left.get_hip_angle() + Front_Left.get_hipoffset());
    Front_Left.servoknee.write(Front_Left.get_knee_angle() + Front_Left.get_kneeoffset());

    Front_Right.servohip.write(Front_Right.get_hip_angle() + Front_Right.get_hipoffset());
    Front_Right.servoknee.write(Front_Right.get_knee_angle() + Front_Right.get_kneeoffset());

    Back_Left.servohip.write(Back_Left.get_hip_angle() + Back_Left.get_hipoffset());
    Back_Left.servoknee.write(Back_Left.get_knee_angle() + Back_Left.get_kneeoffset());

    Back_Right.servohip.write(Back_Right.get_hip_angle() + Back_Right.get_hipoffset());
    Back_Right.servoknee.write(Back_Right.get_knee_angle() + Back_Right.get_kneeoffset());

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
    delay(1000);
}

void Quadruped::move_forward()
{
    if (initial_position == 2)
    {
        moveTo_base_pos();
        initial_position = -1;
    }
    else if (initial_position == -1)
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
    // DEBUG_PRINT(initial_position);
    // DEBUG_PRINT("\n");
    else
    {
        const int frames = 40;
        double y_arc;
        double x_arc, x_horiz = -step_size;
        double step = step_size / frames;

        for (int i = 0; i <= frames; i++)
        {
            double t = (double)i / frames;
            double x_arc = -t * step_size;

            y_arc = height - (sqrt((step_size / 2) * (step_size / 2) - (x_arc + step_size / 2) * (x_arc + step_size / 2)));
            x_horiz = i * step - step_size;
            DEBUG_PRINT(x_horiz);
            DEBUG_PRINT("\n");

            if (initial_position == 0)
            {
                Front_Left.get_angles(height, x_horiz);
                Back_Right.get_angles(height, x_horiz);
                Front_Right.get_angles(y_arc, x_arc);
                Back_Left.get_angles(y_arc, x_arc);
                // FL and BR 's next move should be an horizontal(backward)
                // AND FR and BL 's next move should be arc(forward)
            }
            else if (initial_position == 1)
            {
                Front_Left.get_angles(y_arc, x_arc);
                Back_Right.get_angles(y_arc, x_arc);
                Front_Right.get_angles(height, x_horiz);
                Back_Left.get_angles(height, x_horiz);
                // FL and BR 's next move should be an arc(forward)
                // AND FR and BL 's next move should be horizontal(backward)
            }
            move_bot();
        }
        initial_position = initial_position == 0 ? 1 : 0;
    }
    DEBUG_PRINT(initial_position);
    DEBUG_PRINT("\n\n");
}

void Quadruped::move_backward()
{
    if (initial_position == 2)
    {
        moveTo_base_pos();
        initial_position = -1;
    }
    else if (initial_position == -1)
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

void Quadruped ::move_vert()
{
    for (float i = height; i >= 6.5; i = i - 0.1)
    {
        Back_Right.get_angles(i, 0);
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

void Quadruped ::calibrate()
{
    Back_Left.calibrate("Back left");
    Back_Right.calibrate("Back right");
    Front_Left.calibrate("Front left");
    Front_Right.calibrate("Front right");
}
void Quadruped ::shake_hand()
{
    moveTo_base_pos();
    delay(100);
    Front_Right.move_angles(180, 30);
    delay(500);
    for (int times = 0; times < 2; times++)
    {

        for (int i = 180; i >= 160; i = i - 2)
        {
            // Serial.println("here1");
            Front_Right.servohip.write(i);
            delay(50);
        }
        for (int i = 160; i <= 180; i = i + 2)
        {
            Front_Right.servohip.write(i);
            // Serial.println("here2");
            delay(50);
        }
    }
    delay(1000);
    moveTo_base_pos();
}

void Quadruped ::sit_down()
{
    for (int i = height; i >= 6.5; i = i - 0.2)
    {
        Front_Left.get_angles(i, 0);
        Front_Right.get_angles(i, 0);
        Back_Left.get_angles(i, 0);
        Back_Right.get_angles(i, 0);
        move_bot();
    }
    delay(500);
}
void Quadruped ::say_hi()
{
    if (initial_position != 2)
    {
        sit_down();
        initial_position = 2;

        for (float i = 6.5; i <= 10; i = i + 0.2)
        {
            Front_Left.get_angles(i, 0);
            Front_Right.get_angles(i, 0);
            move_bot();
        }
    }
    delay(1000);
    Front_Right.move_angles(180, 0);
    delay(500);
    for (int times = 0; times < 2; times++)
    {

        for (int i = 0; i <= 120; i = i + 10)
        {
            Front_Right.servoknee.write(i);
            // Serial.println("here2");
            delay(50);
        }
        for (int i = 120; i >= 0; i = i - 10)
        {
            // Serial.println("here1");
            Front_Right.servoknee.write(i);
            delay(50);
        }
    }
    delay(1000);
    move_bot();
    delay(500);
}
