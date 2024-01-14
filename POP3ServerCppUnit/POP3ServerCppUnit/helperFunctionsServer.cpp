#include "helperFunctionsServer.h"

bool receive_data_server(SOCKET sock, char message[]) {
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