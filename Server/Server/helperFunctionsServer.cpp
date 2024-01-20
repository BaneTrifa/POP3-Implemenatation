#include "helperFunctionsServer.h"

std::vector<thread_t> ThreadPool(MAX_CLIENTS);


bool ReceiveDataServer(int sock, char message[], int thread_id) {
    int read_size = recv(sock, message, DEFAULT_BUFLEN, 0);
    if(read_size <= 0) { 
        std::cout << "Client No. " << thread_id << " disconnected!~" << std::endl;
        ThreadPool[thread_id].active = false;

		PrintAllThreads();

        return false;
    }
    message[read_size] = 0;
    
    return true;
} 

bool SendDataServer(int sock, const char* message, int thread_id) {
    if( send(sock , message , strlen(message), 0) < 0)
    {
        std::cout << "Client No. " << thread_id << " disconnected!~" << std::endl;
        ThreadPool[thread_id].active = false;

		PrintAllThreads();

        return false;
    }

    return true;
}

int GetFreeThread() {

    for(int i = 0; i < MAX_CLIENTS; i++) {
        if(!ThreadPool[i].active) 
            return i;
    }

    return -1;
}

void PrintAllThreads() {

	cout << "------------------------------------------------------------" << endl;

	for (int i = 0; i < MAX_CLIENTS; ++i) {
		cout << "Client No. " << (i + 1) << ": " << (ThreadPool[i].active ? "active" : "inactive") << endl;
	}

	cout << "------------------------------------------------------------" << endl;
}

bool process_request(SOCKET sock, char* gmail, char* request, int ThreadID) {

	MailDatabase mDB;
	std::string DbName = DB_NAME;

	std::istringstream iss(request);
	std::string command;
	int arg = -1;

	iss >> command; // Extract command from request
	iss >> arg;		// Extract arg from request, if exist

	if (command == "STAT") {		// STAT command
		int MessagesCount = 0;
		int TotalLength = 0;
		string StrMessagesCount;
		string StrTotalLength;

		TotalLength = mDB.ProcessStatRequest(DbName.c_str(), gmail, &MessagesCount);

		StrMessagesCount = to_string(MessagesCount);
		StrTotalLength = to_string(TotalLength);

		std::string message = "+OK ";
		message += StrMessagesCount;
		message += " ";
		message += StrTotalLength;

		if (SendDataServer(sock, message.c_str(), ThreadID) == false) {
			return  false;
		}
		else {
			return true;
		}
	}

	if (command == "LIST" and arg != -1) {				// LIST command (with argument)

		string MessagesCount;
		string MessageLength;
		string message;


		if (mDB.FindMail(DbName.c_str(), gmail, arg)) {
			message = "+OK ";
			message += to_string(arg);
			message += " ";
			message += to_string(mDB.GetContent().length());

			if (SendDataServer(sock, message.c_str(), ThreadID)) return  true;

			return false;

		}
		else {
			message = "-ERR no such message\n";
			if (SendDataServer(sock, message.c_str(), ThreadID)) return  true;

			return false;
		}

	}

	if (command == "LIST" && arg == -1) {			// LIST command (without arg)
		string message = mDB.ListAllMessages(DbName.c_str(), gmail);

		if (SendDataServer(sock, message.c_str(), ThreadID)) return  true;

		return false;
	}

	if (command == "RETR") {						// RETR command

		string MessagesCount;
		string MessageLength;
		string message;

		if (mDB.FindMail(DbName.c_str(), gmail, arg)) {

			// construct message that contains length of the message
			message = "+OK "; message += " "; message += to_string(mDB.GetContent().length()); message += " octets\n";

			// append message with other informations
			message += "Subject: "; message += mDB.GetSubject(); message += "\nReceiver: "; message += mDB.GetReceiver();
			message += "\n"; message += mDB.GetContent();

		}
		else {
			message = "-ERR no such message\n";
		}

		// sending response
		if (!SendDataServer(sock, message.c_str(), ThreadID)) return false;
		return true;

	}
	
	if (command == "DELE") {					// DELETE COMMAND

		string message;

		if (mDB.MarkMessageAsDeleted(DbName.c_str(), gmail, arg)) {
			message = "+OK message deleted!\n";
		}
		else {
			message = "-ERR no such message or message already deleted!\n";
		}

		// sending response
		if (!SendDataServer(sock, message.c_str(), ThreadID)) return false;
		return true;

	}
	
	if (command == "RSET") {					// RESTART COMMAND

		string message;

		if (mDB.UnmarkDeletedMessages(DbName.c_str(), gmail)) {
			message = "+OK\n";
		}
		else {
			message = "-ERR probleem with database, please try again!\n";
		}

		// sending response
		if (!SendDataServer(sock, message.c_str(), ThreadID)) return false;
		return true;

	}
	
	if (command == "NOOP") {					// NOOP command
		string message = "+OK";
		if (SendDataServer(sock, message.c_str(), ThreadID)) return  true;

		return false;
	}
	
	if (command == "QUIT") {					// QUIT command
		string message = "+OK POP3 server signing off\n";

		mDB.DeleteMarkedMessages(DbName.c_str(), gmail);

		if (SendDataServer(sock, message.c_str(), ThreadID)) return  true;

		return false;
	}

	return true;
}