#include "pothole.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
/*
Cx e Cy rappresentano le coordinate del centro
*/
bool isIntoRadius(const double cx, const double cy, int radius, const double x, const double y)
{
    double distance = sqrt(pow((cx - x), 2) + pow((cy - y), 2));
    return (distance > radius) ? false : true;
}

int comparePotholesByRange(Pothole *p1, Pothole *p2, int range)
{
    if ((p1->latitude == p2->latitude) && (p1->longitude == p2->longitude))
    {
        return 0;
    }
    else if (isIntoRadius(p1->latitude, p1->longitude, range, p2->latitude, p2->longitude))
    {
        return 1;
    }
    else
    {
        return -1;
    }
}

Pothole *newPothole(User *user, double latitude, double longitude, double variation)
{
    Pothole *pothole = malloc(sizeof(Pothole));
    pothole->user = user;
    pothole->latitude = latitude;
    pothole->longitude = longitude;
    pothole->variation = variation;
    return pothole;
}

