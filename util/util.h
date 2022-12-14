#ifndef UTIL_H
#define UTIL_H

#include "../data_structure/entity/pothole/pothole.h"
#include "../data_structure/linked_list/list.h"
#include <stdbool.h>
#include <assert.h>


// Copy substring 'in', between left and right delimiters, into 'out'.
// return dimension
int stringInside(const char *, char, char, char *, int);
void stringInsideSquareBrachet(const char *, char *);
bool writeOnFile(const char *filename, Pothole *pothole);
void buildJsonString(const list_node* node, char *to_send);


#endif // !UTIL_H