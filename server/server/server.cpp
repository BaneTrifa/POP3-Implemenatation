#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <stdlib.h>
#include <stdio.h>
#include "data_base.h"
#include "helper_functions_server.h"

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"


// main thread function
int process_transaction(int client_sock, int thread_id, string db_name) {

    while (1) {

        char message[DEFAULT_BUFLEN];
        Database db;

        //receive and check username or id 
        while (1) 
        {
            // Recieve username or id 
            if (!receive_data_server(client_sock, message, thread_id)) return 1;

            // Check username or id 
            if (db.find_user(message, db_name.c_str()) ) {
                send_data_server(client_sock, "ACCEPT", thread_id);
                break;
            }
            else {
                send_data_server(client_sock, "DECLINE", thread_id);
            }

        }

        //receive and check pin
        while (1) {
            // Recieve pin
            if (!receive_data_server(client_sock, message, thread_id)) return 1;

            // Check pin
            if (message == to_string(db.get_pin()) ) {
                send_data_server(client_sock, "ACCEPT", thread_id);
                break;
            }
            else {
                send_data_server(client_sock, "DECLINE", thread_id);
            }
        }

        // Provide service
        while (1) {

        }

    }

    return 0;
}


int __cdecl main(void)
{
    WSADATA wsaData;
    int iResult;

    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;

    struct addrinfo* result = NULL;
    struct addrinfo hints;

    int iSendResult;
    char recvbuf[DEFAULT_BUFLEN];
    const char* sendbuf = "this is a test from server";
    int recvbuflen = DEFAULT_BUFLEN;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }
      
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Create a SOCKET for the server to listen for client connections.
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // Setup the TCP listening socket
    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);

    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    // Receive until the peer shuts down the connection
    while(true)
    {
        // Accept a client socket
        ClientSocket = accept(ListenSocket, NULL, NULL);
        if (ClientSocket == INVALID_SOCKET) {
            printf("accept failed with error: %d\n", WSAGetLastError());
            closesocket(ListenSocket);
            WSACleanup();
            return 1;
        }
        else {
            printf("Connection accepted\n");
        }

        int free_atm = get_free_atm();

        if (free_atm > 0) { // There are free ATMs
            int thread_id = MAX_ATM - free_atm;
            thread_pool[thread_id].active = true; // Set ATM as active

            string message = "ACCEPT";
            if (send(ClientSocket, message.c_str(), message.length(), 0) < 0) {// Send inital message to client
                cout << "Connection lost!" << endl;
            }
            else { // Run ATM*/
                thread_pool[thread_id].thread = thread(process_transaction, ClientSocket, thread_id, DB);
                thread_pool[thread_id].thread.detach();
            }

        }
        else { // No free ATMs
            string message = "Not resource available";
            send(ClientSocket, message.c_str(), message.length(), 0);
        }

        for (int i = 0; i < MAX_ATM; ++i) {
            cout << "Client No. " << (i + 1) << ": " << thread_pool[i].active << endl;
        }

    }

    // shutdown the connection since we're done
    iResult = shutdown(ClientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }

    // No longer need server socket
    closesocket(ListenSocket);

    // cleanup
    closesocket(ClientSocket);
    WSACleanup();

    return 0;
}