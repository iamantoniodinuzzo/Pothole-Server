#ifndef LIST_H
#define LIST_H
#include "../entity/pothole/pothole.h"

typedef struct list_node
{
    Pothole *pothole;
    struct list_node *next;
} list_node;

typedef struct list
{
    list_node *head;
} List;

List *makelist();
void addToList(Pothole *data, List *list);
void displayList(List *list);
void destroyList(List *list);


#endif // !LIST_H