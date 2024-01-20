#define WIN32_LEAN_AND_MEAN

#include <stdlib.h>
#include <stdio.h>
#include "helper_functions_client.h"

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "110"

int __cdecl main(int argc, char** argv)
{
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo* result = NULL,
                    * ptr = NULL,
                    hints;

    int iResult;

    char uinput[DEFAULT_BUFLEN];            // user input option
    char recv_gmail_ACK[DEFAULT_BUFLEN];  // recieved ACCEPT message for gmail from server
    char recv_pass_ACK[DEFAULT_BUFLEN];  // recieved ACCEPT message for password from server
    char response[DEFAULT_BUFLEN];         // recieved rsponse on user command from server
    char message[DEFAULT_BUFLEN];  

    // Validate the parameters
    if (argc != 2) {
        printf("usage: %s server-name\n", argv[0]);
        return 1;
    }

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Attempt to connect to an address until one succeeds
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
            ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            return 1;
        }

        // Connect to server.
        iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }

    if (!receive_data(ConnectSocket, message) ) return 1;

    if (strcmp(message, "ACCEPT") != 0)
    {
        printf("Server busy!!!\n");
        return 1;
    }

    while (1) {

        // Enter and send username or id
        do {
            login_menu(uinput);
            send_data(uinput, ConnectSocket);

            //Receive identification confirmation
            if (!receive_data(ConnectSocket, recv_gmail_ACK)) return 1;

        } while (strcmp(recv_gmail_ACK, "ACCEPT") != 0);


        // Enter and send pin
        do {
            pin_menu(uinput);
            send_data(uinput, ConnectSocket);

            //Receive pin confirmation
            if (!receive_data(ConnectSocket, recv_pass_ACK)) return 1;

        } while (strcmp(recv_pass_ACK, "ACCEPT") != 0);

        system("cls");

        while (1) {
            // Enter option and send to server
            int option = user_menu(uinput);
            send_data(uinput, ConnectSocket);

            // Other command
            if (!receive_data(ConnectSocket, response)) return 1;

            printf("%s\n", response);

            system("pause");
            system("cls");

            if (option == 8) { // QUIT option
                break;
            }

        }

        system("cls");
    }

    // shutdown the connection since no more data will be sent
    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    // cleanup
    closesocket(ConnectSocket);
    WSACleanup();

    return 0;
}