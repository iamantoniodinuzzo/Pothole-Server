#ifndef UTIL_H
#define UTIL_H

#include "../data_structure/entity/pothole/pothole.h"
#include "../data_structure/linked_list/list.h"
#include <stdbool.h>
#include <assert.h>

/*
Gets the string between the square brackets 'in', and stores it in the string 'out'.
Return string dimension.
*/
int stringInsideSquareBracket(const char *input, char *output);
bool writeOnFile(const char *filename, Pothole *pothole);
void buildJsonString(const list_node *node, char *to_send);

#endif // !UTIL_H