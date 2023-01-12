#include "pothole.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
/*
Haversine formula to find distance between two points on a sphere
*/
bool isIntoRadius(const double cx, const double cy, int radius, const double x, const double y)
{
  
    // distance between latitudes
        // and longitudes
        double radius_km = (double)radius/1000;
        double dLat = (x - cx) *
                      M_PI / 180.0;
        double dLon = (y - cy) *
                      M_PI / 180.0;
 
        // convert to radians
        double n_cx = (cx) * M_PI / 180.0;
        double n_x = (x) * M_PI / 180.0;
 
        // apply formulae
        double a = pow(sin(dLat / 2), 2) +
                   pow(sin(dLon / 2), 2) *
                   cos(n_cx) * cos(n_x);
        double rad = 6371;
        double c = 2 * asin(sqrt(a));

        double distance = rad * c;

    return (distance > radius_km) ? false : true;
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

