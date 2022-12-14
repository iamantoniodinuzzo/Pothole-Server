#ifndef USER_H
#define USER_H

#define USERNAME_LEN 50

typedef struct user
{
    char username[USERNAME_LEN];
    int client_fd;
} User;

#endif // !USER_H
