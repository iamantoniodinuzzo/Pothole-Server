#ifndef SERVER_H
#define SERVER_H
#include "../data_structure/entity/user.h"
#include "../data_structure/kdtree/kdtree.h"
#include <stdbool.h>

enum Command
{
    SET_USERNAME = 'u', // usage: u[username]
    NEW_HOLE = 'h',     // usage: h[lat;lng;variation]
    HOLE_LIST_BY_RANGE = 'r', // uage: r[lat;lng;range]
   // DELETE_HOLE = 'd', //usage d[lat;lng]
    TRESHOLD = 't',
    EXIT = 'e'
};

// return true if valid, else false
bool setUsername(User *, const char *);
// return true for continue, else false for exit
bool dispatch(User *, int, char *, KDTree *);

#endif // !SERVER_H