#ifndef _SERVER_AUTO_H_
#define _SERVER_AUTO_H_

#include <fsm.h>
#include <fsmsystem.h>

#include "../kernel/stdMsgpc16pl16.h"
#include "NetFSM.h"

class ServerAuto : public FiniteStateMachine {
	
	// for FSM
	StandardMessage StandardMsgCoding;
	
	MessageInterface *GetMessageInterface(uint32 id);
	void	SetDefaultHeader(uint8 infoCoding);
	void	SetDefaultFSMData();
	void	NoFreeInstances();
	void	Reset();
	uint8	GetMbxId();
	uint8	GetAutomate();
	uint32	GetObject();
	void	ResetData();
	
	// FSM States
	enum	ServerStates {	FSM_Server_Idle,
							FSM_Server_Authorising, 
							FSM_Server_Transaction,
							FSM_Server_Disconnecting};

	
public:
	ServerAuto();
	~ServerAuto();
	
	void Initialize();

	// bool FSMMsg_2_NetMsg();
	void NetMsg_2_FSMMsg(const char* apBuffer, uint16 anBufferLength);
	
	// FSM_Server_Idle
	void FSM_Server_Idle_Connection_Request();
	// FSM_Server_Authorising
	bool FSM_Server_Authorising_UsernamPassword(SOCKET clientSocket);
	// FSM_Server_Transation
	void FSM_Server_Transaction_Processing(SOCKET clientSocket);
	// FSM_Server_Disconnecting
	void FSM_Server_Disconnecting_Processing();

	void Start();

	void FSM_User_Connecting_User_Connected();
	
protected:
	
	FILE* m_File;
	char m_FileName[256];

	static DWORD WINAPI ServerListener(LPVOID);
	static DWORD WINAPI ServerCommunication(LPVOID);
	
	SOCKET m_socket;
	SOCKET m_client_sockets[10];

	HANDLE m_hThread;
	DWORD m_nThreadID;
	uint16 m_nMaxMsgSize;
	
};

#endif /* _User_AUTO_H */