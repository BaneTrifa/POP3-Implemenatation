#ifndef HELPER_FN_SERVER__H
#define HELPER_FN_SERVER_H

#include "NetFSM.h"
#include "const.h"
#include "MailboxDatabase.h"
#include "conversion.h"
#include <queue>
#include <sstream>

/**
 * Function responsible for recieving messages from the client.
 * @param sock Number of client's socket on which it is connected to the server.
 * @param message Buffer for recieving incoming message from the client.
 * @return Return false if client disconnected or return true if message is recieved properly
*/
bool receive_data_server(SOCKET sock, char message[]);

/**
 * Function responsible for sending messages to the client.
 * @param sock Number of client's socket on which it is connected to the server.
 * @param message Buffer for recieving incoming message from the client.
 * @return Return true if message is sent properly else return false
*/
bool send_data_server(SOCKET sock, const char* message);

/**
 * Function responsible handling requests from client.
 * @param sock Number of client's socket on which it is connected to the server.
 * @param gmail Gmail of the client.
 * @param request POP3 request from client.
 * @return Return true if message is handled and sent properly else return false
*/
bool process_request(SOCKET sock, char* gmail, char* request);

#endif