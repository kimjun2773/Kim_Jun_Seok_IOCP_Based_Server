#pragma once
#include "IOCP_Message.h"

class Packet;

struct Session
{
	Session();
	~Session();
	
	void GetIPString(std::string& IPString);
	void SendPacket(Packet* packet);
	
	SOCKET							m_socket;
	SOCKADDR_IN						m_addr;
	
	IOCP_Message					m_AcceptMessage;
	IOCP_Message					m_RecvMessage;
	std::list<IOCP_Message*>		m_lSendMessage;
	
	AsemblyBuffer*					m_pReadBuffer;
	AsemblyBuffer					m_sWriteBuffer;
	
	std::mutex						m_lock;
};