#! /bin/bash
#need '-lm' for math.h library

gcc -pthread -o main \
     server/server.c \
     util/util.c \
     data_structure/entity/pothole/pothole.c \
     data_structure/kdtree/kdtree.c \
     data_structure/linked_list/list.c \
     main.c -lm 
