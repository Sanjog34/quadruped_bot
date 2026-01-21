#include <Arduino.h>
#include <ESP32Servo.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

float height = 18;
float pos = 6.5;

typedef struct
{
    double Humerus;
    double Radius;
    double hip_angle;
    int knee_angle;
    double hip_offset;
    double knee_offset;
    int side;
} Leg;

Leg leg[4];
Servo hip1, knee1, hip2, knee2;

int map_int(int x, int in_min, int in_max, int out_min, int out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void calc_angles(Leg *leg, double height)
{     if (leg->side == 0)
    {
        leg->hip_angle = constrain(map_int(90 - floor(((180 / M_PI) * acos((pow(leg->Humerus, 2) + pow(height, 2) - pow(leg->Radius, 2)) / (2 * height * leg->Humerus)))-(leg->hip_offset*(M_PI/180))),0, 180, 180, 0),0,180);
        leg->knee_angle = constrain(floor(((180 / M_PI) * acos((pow(leg->Radius, 2) - pow(height, 2) + pow(leg->Humerus, 2)) / (2 * leg->Radius * leg->Humerus)))+(leg->knee_offset*(M_PI/180))),0,180);
    }
    else if (leg->side == 1)
    {

    leg->hip_angle = constrain(90 - floor(((180 / M_PI) * (acos((pow(leg->Humerus, 2) + pow(height, 2) - pow(leg->Radius, 2)) / (2 * height * leg->Humerus))))-(leg->hip_offset*(M_PI/180))),0,180);
        leg->knee_angle = constrain(map_int(floor(((180 / M_PI) * (acos((pow(leg->Radius, 2) - pow(height, 2) + pow(leg->Humerus, 2)) / (2 * leg->Radius * leg->Humerus))))+(leg->knee_offset*(M_PI/180))), 0, 180, 180, 0),0,180);
    }
}

void setup()
{
    Serial.begin(9600);


    hip1.attach(13);   // Hip on GPIO 13
    knee1.attach(14);  // Knee on GPIO 14
    hip2.attach(27);
    knee2.attach(26);

    for (int i = 0; i < 4; i++)
    {
        leg[i].Humerus = 8;
        leg[i].Radius = 10.25;
    }
    leg[0].side=0;
    leg[1].side=1;

    hip1.write(180);///158
    knee1.write(0);//65
    hip1.write(0);
    knee1.write(180);

    Serial.println("Hip (13 27) & Knee (14 26) Servo Ready");
}

void loop()
{
    if (Serial.available())
    {
        char c = Serial.read();

        if (c == 'a') // raise leg
        {
            pos = pos >= height ? height : pos + 0.5;
            calc_angles(&leg[0], pos);
            calc_angles(&leg[1], pos);
            hip1.write(leg[0].hip_angle);
            knee1.write(leg[0].knee_angle);
            hip2.write(leg[1].hip_angle);
            knee2.write(leg[1].knee_angle);           
            Serial.println("Hip: " + String(leg[0].hip_angle) + " Knee: " + String(leg[0].knee_angle) + " height :" + String(pos) + " Hip2: " + String(leg[1].hip_angle) + "  Knee2: " + String(leg[1].knee_angle));
        }
        else if (c == 's') // lower leg
        {
            pos = pos <= 6.5 ? 6.5 : pos - 0.5;
            calc_angles(&leg[0], pos);
            calc_angles(&leg[1], pos);
            hip1.write(leg[0].hip_angle);
            knee1.write(leg[0].knee_angle);
            hip2.write(leg[1].hip_angle);
            knee2.write(leg[1].knee_angle);   
            Serial.println("Hip: " + String(leg[0].hip_angle) + " Knee: " + String(leg[0].knee_angle) + " height :" + String(pos) + " Hip2: " + String(leg[1].hip_angle) + " Knee2: " + String(leg[1].knee_angle));
        }
    }
}

