#undef UNICODE

#include "helperFunctionsServer.h"
#include "UserDatabase.h"

using namespace std;

string ExtractGmailPassword(char* request) {

    istringstream iss(request);
    string command;
    string arg;

    iss >> command;     // Extract command from request
    iss >> arg;		// Extract arg  (gmail, password)

    return arg;
}

// main thread function
int ProcessTransaction(int ClientSocket, int ThreadId) {

    std::string DbName = DB_NAME;

    while (1) {

        UserDatabase db;
        char UserCommand[DEFAULT_BUFLEN];
        string gmail;
        char PassCommand[DEFAULT_BUFLEN];
        string password;
        char request[DEFAULT_BUFLEN];

        // receive and check gmail - AUTHENICATION PHASE
        while (1)
        {
            // Recieve gmail
            if (!ReceiveDataServer(ClientSocket, UserCommand, ThreadId)) return 1;

            gmail = ExtractGmailPassword(UserCommand);

            db.FindUser(gmail, DbName.c_str());

            if (db.GetGmail() == gmail &&
                !db.GetBanned() && SendDataServer(ClientSocket, "ACCEPT", ThreadId)) { // check if email is valid and if it is, try to sent ACCEPT 
                break;
            }
            else if (SendDataServer(ClientSocket, "DECLINE", ThreadId) == false) { // email is not valid and try to send DECLINE
                return 1;		// if client is disconnected return 1
            }
        }

        // receive and check password - AUTHENICATION PHASE
        while (1)
        {
            // Recieve password
            if (!ReceiveDataServer(ClientSocket, PassCommand, ThreadId)) return 1;

            password = ExtractGmailPassword(PassCommand);

            string PasswordFromDb = db.GetPassword();

            if (PasswordFromDb == password && SendDataServer(ClientSocket, "ACCEPT", ThreadId)) { // check if email is valid and if it is, try to sent ACCEPT 
                break;
            }
            else if (SendDataServer(ClientSocket, "DECLINE", ThreadId) == false) { // email is not valid and try to send DECLINE
                return 1;		// if client is disconnected return 1
            }
        }

        // TRANSACTION PHASE
        while (1)
        {
            // Receive option
            if (ReceiveDataServer(ClientSocket, request, ThreadId) == false) {
                return 1;
            }

            if (process_request(ClientSocket, &gmail[0], request, ThreadId) == false) {
                return 1;
            }

            if (strcmp(request, "QUIT") == 0) {
                break;
            }
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

    while (1) {
        cout << "Waiting for incoming connections" << endl;

        // Accept a client socket
        ClientSocket = accept(ListenSocket, NULL, NULL);
        if (ClientSocket == INVALID_SOCKET) {
            printf("accept failed with error: %d\n", WSAGetLastError());
            closesocket(ListenSocket);
            WSACleanup();
            return 1;
        }

        int FreeThread = GetFreeThread();

        if (FreeThread >= 0) { // There are free thread

            ThreadPool[FreeThread].active = true; // Set thread as active

            string message = "ACCEPT";
            if (send(ClientSocket, message.c_str(), message.length(), 0) < 0) { // Send inital message to client
                cout << "Connection lost!" << endl;
            }
            else { // Run thread
                ThreadPool[FreeThread].thread = thread(ProcessTransaction, ClientSocket, FreeThread);
                ThreadPool[FreeThread].thread.detach();
            }
        }
        else { // No free thread
            string message = "Not resource available";
            send(ClientSocket, message.c_str(), message.length(), 0);
        }

        PrintAllThreads();
    }

    // No longer need server socket
    closesocket(ListenSocket);

    // shutdown the connection since we're done
    iResult = shutdown(ClientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }

    // cleanup
    closesocket(ClientSocket);
    WSACleanup();

    return 0;
}