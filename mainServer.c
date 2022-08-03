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