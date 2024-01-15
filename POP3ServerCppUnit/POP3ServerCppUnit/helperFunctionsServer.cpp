#include "helperFunctionsServer.h"

bool receive_data_server(SOCKET sock, char message[]) {

	fd_set set;
	struct timeval timeout;
	FD_ZERO(&set);				/* clear the set */
	FD_SET(sock, &set);			/* add our file descriptor to the set */
	timeout.tv_sec = SOCKET_READ_TIMEOUT_SEC;
	timeout.tv_usec = 0;
	int rv = select(sock, &set, NULL, NULL, &timeout);

	int nReceivedBytes = recv(sock, message, DEFAULT_BUFLEN, 0);

	if (nReceivedBytes < 0)
	{
		printf("Client %d disconnected!\n", sock);
		closesocket(sock);
		return false;
	} 

	message[nReceivedBytes] = 0;

	return true;
}

bool send_data_server(SOCKET sock, const char* message) {
    if( send(sock , message , strlen(message), 0) < 0)
    {
        printf("Client %d disconnected!\n", sock);
        return false;
    }

    return true;
}