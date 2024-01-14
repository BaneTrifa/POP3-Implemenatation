#ifndef HELPER_FN_SERVER__H
#define HELPER_FN_SERVER_H

#include "NetFSM.h"
#include "const.h"

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

#endif