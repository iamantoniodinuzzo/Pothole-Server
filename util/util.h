#ifndef UTIL_H
#define UTIL_H

#include "../data_structure/entity/pothole/pothole.h"
#include "../data_structure/linked_list/list.h"
#include <stdbool.h>
#include <assert.h>


/*
Gets the string between the square brackets 'in', and stores it in the string 'out'.
 Return length of substring.
 Return -1 when not found or out[] too small.
*/
long stringInsideSquareBracket(const char *in, long out_size, char *out);

bool writeOnFile(const char *filename, Pothole *pothole);
/*
Build and return a json string from a list of photoles.
{
    potholes:[
        {"user":<username>,"lat":<latitude>,"lng":<longitude>,"var":<variation>},
        //...
    ]
}
*/
void buildJsonString(const list_node *node, char *to_send);

double calculateThreasholdFromFile(const char *filename);

#endif // !UTIL_H