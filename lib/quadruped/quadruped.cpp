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
    servohip.attach(hip_Pin);
    servoknee.attach(knee_Pin);
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
    if (leg_id == 1)
    {
        hip_angle = int(90 - (180.0 / M_PI) * (atan2(length, height) + acos(hip_arg) - (hip_offset * M_PI / 180.0)));
        int knee_deg = int((180.0 / M_PI) * (acos(knee_arg) + (knee_offset * M_PI / 180.0)));
        knee_angle = map(knee_deg, 0, 180, 180, 0);
    }
    else if (leg_id == 0)
    {
        int hip_deg = int(90 - (180.0 / M_PI) * (atan2(length, height) + acos(hip_arg) - (hip_offset * M_PI / 180.0)));
        int knee_angle = int((180.0 / M_PI) * (acos(knee_arg) + (knee_offset * M_PI / 180.0)));
        hip_angle = map(hip_deg, 0, 180, 180, 0);
    }
    hip_angle = constrain(hip_angle, 0, 180);
    knee_angle = constrain(knee_angle, 0, 180);
}

void Leg::move_vertical()
{
    DEBUG_PRINT("hip angle     knee angle      height from ground\n");
    for (float i = height; i >= 0; i = i - 0.1)
    {
        get_angles(i, 0);
        DEBUG_PRINT(hip_angle);
        DEBUG_PRINT("    ");
        DEBUG_PRINT(knee_angle);
        DEBUG_PRINT("    ");
        DEBUG_PRINT(i);
        DEBUG_PRINT("\n");
        move_leg();
    }
    for (float i = 0; i <= height; i = i + 0.1)
    {
        get_angles(i, 0);
        DEBUG_PRINT(hip_angle);
        DEBUG_PRINT("    ");
        DEBUG_PRINT(knee_angle);
        DEBUG_PRINT("    ");
        DEBUG_PRINT(i);
        DEBUG_PRINT("\n");
        move_leg();
    }
}

void Leg::move_horizontal()
{
    DEBUG_PRINT("hip angle     knee angle      length      height from ground\n");
    for (float i = 0; i <= step_size; i = i + 0.1)
    {
        get_angles(height, i);
        DEBUG_PRINT(hip_angle);
        DEBUG_PRINT("    ");
        DEBUG_PRINT(knee_angle);
        DEBUG_PRINT("    ");
        DEBUG_PRINT(i);
        DEBUG_PRINT("    ");
        DEBUG_PRINT(height);
        DEBUG_PRINT("\n");
        move_leg();
    }
    for (float i = step_size; i >= 0; i = i - 0.1)
    {
        get_angles(height, i);
        DEBUG_PRINT(hip_angle);
        DEBUG_PRINT("    ");
        DEBUG_PRINT(knee_angle);
        DEBUG_PRINT("    ");
        DEBUG_PRINT(i);
        DEBUG_PRINT("    ");
        DEBUG_PRINT(height);
        DEBUG_PRINT("\n");
        move_leg();
    }
}

void Leg::base_height_stepSize(double height, double step_size)
{
    this->height = height;
    this->step_size = step_size;
    get_angles(this->height, 0);
    move_leg();
}

void Leg::move_leg()
{
    servohip.write(hip_angle);
    servoknee.write(knee_angle);
    delay(8);
}

double Leg::get_hip_angle()
{
    return hip_angle;
}

double Leg::get_knee_angle()
{
    return knee_angle;
}

void Quadruped::set_base_height_and_step_size(double height, double step_size)
{
    this->height = height;
    this->step_size = step_size;
}

void Quadruped::move_bot()
{
    Front_Left.servohip.write(Front_Left.get_hip_angle());
    Front_Left.servoknee.write(Front_Left.get_knee_angle());

    Front_Right.servohip.write(Front_Right.get_hip_angle());
    Front_Right.servoknee.write(Front_Right.get_knee_angle());

    Back_Left.servohip.write(Back_Left.get_hip_angle());
    Back_Left.servoknee.write(Back_Left.get_knee_angle());

    Back_Right.servohip.write(Back_Right.get_hip_angle());
    Back_Right.servoknee.write(Back_Right.get_knee_angle());

    delay(8);
}

void Quadruped::base_height()
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
    if (initial_position == -1)
    {
        for (double i = 0; i <= step_size; i = i + 0.5)
        {
            Front_Left.get_angles(height, i);
            Back_Right.get_angles(height, i);
            move_bot();
        }
        initial_position = 0; // FL and BR 's next move should be an arc(forward)
                              // AND FR and BL 's next move should be horizontal(backward)
    }
    else
    {
        const int frames = 40;
        double y_arc;
        double x_arc, x_horiz = 0;
        double step = step_size / frames;
        // Forward arc
        for (int i = 0; i <= frames; i++)
        {
            double t = (double)i / frames;
            double x_arc = step_size - t * step_size;
            y_arc = height - (sqrt((step_size / 2) * (step_size / 2) - (x_arc - step_size / 2) * (x_arc - step_size / 2)));
            x_horiz = x_horiz + t * step_size;
            if (initial_position == 0)
            {
                Front_Left.get_angles(y_arc, x_arc);
                Back_Right.get_angles(y_arc, x_arc);
                Front_Right.get_angles(height, x_horiz);
                Back_Left.get_angles(height, x_horiz);
                initial_position == 1; // FL and BR 's next move should be an horizontal(backward)
                                       // AND FR and BL 's next move should be arc(forward)
            }
            else if (initial_position == 1)
            {
                Front_Right.get_angles(y_arc, x_arc);
                Back_Left.get_angles(y_arc, x_arc);
                Front_Left.get_angles(height, x_horiz);
                Back_Right.get_angles(height, x_horiz);
                initial_position = 0; // FL and BR 's next move should be an arc(forward)
                                       // AND FR and BL 's next move should be horizontal(backward)
            }
            move_bot();
        }
    }
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
                double x_arc = 0, x_horiz;
                double step = step_size / frames;
                for (int i = 0; i <= frames; i++)
                {
                    double t = (double)i / frames;
                    double x_arc = x_arc - t * step_size;
                    y_arc = height - (sqrt((step_size / 2) * (step_size / 2) - (x_arc - step_size / 2) * (x_arc - step_size / 2)));
                    x_horiz = step_size - t * step_size;
                    if (initial_position == 0)
                    {
                        Front_Left.get_angles(height, x_horiz);
                        Back_Right.get_angles(height, x_horiz);
                        Front_Right.get_angles(y_arc, x_arc);
                        Back_Left.get_angles(y_arc, x_arc);
                        initial_position == 1; // FL and BR 's next move should be an arc(backward)
                                               // AND FR and BL 's next move should be horizontal(forward)
                    }
                    else if (initial_position == 1)
                    {
                        Front_Left.get_angles(y_arc, x_arc);
                        Back_Right.get_angles(y_arc, x_arc);
                        Front_Right.get_angles(height, x_horiz);
                        Back_Left.get_angles(height, x_horiz);
                        initial_position == 0; // FL and BR 's next move should be an horizontal(forward)
                                               // AND FR and BL 's next move should be arc(backward)
                    }
                }
                move_bot();
            }
        }

        void Quadruped::shake_hand()
        {
            base_height();
            delay(200);
            Front_Right.servohip.write(0);
            Front_Right.servoknee.write(0);
            delay(500);
            for (int times = 0; times < 3; times++)
            {
                for (float i = 0; i <= 20; i = i + 2)
                {
                    Front_Right.servohip.write(i);
                    delay(8);
                }
                base_height();
                for (float i = 20; i >= 0; i = i - 2)
                {
                    Front_Right.servohip.write(i);
                    delay(8);
                }
            }
            base_height();
        }
