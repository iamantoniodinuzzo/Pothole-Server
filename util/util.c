#include "util.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../data_structure/kdtree/kdtree.h"
#include "../data_structure/entity/user.h"


int stringInsideSquareBracket(const char *in,char *out)
{
    /*
    %*[^(]  read and discard everything until it finds a square bracket
    (       read and discard the first square bracket
    %[^)]   read and store up up to (but not including) the closing square bracket
    %*[^\n] read and discard up to (but not including) the newline
    */
    return sscanf(in, "%*[^[][%[^]]%*[^\n]", out);
}

bool writeOnFile(const char *filename, Pothole *pothole)
{
    FILE *file = fopen(filename, "a+");
    assert(file);

    fprintf(file, "[%s;%f;%f;%f]\n", pothole->user->username, pothole->latitude, pothole->longitude, pothole->variation);
    fclose(file);
    return true;
}

/*
Build and return a json string from a list of photoles.
{
    potholes:[
        {"user":<username>,"lat":<latitude>,"lng":<longitude>,"var":<variation>},
        //...
    ]
}
*/
void buildJsonString(const list_node *node, char *json_string)
{
    char tmp[1000];

    if (node != NULL)
    {
        sprintf(tmp, "{\"user\":%s,\"lat\":%f,\"lon\":%f,\"var\":%f}", node->pothole->user->username, node->pothole->latitude, node->pothole->longitude, node->pothole->variation);

        if (node->next != NULL)
            strcpy(tmp, strcat(tmp, ","));
        else
        {
            strcpy(tmp, strcat(tmp, "]}\n"));
            tmp[strlen(tmp)] = '\0';
        }
        strcpy(json_string, strcat(json_string, tmp));
        buildJsonString(node->next, json_string);
    }
}