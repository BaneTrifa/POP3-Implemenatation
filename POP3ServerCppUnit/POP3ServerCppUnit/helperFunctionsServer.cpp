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
	
	if(command == "STAT") {		// STAT command
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
	else if (command == "LIST") {	// LIST command
		
		string messages_count;
		string len_msg;
		string message;


		if (mDB.find_mail(gmail, arg, db_name.c_str())) {
			message = "+OK ";
			message += int_to_string(arg);
			message += " ";
			message += int_to_string(mDB.get_content().length());

			if (send_data_server(sock, message.c_str()) ) return  true;
				
			return false;
				
		} else {
			message = "-ERR no such message\n";
			if (send_data_server(sock, message.c_str())) return  true;
				
			return false;
		}

	} 
	else if(command == "RETR") {	// RETR
	
		string messages_count;
		string len_msg;
		string message;

		if (mDB.find_mail(gmail, arg, db_name.c_str())) {
			
			// construct message that contains length of the message
			message = "+OK "; message += " "; message += int_to_string(mDB.get_content().length()); message += " octets\n";

			// append message with other informations
			message += "Subject: "; message += mDB.get_subject(); message += "\nReceiver: "; message += mDB.get_receiver();
			message += "\n"; message += mDB.get_content();

		} else {
			message = "-ERR no such message\n";
		}

		// sending response
		if (!send_data_server(sock, message.c_str()) ) return false; 						
		return true;

	} else if (command == "DELE") {	// DELETE COMMAND

		string message;

		if (mDB.mark_message_as_deleted(db_name.c_str(), gmail, arg) ) {
			message = "+OK message deleted!\n";
		} else {
			message = "-ERR no such message or message already deleted!\n";
		}

		// sending response
		if (!send_data_server(sock, message.c_str()) ) return false; 					
		return true;

	}
	else if (command == "RSET") {	// RESTART COMMAND

		string message;

		if (mDB.unmark_message_deleted(db_name.c_str(), gmail) ) {
			message = "+OK\n";
		} else {
			message = "-ERR probleem with database, please try again!\n";
		}

		// sending response
		if (!send_data_server(sock, message.c_str()) ) return false; 					
		return true;

	}
	else if(command == "NOOP") {	// NOOP command
		string message = "+OK";
		if (send_data_server(sock, message.c_str())) return  true;
		
		return false;
	}
	else if(command == "QUIT") {	// QUIT command
		string message = "+OK POP3 server signing off\n";

		mDB.delete_marked_message(db_name.c_str(), gmail);

		if (send_data_server(sock, message.c_str())) return  true;
		
		return false;
	}

	 return true;
}