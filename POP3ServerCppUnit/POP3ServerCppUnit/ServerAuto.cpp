#include <stdio.h>
#include <conio.h>
#include <time.h>

#include "const.h"
#include "ServerAuto.h"
#include "HelperFunctionsServer.h"
#include "UserDatabase.h"

bool g_ProgramEnd = false;

#define StandardMessageCoding 0x00

struct ServerParameter {
	ServerAuto* server;
	SOCKET clientSocket;
};

ServerAuto::ServerAuto() : FiniteStateMachine(USER_AUTOMATE_TYPE_ID, USER_AUTOMATE_MBX_ID, 0, 3, 3) {
}

ServerAuto::~ServerAuto() {
}


uint8 ServerAuto::GetAutomate() {
	return USER_AUTOMATE_TYPE_ID;
}

/* This function actually connnects the ServerAuto with the mailbox. */
uint8 ServerAuto::GetMbxId() {
	return USER_AUTOMATE_MBX_ID;
}

uint32 ServerAuto::GetObject() {
	return GetObjectId();
}

MessageInterface *ServerAuto::GetMessageInterface(uint32 id) {
	return &StandardMsgCoding;
}

void ServerAuto::SetDefaultHeader(uint8 infoCoding) {
	SetMsgInfoCoding(infoCoding);
	SetMessageFromData();
}

void ServerAuto::SetDefaultFSMData() {
	SetDefaultHeader(StandardMessageCoding);
}

void ServerAuto::NoFreeInstances() {
	printf("[%d] ServerAuto::NoFreeInstances()\n", GetObjectId());
}

void ServerAuto::Reset() {
	printf("[%d] ServerAuto::Reset()\n", GetObjectId());
}


void ServerAuto::Initialize() {
	SetState(FSM_Server_Idle);	
	
	//intitialization message handlers
	InitEventProc(FSM_Server_Idle, MSG_TCP_Listening, (PROC_FUN_PTR)&ServerAuto::FSM_Server_Idle_Connection_Request);
	
	InitEventProc(FSM_Server_Authorising, MSG_User_Name_Password, (PROC_FUN_PTR)&ServerAuto::FSM_Server_Authorising_UsernamPassword);
	InitEventProc(FSM_Server_Authorising, MSG_QUIT, (PROC_FUN_PTR)&ServerAuto::FSM_Server_Disconnecting_Processing);

	InitEventProc(FSM_Server_Transaction, MSG_COMMAND, (PROC_FUN_PTR)&ServerAuto::FSM_Server_Transaction_Processing);
	InitEventProc(FSM_Server_Transaction, MSG_QUIT, (PROC_FUN_PTR)&ServerAuto::FSM_Server_Disconnecting_Processing);
}

void ServerAuto::FSM_Server_Idle_Connection_Request() {
	m_hThread = CreateThread(NULL, 0, ServerListener, (LPVOID) this, 0, &m_nThreadID); 
	if (m_hThread == NULL) {
		/* Cannot create thread.*/
		printf("Cannot create thread listener!\n");
		return ;
	}
}

bool ServerAuto::FSM_Server_Authorising_UsernamPassword(SOCKET clientSocket){

    UserDatabase db;
	std::string db_name = DB_NAME;
	char* gmail = new char[50];
	char* password = new char[50];

	while (true) {
		// Receive gmail
		if(receive_data_server(clientSocket, gmail) == false) 
			return 1;

		if (db.find_user(gmail, db_name.c_str()) && !db.get_logged_in() &&
			!db.get_banned() && send_data_server(clientSocket, "ACCEPT")) { // check if email is valid and if it is, try to sent ACCEPT 
            break;
		}
		else if (send_data_server(clientSocket, "DECLINE") == false){ // email is not valid and try to send DECLINE
			return 1;		// if client is disconnected return 1
         }
	}

	db.set_logged_in(true);
	return 1;
	//printf("Gmail: %s\n", gmail);

	while (true) {
		// Receive password
		if(receive_data_server(clientSocket, password) == false) 
			return 1;

		std::string password_from_db = db.get_password();
		if (password == password_from_db && send_data_server(clientSocket, "ACCEPT")) { // check if passwrod is valid and if it is, try to sent ACCEPT
            break;
		}
		else if (send_data_server(clientSocket, "DECLINE") == false){	// email is not valid and try to send DECLINE
			return 1;	// if client is disconnected return 1
        }
	}

	//printf("Password: %s\n", password);

	this->FSM_Server_Transaction_Processing(clientSocket);

	return 0;

}

void ServerAuto::FSM_Server_Transaction_Processing(SOCKET clientSocket) {

	char* option = new char[50];
	std::queue<int> delete_queue;

	while (true) {
		// Receive option
		if(receive_data_server(clientSocket, option) == false) 
			return;



		//printf("Option: %s\n", option);

	}
}

void ServerAuto::FSM_Server_Disconnecting_Processing(){

}


void ServerAuto::Start(){

	PrepareNewMessage(0x00, MSG_TCP_Listening);
	SetMsgToAutomate(USER_AUTOMATE_TYPE_ID);
	SetMsgObjectNumberTo(0);
	SendMessage(USER_AUTOMATE_MBX_ID);

}

void ServerAuto::FSM_User_Connecting_User_Connected(){
	
}

DWORD ServerAuto::ServerCommunication(LPVOID param) {
	ServerParameter* server_param = (ServerParameter*) param;
	SOCKET clientSocket = (SOCKET) server_param->clientSocket;
	ServerAuto* server = server_param->server;

	printf("Thread: %d connected\n", clientSocket);

	char* data;
	data = "ACCEPT";
	if (send(clientSocket, data, strlen(data), 0) != strlen(data)) {
		delete [] data;
		printf("Client dissconnected!\n");
		return 1;
	}

	while (1) {

		if(server->FSM_Server_Authorising_UsernamPassword(clientSocket)) return 1;
		
	}

	return 0;

}

DWORD ServerAuto::ServerListener(LPVOID param) {
	ServerAuto* serverTemp = (ServerAuto*)param;
	SOCKET newSocket;
	SOCKADDR_IN local_sin;

	WSADATA WsaData;
	WORD wVersionRequested = MAKEWORD( 2, 1 );
	if (WSAStartup (wVersionRequested, &WsaData) == SOCKET_ERROR)  {
		throw;
		return 1;
	}

	memset(&local_sin, 0, sizeof(local_sin));
	local_sin.sin_family = AF_INET; // tip adrese
	local_sin.sin_port = htons(PORT); // port 

	local_sin.sin_addr.s_addr =inet_addr(ADRESS);

	serverTemp->m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if (serverTemp->m_socket == SOCKET_ERROR)  {
		int k = WSAGetLastError();
		return 1;
	}
 
	if( bind( serverTemp->m_socket, (SOCKADDR *)&local_sin, sizeof(local_sin)) == SOCKET_ERROR)  {
		int k = WSAGetLastError();
		closesocket(serverTemp->m_socket);
		return 1;
	}


	int retValue = listen( serverTemp->m_socket, MAX_PENDING_CONNECTIONS);
	if( retValue == SOCKET_ERROR) {
		int i = WSAGetLastError();
		return 1;
	}  

	while (true) {
		newSocket = accept( serverTemp->m_socket, NULL, NULL);
		if( newSocket == INVALID_SOCKET)  {
			int i = WSAGetLastError(); 
			return 1;
		} 

		ServerParameter param = {serverTemp, newSocket};

		serverTemp->m_hThread = CreateThread(NULL, 0, ServerCommunication, (LPVOID) &param, 0, &serverTemp->m_nThreadID); 
		if (serverTemp->m_hThread == NULL) {
			/* Cannot create thread.*/
			closesocket(newSocket);
			newSocket = INVALID_SOCKET;
			return 1;
		}
	}

	return 0;
}