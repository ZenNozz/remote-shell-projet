#include "../headers/client.h"

int connectToServer(char* port, char* ip) {
	// Retrieve server address
	struct addrinfo hints, *res, *attempt;
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

	// Connect to the first address we can
	for (attempt = res; res != NULL; res = res->ai_next) {
		serverSocket = socket(attempt->ai_family, attempt->ai_socktype, attempt->ai_protocol);
		if (serverSocket == -1) {
			close(serverSocket);
			continue;
		}
		if (connect(serverSocket, attempt->ai_addr, attempt->ai_addrlen) == -1) {
			close(serverSocket);
			continue;
		}
		freeaddrinfo(res); // attempt points to res, so both get freed
		return serverSocket;
	}

	// Couldn't connect to any sockets
	return -3;
}