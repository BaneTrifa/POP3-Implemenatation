#ifndef HELPER_FN_SERVER__H
#define HELPER_FN_SERVER_H

#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#include <vector>
#include <thread>
#include <string>
#include <sstream>
#include "const.h"
#include "MailboxDatabase.h"

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")


struct thread_t {
    std::thread thread;
    bool active;
};
extern std::vector<thread_t> ThreadPool; // Vecotr that cointains all available ATMs (active or inactive)


/**
 * Function responsible for recieving messages from the client.
 * @param sock Number of client's socket on which it is connected to the server.
 * @param message Buffer for recieving incoming message from the client.
 * @param thread_id ID of thread which corespods to active ATM
 * @return Return false if client disconnected or return true if message is recieved properly
*/
bool ReceiveDataServer(int sock, char message[], int thread_id);

/**
 * Function responsible for sending messages to the client.
 * @param sock Number of client's socket on which it is connected to the server.
 * @param message Buffer for recieving incoming message from the client.
 * @param thread_id ID of thread which corespods to active ATM
 * @return Return true if message is sent properly else return false
*/
bool SendDataServer(int sock, const char* message, int thread_id);

/**
 * @return Return number of free threds (ATMs)
*/
int GetFreeThread();

void PrintAllThreads();

/**
 * Function responsible handling requests from client.
 * @param sock Number of client's socket on which it is connected to the server.
 * @param gmail Gmail of the client.
 * @param request POP3 request from client.
 * @param ThreadID Index of current thread in ThreadPool.
 * @return Return true if message is handled and sent properly else return false
*/
bool process_request(SOCKET sock, char* gmail, char* request, int ThreadID);

#endif
