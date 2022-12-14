#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "server/server.h"

#define PORT 4444
#define SOCKETERROR (-1)
#define SERVER_BACKLOG 10
#define BUFSIZE 1024
#define THREAD_POOL_SIZE 10

typedef struct sockaddr_in SA_IN;
typedef struct sockaddr SA;

void error(const char *msg);
int check(int exp, const char *msg);
void *handle_connection(void *client_cocket);
void close_connection(User *user);


KDTree *tree;
int main()
{

    int server, client, ret;
    SA_IN server_addr, client_addr;
    socklen_t addr_size;

    printf("[+]Creating server socket\n");
    check(server = socket(PF_INET, SOCK_STREAM, 0), "[-]Error Creating server socket\n"); // TCP

    // initialize the address struct
    memset(&server_addr, '\0', sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // local server

    printf("[+]Binding server socket\n");
    check(bind(server, (SA *)&server_addr, sizeof(server_addr)), "[-]Error Binding.\n");

    printf("[+]Bind to port %d\n", PORT);

    //--- Load potholes from file
    tree = buildKDTree();

    check(listen(server, SERVER_BACKLOG), "[-]Error Listening.\n");

    while (true)
    {
        printf("Waiting for connections...\n");
        // wait for, and eventually accept an incoming connection
        check(client = accept(server, (SA *)&client_addr, &addr_size), "[-]Error client not accept.");

        printf("[+]New connection accepted from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        User *new_user = malloc(sizeof(User));
        new_user->client_fd = client;
        pthread_t t;
        pthread_create(&t, NULL, handle_connection, new_user);
        pthread_detach(t);

    } // while

    close(server);
    destroyTree(tree);

    return EXIT_SUCCESS;
}

// check about socket error and print an error message
int check(int exp, const char *err_msg)
{
    if (exp == SOCKETERROR)
    {
        error(err_msg);
    }
    return exp;
}

// print an error message and terminate program
void error(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

void *handle_connection(void *p_client_cocket)
{

    User *user = (User *)p_client_cocket;
    char buffer[BUFSIZE];
    ssize_t msglen;

    // check if message is not empty. If so, close connectionps
    msglen = recv(user->client_fd, buffer, BUFSIZE, 0);
    if (msglen <= 0)
    {
        close_connection(user);
    }
    buffer[msglen] = '\0';

    // ask to client to set an username
    if (!setUsername(user, buffer))
    {
        printf("Invalid username set by [%d], closing connection", user->client_fd);
        close_connection(user);
    }

    // after setting username, start dispatcher loop
    do
    {
        printf("Listening requests...\n");
        msglen = recv(user->client_fd, buffer, BUFSIZE, MSG_NOSIGNAL);
        if (msglen <= 0)
        {
            if (errno == EINTR)
                continue;
            close_connection(user);
            break;
        }
        buffer[msglen] = '\0';
        printf("Data recived from (%s) : %s\n", user->username, buffer);
    } while (dispatch(user, buffer[0], buffer + 1, tree));

    close_connection(user);
}

void close_connection(User *user)
{
    printf("Closing client (%s) connection...\n", user->username);
    if (close(user->client_fd) < 0)
    {
        perror("[-]Error in Closing connection.\n");
    }
    printf("Connection w/ (%s) closed\n", user->username);
    free(user);
    pthread_exit(NULL);
}
