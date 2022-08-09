#include "headers/client.h"
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
                char arg[3]; // -p=2048 2048
                memcpy(arg, argv[i], 2);
                if (strcmp(arg, "-p") == 0) {
                    char cpy[strlen(argv[i])]; // -p=2048
                    strcpy(cpy, argv[i]);
                    char token[5];
                    strcpy(token, strtok(cpy, "=")); // -p
                    strcpy(token, strtok(NULL, "=")); // 2048
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
                    char token[5];
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

    if (verbose) {
        printf("Connecting to remote shell with the following parameters\n");
        if (ip[0] == 0) {
            printf("Port: %s\n", port);
        } else {
            printf("IP Address: %s:%s\n", ip, port);
        }
        printf("Max bytes/communication: %ld\n\n", max);
    }
    // Initialize variables needed for communication
    void* recvBuf = malloc(max);
    void* sendBuf = malloc(max);
    if (recvBuf == NULL || sendBuf == NULL) {
        if (verbose) {
            printf("malloc() call failed\n");
        }
        return 0;
    }
    
    int serverSocketFD;
    int bytesSent;
    int bytesReceived;

    

    // Connection loop
    while((serverSocketFD = connectToServer(port, ip))) {
        // Check if connection is successful, otherwise wait one second before retrying
        if (serverSocketFD == -1) {
            if (verbose) {
                printf("getaddrinfo() call failed\n");
            }
            sleep(1);
            continue;
 
        } else if (serverSocketFD == -2) {
            if (verbose) {
                printf("No servers could be found\n");
            }
            sleep(1);
            continue;

        } else if (serverSocketFD == -3) {
            if (verbose) {
                printf("Failed to create socket\n");
            }
            sleep(1);
            continue;
        } else {
            printf("Successfully connected to server\n");
        }

        // Get command to execute on remote shell
        if (verbose) {
            printf("Enter a command\n");
        }
        fgets((char*)sendBuf, max, stdin);
        // Send command to remote shell
        if (verbose) {
            printf("Sending command\n");
        }
        bytesSent = send(serverSocketFD, sendBuf, strlen(sendBuf), 0);
            if (bytesSent == -1) {
                if (verbose) {
                    printf("Failed to send\n");
                }
                close(serverSocketFD);
                continue;
            } else {
                if (verbose) {
                    printf("%d bytes sent\n", bytesSent);
                }
                bytesSent = 0;
            }
        // Receive execution result from remote shell
        bytesReceived = recv(serverSocketFD, recvBuf, max, 0);
            if (bytesReceived == -1) {
                if (verbose) {
                    printf("Failed to receive\n");
                }
                close(serverSocketFD);
                continue;
            } else {
                if (verbose) {
                    printf("%d bytes received\n", bytesReceived);
                }
            }

        // Get rid of trailing new lines from fgets
        char checkexit[strlen((char*)sendBuf)];
        strcpy(checkexit, (char*)sendBuf);
        checkexit[strcspn(checkexit, "\r\n")] = '\0'; 

        if (strcmp(checkexit, "closeShell") == 0) {
            if (verbose) {
                printf("Server closed\n");
            }
            // Exit shell 
            free(recvBuf);
            free(sendBuf);
            close(serverSocketFD);
            return 1;
        }

        // Print execution to local shell
        printf("\n%s\n", (char*)recvBuf);
        memset(sendBuf, 0, strlen((char*)sendBuf));
        memset(recvBuf, 0, strlen((char*)recvBuf));
        close(serverSocketFD);
    }
}