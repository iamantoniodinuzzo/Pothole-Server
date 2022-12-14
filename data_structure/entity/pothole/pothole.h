#ifndef POTHOLE_H
#define POTHOLE_H
#include "../user.h"
#include <stdbool.h>

#define USERNAME_LEN 50

typedef struct pothole
{
    User *user;
    double latitude, longitude, variation;
} Pothole;

/*
Il criterio di confronto è il seguente:
- Se due buche sono nel raggio viene restituito 1
- Se due buche sono in un raggio superiore viene restituito -1
- Se due buche hanno le stesse coordinate allora viene restituito 0
*/
int comparePotholesByRange(Pothole *, Pothole *, int);
Pothole *newPothole(User *user, double latitude, double longitude, double variation);

#endif // !USER_H
