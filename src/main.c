#include "main.h"

pthread_t           *streamThread;
struct addrinfo     *hints;
struct addrinfo     *rp;
struct addrinfo     **result;
int                 error;
int                 *serverSock; 
int                 *clientSock;
struct sockaddr     *clientAddr;
socklen_t           *clientAddrLen;
int                 indexThread;

void handle_sigint(int sig)
{
    free(streamThread);
    free(hints);
    free(rp);
    free(*result);
    close(*serverSock);
    if (serverSock != NULL)
        free(serverSock);
    close(*clientSock);
    free(clientSock);
    free(clientAddr);
    printf("End Program\n");
}
        
void handle_sigterm(int sig)
{
    free(streamThread);
    free(hints);
    free(rp);
    free(*result);
    close(*serverSock);
    close(*clientSock);
    free(clientSock);
    free(clientAddr);
    printf("End Program\n");
}

void handle_sigquit(int sig)
{
    free(streamThread);
    free(hints);
    free(rp);
    free(*result);
    close(*serverSock);
    close(*clientSock);
    free(clientSock);
    free(clientAddr);
    printf("End Program\n");
}
void *stream (void *param)
{
    int clientFd = *((int *)param);
    char *helloMsg = "Hello Client";
    char buf[1024];
    char sendMsg[1024];
    send(clientFd, helloMsg, strlen(helloMsg), 0);

    do
    {
        error = recv(clientFd, buf, 1024, 0);
    }
    while (error < 0);
    
    printf("Client: %s\n", buf);
    bzero(buf, 1024);
    printf("Start Chat\n");
    while (true)
    {
        printf("Enter msg: \n");
        fflush(stdin);
        scanf("%[^\n]s", sendMsg);
        send(clientFd, sendMsg, strlen(sendMsg), 0);
        bzero(sendMsg, 1024);
        printf("Wait for client\n");
        do
        {
            error = recv(clientFd, buf, 1024, 0);
        }
        while (error < 0);
        printf("Client: %s\n", buf);
        if (strcmp(buf, "END") == 0)
        {
            break;
        }
        bzero(buf, 1024);
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "usage: %s port\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    printf("Start Program\n");

    signal(SIGINT, handle_sigint);
    signal(SIGQUIT, handle_sigquit);
    signal(SIGTERM, handle_sigterm);

    serverSock = (int *)malloc(sizeof(int));
    if (serverSock == NULL)
    {
        fprintf(stderr, "[ERROR] Not enough space to allocate socket file description");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Allocated Socket FD\n");
    }

    hints = (struct addrinfo *)malloc(sizeof(struct addrinfo));
    if (hints == NULL)
    {
        fprintf(stderr, "[ERROR] Not enough memory!\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Allocated Hints\n");
    }

    memset(hints, 0, sizeof(*hints));
    hints->ai_addr = NULL;
    hints->ai_canonname = NULL;
    hints->ai_family = AF_INET;
    hints->ai_flags = AI_PASSIVE;
    hints->ai_next = NULL;
    hints->ai_protocol = IPPROTO_TCP;
    hints->ai_socktype = SOCK_STREAM;

    result = (struct addrinfo **)malloc(sizeof(struct addrinfo *));
    if (result == NULL)
    {
        fprintf(stderr, "[ERROR] Not enough space\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Allocated Getaddrinfo Result\n");
    }
 
    error = getaddrinfo(NULL, argv[1], hints, result);
    if (error != 0)
    {
        fprintf(stderr, "[ERROR] getaddrinfo: %s\n", gai_strerror(error));
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Getaddrinfo Success\n");
    }

    for (rp = *result; rp != NULL; rp = rp->ai_next)
    {
        *serverSock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (*serverSock < 0)
        {
            continue;
        }
        else
        {
            printf("Create Socket Success\n");
        }

        if (bind(*serverSock, rp->ai_addr, rp->ai_addrlen) == 0)
        {
            printf("Bind Success\n");
            break;
        }

        close(*serverSock);
    }

    if (rp == NULL)
    {
        fprintf(stderr, "[ERROR] Could not bind\n");
        exit(EXIT_FAILURE);
    }    
    if (setsockopt(*serverSock, SOL_SOCKET, SO_REUSEPORT, &(int){1}, sizeof(int)) < 0)
        fprintf(stderr,"setsockopt(SO_REUSEADDR) failed");

    freeaddrinfo(*result);
    printf("Initial Program Success\n");

    if ((error = listen(*serverSock, 5)) != 0)
    {
        fprintf(stderr, "[ERROR] Listen Failed: %s\n", strerror(error));
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Listenning....\n");
    }

    indexThread = 0;
    streamThread = (pthread_t *)malloc(sizeof(pthread_t) * 5);
    if (streamThread == NULL)
    {
        fprintf(stderr, "[ERROR] NOt enough space to create thread\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Created Thread For Program\n");
    }

    clientSock = (int *)malloc(sizeof(int));
    if (clientSock == NULL)
    {
        fprintf(stderr, "[ERROR] Not enough space to create client socket\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Allocated Socket for Client\n");
    }

    while (true)
    {
        printf("Waiting for client connect\n");
        clientAddr = (struct sockaddr *)malloc(sizeof(struct sockaddr));
        if (clientAddr == NULL)
        {
            fprintf(stderr, "[ERROR] Not enough space memory\n");
            continue;
        }
        else
        {
            printf("Allocated client address\n");
        }

        clientAddrLen = (socklen_t *)malloc(sizeof(socklen_t));
        if (clientAddrLen == NULL)
        {
            fprintf(stderr, "[ERROR] Not enough space to allocated client address len\n");
            continue;
        }
        *clientAddrLen = (socklen_t)sizeof(clientAddr);
        *clientSock = accept(*serverSock, clientAddr, clientAddrLen);
        if (*clientSock < 0)
        {
            fprintf(stderr, "[ERROR] Cannot accept client: %s\n", strerror(errno));
            continue;
        }
        else
        {
            printf("Accepted Client\n");
        }

        if (pthread_create(streamThread + indexThread, NULL, stream, clientSock) != 0)
        {
            printf("Have Create Thread at Index: %d\n", indexThread);
        }

        indexThread++;
        if (indexThread >= 5)
        {
            indexThread = 0;
            while (indexThread < 5)
            {
                pthread_join(*(streamThread + indexThread), NULL);
                indexThread++;
            }
            indexThread = 0;
        }
    }

    exit(EXIT_SUCCESS);
}