#include "socketutil.h"
#include <ws2tcpip.h>


int GetTcp4AddrInfo(const char *host, const char *port, sockaddr_in &ret)
{
	//--------------------------------
	// Declare and initialize variables.
	struct addrinfo aiHints;
	struct addrinfo *aiList = NULL;
	int retVal;

	//--------------------------------
	// Setup the hints address info structure
	// which is passed to the getaddrinfo() function
	memset(&aiHints, 0, sizeof(aiHints));
	aiHints.ai_family = AF_INET;
	aiHints.ai_socktype = SOCK_STREAM;
	aiHints.ai_protocol = IPPROTO_TCP;

	//--------------------------------
	// Call getaddrinfo(). If the call succeeds,
	// the aiList variable will hold a linked list
	// of addrinfo structures containing response
	// information about the host
	retVal = getaddrinfo(host, port, &aiHints, &aiList);
	if (retVal == 0) {
		memcpy(&ret, aiList->ai_addr, sizeof(sockaddr_in));
		freeaddrinfo(aiList);
	}	
	return retVal;
}