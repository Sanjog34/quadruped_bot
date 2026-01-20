#include <stdio.h>
#include <math.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
typedef struct
{
    double Humerus;
    double Radius;
    double height;
    double hip_angle;
    double knee_angle;
} Leg;

int map(int x, int in_min, int in_max, int out_min, int out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


void calc_angles_horizontal_motion(Leg *leg, double length, double height)
{
    double d = pow((pow(length, 2) + pow(height, 2)), 0.5);
    leg->hip_angle = 90 - floor((180 / M_PI) * (atan(length / height) + acos((pow(leg->Humerus, 2) + pow(d, 2) - pow(leg->Radius, 2)) / (2 * leg->Humerus * d))));
    leg->knee_angle = map(floor((180 / M_PI) * acos((pow(leg->Humerus, 2) - pow(d, 2) + pow(leg->Radius, 2)) / (2 * leg->Humerus * leg->Radius))), 0, 180, 180, 0);
}

void calc_angles_vertical_motion(Leg *leg, double height, double length)
{
    leg->hip_angle = 90 - floor((180 / M_PI) * (acos((pow(leg->Humerus, 2) + pow(height, 2) - pow(leg->Radius, 2)) / (2 * height * leg->Humerus))));
    leg->knee_angle = map(floor((180 / M_PI) * (acos((pow(leg->Radius, 2) - pow(height, 2) + pow(leg->Humerus, 2)) / (2 * leg->Radius * leg->Humerus)))), 0, 180, 180, 0);
}
int main()
{
    Leg leg[4];
    int temp;
    double height;
    double length;
    for (int i = 0; i < 4; i++)
    {
        leg[i].Humerus = 8;
        leg[i].Radius = 10.25;
    }
    printf("choose leg and max height to raise the leg : \n\n\n");
    scanf("%d%lf%lf", &temp, &height, &length);
    printf("leg.no.     hip_angle     knee_angle     height from ground\n");
    for (float i = 0; i <= height; i = i + 0.1)
    {
        calc_angles_vertical_motion(&leg[temp], i,length);
        printf("%d         %lf            %lf             %lf\n", temp, leg[temp].hip_angle, leg[temp].knee_angle, i);
    }
    printf("\n\n\n\nleg.no.     hip_angle     knee_angle    hor_length\n");
    for (float i = 0; i <= length; i = i + 0.1)
    {
        calc_angles_horizontal_motion(&leg[temp], i, height);
        printf("%d         %lf            %lf                 %lf\n", temp, leg[temp].hip_angle, leg[temp].knee_angle, i);
    }
}