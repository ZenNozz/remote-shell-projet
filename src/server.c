#include "../headers/server.h"
int createSocket() {
    return socket(AF_INET, SOCK_STREAM, 0);
}

int bindSocket(int socketFD, char* port, char* ip) {
    // Create socket
    int rv;
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    // Returns linked list of all results
    if (ip[0] == 0) {
        rv = getaddrinfo(NULL, port, &hints, &res);
    } else {
        rv = getaddrinfo(ip, port, &hints, &res);
    }
    
    if (rv != 0) {
        close(socketFD);
        printf("getaddrinfo: %s\n", gai_strerror(rv));
        return -1;
    }
    // Try to bind the socket to first result
    if (bind(socketFD, res->ai_addr, res->ai_addrlen) == -1) {
        freeaddrinfo(res);
        close(socketFD);
        return -1;
    }
    freeaddrinfo(res);
    return 0;
}

int acceptConnections(int socketFD) {
    struct sockaddr_in* peer = malloc(sizeof(struct sockaddr_in*));
    socklen_t peer_addr_size = sizeof(peer);
    int connectedSocketFD = accept(socketFD, (struct sockaddr*)peer, &peer_addr_size);
    free(peer);
    if (connectedSocketFD == -1) {
        return -1;
    }
    return connectedSocketFD;
}

char* execShell(char* command, char* rv, long max) {
    char* str = malloc(max);
    if (str == NULL) {
        return NULL;
    }
    FILE* shellReturn;
    shellReturn = popen(command, "r"); // Execute the command
    while (fgets(str, max, shellReturn) != NULL) { 
        // Read the output of the command into rv
        strcat(rv, str); 
    }
    free(str);
    return rv;
}