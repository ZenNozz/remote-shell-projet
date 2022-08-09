#include "../headers/server.h"

int createSocket() {
    return socket(AF_INET, SOCK_STREAM, 0);
}

int bindSocket(int socketFD, char* port, char* ip) {
    // Create socket
    int rv;
    struct addrinfo hints, *res, *binding;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    // Returns linked list of all results
    if (ip[0] == 0) { // '\0' == 0
        rv = getaddrinfo(NULL, port, &hints, &res);
    } else {
        rv = getaddrinfo(ip, port, &hints, &res);
    }
    if (rv != 0) {
        close(socketFD);
        printf("getaddrinfo: %s\n", gai_strerror(rv));
        return -1;
    }
    // Bind to the first address we can

    for (binding = res; res != NULL; res = res->ai_next) {
        if (bind(socketFD, binding->ai_addr, binding->ai_addrlen) == -1) {
            continue;
        } else {
            freeaddrinfo(res); // binding points to res, so both get freed
            return 0;
        }

    }
    return -1;
}

int acceptConnections(int socketFD) {
    struct sockaddr_in* peer = malloc(sizeof(struct sockaddr_in*)); 
    if (peer == NULL ) {
        return 0;
        }
    socklen_t peer_addr_size = sizeof(peer);
    int connectedSocketFD = accept(socketFD, (struct sockaddr*)peer, &peer_addr_size);
    free(peer);
    return connectedSocketFD;
}

void execShell(char* command, char* rv, long max) {
    char* str = malloc(1);
    if (str == NULL) {
        rv = NULL;
        return;
    }
    FILE* shellReturn;
    shellReturn = popen(command, "r"); // Execute the command
    while (fgets(str, max, shellReturn) != NULL) { 
        // Read the output of the command into rv
        strcat(rv, str); 
    }
    free(str);
    return;
}