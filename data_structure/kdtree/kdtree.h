#ifndef KDTREE_H
#define KDTREE_H
#include "../entity/pothole/pothole.h"
#include "../linked_list/list.h"
#include <pthread.h>

// A structure to represent node of kd tree
// 0 latitude and 1 longitude
typedef struct node
{
    Pothole *pothole;
    double point[2]; // To store 2 dimensional point
    struct node *left, *right;

} Node;

typedef struct kdtree
{
    pthread_mutex_t mutex;
    Node *root;
} KDTree;

KDTree *buildKDTree();
Node *insert(KDTree *tree, Pothole *pothole);
Node *deleteNode(KDTree *tree, Pothole *pothole);
/*
Searches through root for points inside given radius.
If a node is inside the radius, is added to a result list.
*/
List *search_by_radius(KDTree *tree, int radius, Pothole *center);
/*
Shows me if my tree looks how it should during testing
Prints tree in traditional sorted order.
*/
void printTree(KDTree *tree);
void destroyTree(KDTree *tree);

#endif // !KDTREE_H