#include "kdtree.h"
#include <stdio.h>
#include <stdlib.h>
#include "../../util/util.h"
#include <math.h>
#include <string.h>
#include <stdbool.h>

KDTree *newTree()
{
    KDTree *result = malloc(sizeof(KDTree));
    result->mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
    result->root = NULL;
    return result;
}

KDTree *buildKDTreeFromFile(const char *filename)
{
    KDTree *tree = newTree();
    FILE *file = fopen(filename, "r");
    assert(file);

    if (file != NULL)
    {
        printf("[+]Building tree...\n");
        char *currentLine = NULL;
        size_t strLen = 0;
        char insideBrachet[BUFSIZ];

        while (getline(&currentLine, &strLen, file) != -1)
        {
            stringInsideSquareBracket(currentLine, insideBrachet);
            double latitude = 0, longitude = 0, variation = 0;
            char *username = NULL;

            // get username
            char *token = strtok(insideBrachet, ";");
            if (token == NULL)
            {
                perror("[-]Username not valid\n");
                break;
            }
            username = token;

            // get latitude
            token = strtok(NULL, ";");
            if (token == NULL)
            {
                perror("[-]Latitude not valid!\n");
                break;
            }
            latitude = atof(token);

            // get longitude
            token = strtok(NULL, ";");
            if (token == NULL)
            {
                perror("[-]Longitude not valid!\n");
                break;
            }
            longitude = atof(token);

            // get variation
            token = strtok(NULL, ";");
            if (token == NULL)
            {
                perror("[-]Variation not valid!\n");
                break;
            }
            variation = atof(token);

            // create partial user
            User *user = malloc(sizeof(User));
            strcpy(user->username, username);
            Pothole *pothole = newPothole(user, latitude, longitude, variation);
            tree->root = insert(tree, pothole);
        }

        free(currentLine);
        fclose(file);
    }
    printf("[+]Tree built\n");

    return tree;
}

/*
Opens storage file (data/data.txt), uses it to build kd tree, then closes file and returns pointer to the tree.
*/
KDTree *buildKDTree()
{
    return buildKDTreeFromFile("data/data.txt");
}

Node *newNode(Pothole *pothole)
{
    Node *temp = malloc(sizeof(Node));

    temp->point[0] = pothole->latitude;
    temp->point[1] = pothole->longitude;
    temp->pothole = pothole;

    temp->left = temp->right = NULL;
    return temp;
}

// A utility function to find minimum of three integers
Node *minNode(Node *x, Node *y, Node *z, int d)
{
    Node *res = x;
    if (y != NULL && y->point[d] < res->point[d])
        res = y;
    if (z != NULL && z->point[d] < res->point[d])
        res = z;
    return res;
}

// Recursively finds minimum of d'th dimension in KD root
// The parameter depth is used to determine current axis.
Node *findMinRec(Node *root, int d, unsigned depth)
{
    // Base cases
    if (root == NULL)
        return NULL;

    // Current dimension is computed using current depth and total
    // dimensions (k)
    unsigned cd = depth % 2;

    // Compare point with root with respect to cd (Current dimension)
    if (cd == d)
    {
        if (root->left == NULL)
            return root;
        return findMinRec(root->left, d, depth + 1);
    }

    // If current dimension is different then minimum can be anywhere
    // in this subtree
    return minNode(root,
                   findMinRec(root->left, d, depth + 1),
                   findMinRec(root->right, d, depth + 1), d);
}

// A wrapper over findMinRec(). Returns minimum of d'th dimension
Node *findMin(Node *root, int d)
{
    // Pass current level or depth as 0
    return findMinRec(root, d, 0);
}

// A utility method to determine if two Points are same
// in K Dimensional space
bool arePointsSame(double point1[], double point2[])
{
    // Compare individual pointinate values
    for (int i = 0; i < 2; ++i)
        if (point1[i] != point2[i])
            return false;

    return true;
}

// Copies point p2 to p1
void copyPoint(double p1[], double p2[])
{
    for (int i = 0; i < 2; i++)
        p1[i] = p2[i];
}

/*
Inserts struct entry into tree at correct coordinates.
Each node stores its own coordinates rather than pointing to the ones held in the struct.
 The parameter depth is used to decide axis of comparison
 */
Node *insertRec(Node *root, Node *new_node, int depth)
{
    // Tree is empty?
    if (root == NULL)
    {
        return new_node;
    }

    // Calculate current dimension (cd) of comparison
    unsigned cd = depth % 2;

    // Compare the new point with root on current dimension 'cd'
    // and decide the left or right subtree
    if (new_node->point[cd] < (root->point[cd]))
    {
        root->left = insertRec(root->left, new_node, depth + 1);
    }
    else
    {
        root->right = insertRec(root->right, new_node, depth + 1);
    }

    return root;
}

// Inserts a new pothole within the tree by returning its root.
// The insert is thread safe, the function acquires the mutex of the tree.
Node *insert(KDTree *tree, Pothole *pothole)
{
    pthread_mutex_lock(&tree->mutex);
    Node *result = insertRec(tree->root, newNode(pothole), 0);
    pthread_mutex_unlock(&tree->mutex);

    return result;
}

// Function to delete a given pothole from root with root
// as 'root'.  depth is current depth and passed as 0 initially.
// Returns root of the modified root.
Node *deleteNodeRec(Node *root, Node *pothole, int depth)
{
    // Given point is not present
    if (root == NULL)
        return NULL;

    // Find dimension of current node
    int cd = depth % 2;

    // If the point to be deleted is present at root
    if (arePointsSame(root->point, pothole->point))
    {
        // 2.b) If right child is not NULL
        if (root->right != NULL)
        {
            // Find minimum of root's dimension in right subtree
            Node *min = findMin(root->right, cd);

            // Copy the minimum to root
            copyPoint(root->point, min->point);

            // Recursively delete the minimum
            root->right = deleteNodeRec(root->right, min, depth + 1);
        }
        else if (root->left != NULL) // same as above
        {
            Node *min = findMin(root->left, cd);
            copyPoint(root->point, min->point);
            root->right = deleteNodeRec(root->left, min, depth + 1);
        }
        else // If node to be deleted is leaf node
        {
            free(root);
            return NULL;
        }
        return root;
    }

    // 2) If current node doesn't contain point, search downward
    if (pothole->point[cd] < root->point[cd])
    {
        root->left = deleteNodeRec(root->left, pothole, depth + 1);
    }
    else
    {
        root->right = deleteNodeRec(root->right, pothole, depth + 1);
    }

    return root;
}

// Function to delete a given point from K D Tree.
// This function is thread safe.
Node *deleteNode(KDTree *tree, Pothole *pothole)
{
    pthread_mutex_lock(&tree->mutex);
    Node *result = deleteNodeRec(tree->root, newNode(pothole), 0);
    pthread_mutex_unlock(&tree->mutex);
    return result;
}

void freeTree(Node *node)
{
    if (node)
    {
        freeTree(node->left);
        freeTree(node->right);
        free(node);
    }
}

void destroyTree(KDTree *tree)
{
    freeTree(tree->root);
    pthread_mutex_destroy(&tree->mutex);
}

// Ticker is used to alternate key comparisons between latitude and longitude
void radiusSearch(Node *root, int radius, Pothole *center, int ticker, List *in_radius)
{

    if (root == NULL)
    {
        return;
    }

    if (comparePotholesByRange(center, root->pothole, radius) == 1)
    {
        // write node (root->pothole) to the list
        addToList(root->pothole, in_radius); // this not build the list
        // go down both branches
        if (ticker == 0)
        {
            ticker = 1;
        }
        else
        {
            ticker = 0;
        }
        radiusSearch(root->left, radius, center, ticker, in_radius);
        radiusSearch(root->right, radius, center, ticker, in_radius);
    }
    else
    {
        // the current node is outside the radius
        if (ticker == 0)
        {
            // check center latitude value of key
            // first, check if current node partitions the radius on both sides
            // fabs used to get the absolute value of a floating point number.
            // This function returns the absolute value in double
            if (fabs(center->latitude - root->point[0]) <= radius)
            {
                // If it does; go down both branches
                radiusSearch(root->left, radius, center, 1, in_radius);
                radiusSearch(root->right, radius, center, 1, in_radius);
            }
            else if (center->latitude < root->point[0])
            {
                // Go left
                radiusSearch(root->left, radius, center, 1, in_radius);
            }
            else
            {
                // Go right
                radiusSearch(root->right, radius, center, 1, in_radius);
            }
        }
        else if (ticker == 1)
        {
            // check center longitude value of key
            // first, check if current node partitions the radius on both sides
            if (fabs(center->longitude - root->point[1]) <= radius)
            {
                // If it does; go down both branches
                radiusSearch(root->left, radius, center, 0, in_radius);
                radiusSearch(root->right, radius, center, 0, in_radius);
            }
            else if (center->longitude < root->point[1])
            {
                // Go left
                radiusSearch(root->left, radius, center, 0, in_radius);
            }
            else
            {
                // Go right
                radiusSearch(root->right, radius, center, 0, in_radius);
            }
        }
    }
    return;
}

// Allows searching for all points near a center defined a radius.
// This research is thread safe.
List *search_by_radius(KDTree *tree, int radius, Pothole *center)
{
    List *in_radius = makelist();
    pthread_mutex_lock(&tree->mutex);
    radiusSearch(tree->root, radius, center, 0, in_radius);
    pthread_mutex_unlock(&tree->mutex);
    return in_radius;
}

// Function to print binary tree in 2D
// It does reverse inorder traversal
void print2DUtil(Node *root, int space)
{
    // Base case
    if (root == NULL)
        return;

    // Increase distance between levels
    space += 10;

    // Process right child first
    print2DUtil(root->right, space);

    // Print current node after space
    // count
    printf("\n");
    for (int i = 10; i < space; i++)
        printf(" ");
    printf("%s [%f,%f]\n", root->pothole->user->username, root->pothole->latitude, root->pothole->longitude);

    // Process left child
    print2DUtil(root->left, space);
}

/*
Prints tree in traditional sorted order locking mutex
*/
void printTree(KDTree *tree)
{
    pthread_mutex_lock(&tree->mutex);
    if (tree->root == NULL)
    {
        printf("[-]Tree is empty\n");
        pthread_mutex_unlock(&tree->mutex);
        return;
    }
    // treeprint(tree->root,0);
    print2DUtil(tree->root, 0);
    pthread_mutex_unlock(&tree->mutex);
}