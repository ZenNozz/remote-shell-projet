#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX 10240 // Max number of bytes to be read from stream
#define PORT "4444" // Port on which to connect


// Finds the server's IPv4 address and attempts to connect to it.
// Returns a socket FD on success
int connectToServer(char* port, char* ip);