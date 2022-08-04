#include "headers/server.h"
#include <errno.h>

int main(int argc, char* argv[]) {
    // Check for command line parameters
    int verbose = 0;
    char port[5];
    char ip[15] = {0};
    long max = MAX;
    strcpy(port, PORT);

    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            if (strcmp(argv[i], "-v") == 0) {
                verbose = 1;
            } else {
                char arg[3]; 
                memcpy(arg, argv[i], 2);
                if (strcmp(arg, "-p") == 0) {
                    char cpy[strlen(argv[i])];
                    strcpy(cpy, argv[i]);
                    char token[5];
                    strcpy(token, strtok(cpy, "="));
                    strcpy(token, strtok(NULL, "="));
                    long port_long = strtol(token, NULL, 10);
                    if (port_long < 0 || port_long > 65535 || errno != 0) {
                        printf("Parameter must be between 0 and 65535\n");
                        return 0;
                    } else {
                        strcpy(port, token);
                    }
                } else if (strcmp(arg, "-b") == 0) {
                    char cpy[strlen(argv[i])];
                    strcpy(cpy, argv[i]); 
                    char token[4];
                    strcpy(token, strtok(cpy, "="));
                    strcpy(token, strtok(NULL, "="));
                    long bytes_long = strtol(token, NULL, 10);
                    if (bytes_long < 1 || bytes_long > 10240 || errno != 0) {
                        printf("Parameter must be between 1 and 10240\n");
                        return 0;
                    } else {
                        max = bytes_long;
                    }
                } else if (strcmp(arg, "-i") == 0) {
                    char cpy[strlen(argv[i])];
                    strcpy(cpy, argv[i]);
                    char token[15]; 
                    strcpy(token, strtok(cpy, "="));
                    strcpy(token, strtok(NULL, "="));
                    if (inet_addr(token) == -1) {
                        // -1 corresponds to a valid IP address, however it's the broadcast address
                        printf("Invalid IP address\n");
                        return 0;
                    }
                    // TODO: check for reserved IP addresses
                    strcpy(ip, token);
                } else {
                    printf("Command line arguments are\n");
                    printf("-v : verbose mode - show all execution messages\n");
                    printf("-i=N.N.N.N : sets server IP to N.N.N.N (IPv4)\n");
                    printf("-p=N : sets server port to N (0<N<65535)\n");
                    printf("-b=N : sets maximum bytes per communication (0<N<2048)\n");
                    return 0;
                }
            }
        }
    }

    // Create and bind socket
    int boundSocket = createSocket();
    if (boundSocket == -1) {
        if (verbose) {
            printf("Error creating socket\n");
        }
        return 0;
    } else {
        if (verbose) {
            printf("Socket created\n");
        }
    }
    if (bindSocket(boundSocket, port, ip) == -1) {
        if (verbose) {
            printf("Error binding socket\n");
        }
        return 0;
    } else {
        if (verbose) {
            printf("Socket bound\n");
        }
    }

    if (verbose) {
        printf("Server started with the following parameters\n");
        if (ip[0] == 0) {
            printf("Accepting all connections on port %s\n", port);
        } else {
            printf("Server started with the following parameters\n");
        }
        printf("Max bytes/communication: %ld\n", max);
    }
    
    // Start listening for connections
    if (listen(boundSocket, BACKLOG) == -1) {
        if (verbose) {
            printf("Error listening\n");
        }
        return 0;
    } else {
        if (verbose) {
            printf("Started listening\n");
        }
    }


    // Initialize variables needed for communication
    int peerSocket;
    int bytesReceived;
    int bytesSent;
    void* recvBuf = malloc(max); // Avoid compiler warning
    void* sendBuf = malloc(max); // Avoid compiler warning
    if (recvBuf == NULL || sendBuf == NULL) {
        if (verbose) {
            printf("malloc() call failed\n");
        }
        return 0;
    }

    // Accept loop
    while((peerSocket = acceptConnections(boundSocket))) {
        // Check if a client is attempting to connect,
        // and if the attempt fails, wait one second
        if (peerSocket == -1) {
            if (verbose) {
                printf("Failed to accept client's connection\n");
            }
            sleep(1);
            continue;
        }
        if (verbose) {
            printf("Connection accepted\n");
        }

        // Receive message from client. If the message fails to send,
        // close the connection and wait one second
        bytesReceived = recv(peerSocket, recvBuf, max, 0);

        if (bytesReceived == -1) {
            if (verbose) {
                printf("Failed to receive message\n");
            }
            close(peerSocket);
            sleep(1);
            continue;

        } else {
            if (bytesReceived == 0) {
                // If no bytes are sent, close the connection
                if (verbose) {
                    printf("Connection with client closed\n");
                }
                close(peerSocket);
                continue;
            } else {
                // Execute command on local shell
                if (verbose) {
                    printf("%d bytes received from client\n", bytesReceived);
                    
                }
                bytesReceived = 0;
                // Get rid of trailing newlines from fgets
                char checkexit[strlen((char*)recvBuf)];
                strcpy(checkexit, (char*)recvBuf);
                checkexit[strcspn(checkexit, "\r\n")] = '\0';

                if (strcmp("closeShell", checkexit) == 0) {
                    free(recvBuf);
                    free(sendBuf);
                    close(peerSocket);
                    close(boundSocket);
                    if (verbose) {
                        printf("Server closing\n");
                    }
                    return 1;
                } else {
                    execShell((char*)recvBuf, (char*)sendBuf, max);
                    if (sendBuf == NULL) {
                        if (verbose) {
                            printf("Failed to execute the command\n");
                        }
                        continue;
                    }
                    if (verbose) {
                        printf("Command return:\n\n%s\n", (char*)sendBuf);
                    }

                    // Send execution results to client
                    bytesSent = send(peerSocket, sendBuf, strlen(sendBuf), 0);
                    if (verbose) {
                        printf("%d bytes sent to client\n", bytesSent);
                    }
                    memset(sendBuf, 0, strlen(sendBuf));
                    memset(recvBuf, 0, strlen(recvBuf));
                }
            }
        }
    }
}