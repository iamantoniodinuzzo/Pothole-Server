#include "server.h"
#include "../data_structure/entity/pothole/pothole.h"
#include "../data_structure/kdtree/kdtree.h"
#include "../util/util.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <sys/socket.h>

void sendMsg(const User *user, const char *msg)
{
    if (send(user->client_fd, msg, strlen(msg), 0) == -1)
    {
        perror("[-]Error in sending\n");
        return;
    }
}

bool setUsername(User *user, const char *msg)
{
    char username[USERNAME_LEN];
    long len = stringInsideSquareBracket(msg, sizeof(username), username);

    for (long i = 0; i < len; ++i)
    {
        if (username[i] != ' ') // check if username is not empty
        {
            strcpy(user->username, username);
            printf("Username is set to [%s].\n", user->username);
            return true;
        }
    }

    return false;
}

void add_hole(User *user, const char *msg, KDTree *tree)
{
    char buffer[2064];
    double latitude = 0, longitude = 0, variation = 0;
    buffer[sizeof(buffer)] = '\0';

    int len = stringInsideSquareBracket(msg, sizeof(buffer), buffer);

    // get latitude
    char *token = strtok(buffer, ";");
    if (token == NULL)
    {
        perror("[-]Latitude not valid!\n");
        return;
    }
    latitude = atof(token);

    // get longitude
    token = strtok(NULL, ";");
    if (token == NULL)
    {
        perror("[-]Longitude not valid!\n");
        return;
    }
    longitude = atof(token);

    // get variation
    token = strtok(NULL, ";");
    if (token == NULL)
    {
        perror("[-]Variation not valid!\n");
        return;
    }
    variation = atof(token);

    // build new pothole
    Pothole *new_pothole = newPothole(user, latitude, longitude, variation);

    // add new pothole into tree
    tree->root = insert(tree, new_pothole);

    // add new pothole into file
    if (!writeOnFile("data/data.txt", new_pothole))
        printf("[-]This pothole is not memrize into file!\n");
    else
        printf("[+]Store into file successfully!\n");

    printf("User (%s) added a pothole [lat:%f, lng:%f ,var:%f]\n", new_pothole->user->username, new_pothole->latitude, new_pothole->longitude, new_pothole->variation);
}

void send_holes(const User *user, KDTree *tree)
{
    printf("Sending potholes to user (%s)\n", user->username);
    // send all tree
    printf("- - - The tree - - -\n");
    printTree(tree);
}

void send_threshold(const User *user, KDTree *tree)
{
    char threshold[5];
    sprintf(threshold, "%d\n", 15);
    sendMsg(user, threshold);
    printf("Sending threshold (%s) to user (%s)\n", threshold, user->username);
}

void send_holes_by_range(User *user, char *msg, KDTree *tree)
{
    char buffer[2064];
    stringInsideSquareBracket(msg, sizeof(buffer), buffer);
    double latitude = 0, longitude = 0;
    int range = 0;

    // get latitude
    char *token = strtok(buffer, ";");
    if (token == NULL)
    {
        perror("[-]Latitude not valid!");
        return;
    }
    latitude = atof(token);

    // get longitude
    token = strtok(NULL, ";");
    if (token == NULL)
    {
        perror("[-]Longitude not valid!");
        return;
    }
    longitude = atof(token);

    // get range
    token = strtok(NULL, ";");
    if (token == NULL)
    {
        perror("[-]Range not valid!");
        return;
    }
    range = atoi(token);

    // build new pothole
    // this pothole is not used for its true nature but rather stands for the location of a user,
    // as the center of the circumference of a given radius
    Pothole *center = malloc(sizeof(Pothole));
    center->user = user;
    center->latitude = latitude;
    center->longitude = longitude;

    printf("Find potholes within a radius (%d), from the center (lat:%f, lng:%f) for user (%s)\n", range, latitude, longitude, user->username);

    // get all potholes by radius
    List *result = search_by_radius(tree, range, center);
    // build JSON as response
    char json[50000] = "{\"potholes\":[";
    buildJsonString(result->head, json);
    sendMsg(user, json);

    /*char to_send[50000] = {0};
    sprintf(to_send, "%s", json);
    sendMsg(user, to_send);*/

    destroyList(result);

    json[sizeof(json)] = '\0';
}

bool dispatch(User *user, int command, char *msg, KDTree *tree)
{
    switch (command)
    {
    case NEW_HOLE:
        printf("User (%s) want add a hole\n", user->username);
        add_hole(user, msg, tree);
        return true;
    case HOLE_LIST:
        printf("User (%s) want list of hole\n", user->username);
        send_holes(user, tree);
        return true;
    case TRESHOLD:
        printf("User (%s) want threashold\n", user->username);
        send_threshold(user, tree);
        return true;
    case HOLE_LIST_BY_RANGE:
        printf("User (%s) want list of hole by range\n", user->username);
        send_holes_by_range(user, msg, tree);
        return true;
    case DELETE_HOLE:
        printf("User (%s) want delete a hole\n", user->username);
        return true;
    case EXIT:
        printf("User (%s) want exit\n", user->username);
        return false;
    default:
        printf("Unknown command (%d) by user (%s), full message: %s\n", command, user->username, msg);
        return false;
    }
    return true;
}