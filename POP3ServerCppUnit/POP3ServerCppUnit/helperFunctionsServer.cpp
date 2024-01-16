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

bool process_request(SOCKET sock, char* gmail, char* request) {
	
	MailDatabase mDB;
	std::string db_name = DB_NAME;

	std::istringstream iss(request);
	std::string command;
    int arg = -1;

	iss >> command; // Extract command from request
	iss >> arg;		// Extract arg from request, if exist
	
	if(command == "STAT") {
		int messages_count = 0;
		int total_length = 0;
		string str_messages_count;
		string str_total_length;

		total_length = mDB.process_stat_request(db_name.c_str(), gmail, &messages_count);
		
		str_messages_count = int_to_string(messages_count);
		str_total_length = int_to_string(total_length);

		std::string message = "+OK ";
		message += str_messages_count;
		message += " ";
		message += str_total_length;

		if (send_data_server(sock, message.c_str()) == false){
			return  false;
		} else {
			return true;
		}
	}

	 return true;
}