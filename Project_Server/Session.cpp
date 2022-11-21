#include "stdafx.h" 
#include "Session.h"
#include "NetworkSystem.h"

Session::Session()
{
	m_socket = ::WSASocketW(PF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	m_AcceptMessage.m_MessageType	= IOCP_Message::eAccept;
	m_AcceptMessage.m_pOderer		= this;
	m_AcceptMessage.m_WSABUF.len	= 0;
	
	m_RecvMessage.m_MessageType		= IOCP_Message::eRecv;
	m_RecvMessage.m_pOderer			= this;
	m_RecvMessage.m_WSABUF.len		= MAXIMUM_TRANSFER_SIZE;

	m_pReadBuffer					= nullptr;
}

Session::~Session()
{
	::closesocket(m_socket);

	for (auto pMessage: m_lSendMessage)
	{
		delete pMessage;
	}
}

void Session::GetIPString(std::string& IPString)
{
	char tempStr[20] = "";

	auto iRet = inet_ntop(AF_INET, &m_addr.sin_addr, tempStr, sizeof(m_addr.sin_addr));
	if (NULL == iRet)
	{
		WSAGetLastError();
		IPString = "";
		return;
	}

	IPString = tempStr;
}

void Session::SendPacket(Packet* packet)
{
	packet->Write(&m_sWriteBuffer);

	IOCP_Message* pMessage		= new IOCP_Message();
	pMessage->m_MessageType		= IOCP_Message::eSend;
	pMessage->m_WSABUF.len		= m_sWriteBuffer.m_endPos;
	pMessage->m_pOderer			= this;
	memcpy(pMessage->m_MessageBuffer, m_sWriteBuffer.m_Buffer, m_sWriteBuffer.m_endPos);

	//Lock and Push Message
	{
		std::lock_guard<std::mutex> lock(m_lock);
		m_lSendMessage.push_back(pMessage);
	}

	NetworkSystem::GetInstance().PostSend(this);
}
