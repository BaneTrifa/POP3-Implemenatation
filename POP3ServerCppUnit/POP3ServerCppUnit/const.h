#ifndef _CONST_H_
#define _CONST_H_

#include "./kernelTypes.h"

//#define D_TESTING

const uint8 CH_AUTOMATE_TYPE_ID = 0x02;
const uint8 CL_AUTOMATE_TYPE_ID = 0x01;
const uint8 USER_AUTOMATE_TYPE_ID = 0x00;
const uint8 TEST_AUTOMATE_TYPE_ID = 0x03;

const uint8 CH_AUTOMATE_MBX_ID = 0x02;
const uint8 CL_AUTOMATE_MBX_ID = 0x01;
const uint8 USER_AUTOMATE_MBX_ID = 0x00;
const uint8 TEST_MBX_ID = 0x03;

//	Client states
enum ClStates {	FSM_Cl_Ready, 
				FSM_Cl_Connecting, 
				FSM_Cl_Authorising,
				FSM_Cl_User_Check,
				FSM_Cl_Pass_Check,
				FSM_Cl_Mail_Check,
				FSM_Cl_Receiving,
				FSM_Cl_Deleting,
				FSM_Cl_Reseting,
				FSM_Cl_Disconnecting };

// channel messages
const uint16 MSG_Connection_Request			= 0x0001;
const uint16 MSG_Sock_Connection_Reject		= 0x0002;
const uint16 MSG_Sock_Connection_Acccept	= 0x0003;
const uint16 MSG_Cl_MSG						= 0x0004;
const uint16 MSG_Sock_MSG					= 0x0005;
const uint16 MSG_Disconnect_Request			= 0x0006;
const uint16 MSG_Sock_Disconected			= 0x0007;
const uint16 MSG_Sock_Disconnecting_Conf	= 0x0008;

// client messages
const uint16 MSG_User_Check_Mail		= 0x0009;
const uint16 MSG_Cl_Connection_Reject	= 0x000a;
const uint16 MSG_Cl_Connection_Accept	= 0x000b;
const uint16 MSG_User_Name_Password		= 0x000c;
const uint16 MSG_MSG					= 0x000d;
const uint16 MSG_Cl_Disconected			= 0x000f;

// user messages
const uint16 MSG_Set_All				= 0x0010;
const uint16 MSG_User_Connected			= 0x0011;
const uint16 MSG_User_Connecton_Fail	= 0x0012;
const uint16 MSG_Mail					= 0x0013;
const uint16 MSG_User_Save_Mail			= 0x0015;
const uint16 MSG_User_Disconnected		= 0x0014;

const uint16 MSG_TCP_Listening			= 0x0016;
const uint16 MSG_COMMAND				= 0x0017;
const uint16 MSG_QUIT					= 0x0018;


#define ADRESS "127.0.0.1"
//#define ADRESS "mail.spymac.com"
//#define ADRESS "krtlab8"
#define PORT 27000

#define TIMER1_ID 0
#define TIMER1_COUNT 10
#define TIMER1_EXPIRED 0x20

#define PARAM_DATA 0x01
#define PARAM_Name 0x02
#define PARAM_Pass 0x03

#define DEFAULT_BUFLEN 255
#define SOCKET_READ_TIMEOUT_SEC 30
#define DB_NAME "pop3.db"


#endif //_CONST_H_