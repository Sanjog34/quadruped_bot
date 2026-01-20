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
    double hip_offset;
    double knee_offset;
} Leg;

int map(int x, int in_min, int in_max, int out_min, int out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void calc_angles(Leg *leg, double length, double height) // for horizontal motion of leg
{
    double d = pow((pow(length, 2) + pow(height, 2)), 0.5);
    leg->hip_angle = 90 - floor((180 / M_PI) * (atan(length / height) + acos((pow(leg->Humerus, 2) + pow(d, 2) - pow(leg->Radius, 2)) / (2 * leg->Humerus * d))));
    leg->knee_angle = map(floor((180 / M_PI) * acos((pow(leg->Humerus, 2) - pow(d, 2) + pow(leg->Radius, 2)) / (2 * leg->Humerus * leg->Radius))), 0, 180, 180, 0);
}

void calc_angles_vertical_motion(Leg *leg, double height) // for horizontal motion of leg
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
        leg[i].Humerus = 3;
        leg[i].Radius = 2.5;
    }
    printf("choose leg and max height to raise the leg : \n\n\n");
    scanf("%d%lf%lf", &temp, &height, &length);
    printf("leg.no.     hip_angle     knee_angle     height from ground\n");
    const int frames = 40; // smooth frames per step
    double y;            // foot height
    double x = 0;
    double step = height/frames;

    // Forward arc
//    for (int i = 0; i <= frames; i++)
//     {
//         double t = (double)i / frames;
//         y = 3- sin(M_PI * t) *(length/2) ; // smooth foot arc
//         calc_angles(&leg[temp], 0, y);
//         printf("%d         %lf            %lf             %lf\n", temp, leg[temp].hip_angle, leg[temp].knee_angle, pos);
//     }
    printf("\n\n\n\nleg.no.     hip_angle     knee_angle       length      height_from_ground\n");
    for (int i = 0; i <= frames; i++)
    {
         x = i == 0 ? x : x + step;
        calc_angles(&leg[temp], x, height);
        printf("%d         %lf            %lf             %lf       %lf\n", temp, leg[temp].hip_angle, leg[temp].knee_angle, x, height);
    }
}