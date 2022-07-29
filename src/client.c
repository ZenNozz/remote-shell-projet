#include "../headers/client.h"

int connectToServer(char* port, char* ip) {
	// Retrieve server address
	struct addrinfo hints, *res;
	// Initialize all hints to 0
	memset(&hints, 0, sizeof(struct addrinfo));
	int rv;
	int serverSocket;
	// Server is IPv4, and socket type is STREAM
    hints.ai_family = AF_INET; // INET = IPv4
    hints.ai_socktype = SOCK_STREAM;
	
    // Returns linked list of all results
	if (ip[0] == 0) {
		rv = getaddrinfo(NULL, port, &hints, &res);
	} else {
		rv = getaddrinfo(ip, port, &hints, &res);
	}
	
	if (rv != 0) {
		return -1;
	}

	// No results
	if (res == NULL) {
		return -2;
	}

	// Attempt to connect to first result
	serverSocket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (serverSocket == -1) {
		return -3;
	}
	if (connect(serverSocket, res->ai_addr, res->ai_addrlen) == -1) {
		return -4;
	}

	freeaddrinfo(res);
	return serverSocket;
}