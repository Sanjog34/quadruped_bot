#ifndef QUADRUPED_H
#define QUADRUPED_H
#include <ESP32Servo.h>
class Leg
{
private:
    const double Humerus;
    const double Radius;
    double height;
    double hip_angle=180; 
    double knee_angle=0;
    double hip_offset=0;
    double knee_offset=0;
    int hip_Pin;
    int knee_Pin;
    double step_size;
    int leg_id;

public:
    Servo servohip;
    Servo servoknee;
    Leg(double Humerus, double Radius,int hip_pin,int knee_pin,int leg_id);
    void set_offset(double hip_offset, double knee_offset);
    void get_angles(double height, double length);
    void move_vertical();
    void move_horizontal();
    void base_height_stepSize(double Height, double );
    void move_leg();
    double get_hip_angle();
    double get_knee_angle();
};

class Quadruped
{
private:
    Leg &Front_Right;
    Leg &Front_Left;
    Leg &Back_Right;
    Leg &Back_Left;
    double height;
    double step_size;    //distance covered by bot in 1 footstep
    int initial_position = -1;


public:
    Quadruped(Leg &fr, Leg &fl, Leg &br, Leg &bl)
        : Front_Right(fr),
          Front_Left(fl),
          Back_Right(br),
          Back_Left(bl)
    {
    }
    void set_base_height_and_step_size(double height, double step_size);
    void move_bot();
    void move_forward();
    void move_backward();
    void sit_down();
    void say_hi();
    void shake_hand();
    void base_height();
    void print_angles(double hipfr, double kneefr,double length_fr,double height_fr, double hipbr, double kneebr,double length_br,double height_br);
};
#endif