#pragma once

#define MAXIMUM_TRANSFER_SIZE 8000

struct IOCP_Message : public OVERLAPPED
{
	enum MessegeType { eRecv, eSend, eAccept, eRead, eWrite, eMessage };
	
	IOCP_Message(void);
	IOCP_Message(MessegeType MessageType, void* pOderer);
	
	MessegeType	m_MessageType;
	void*		m_pOderer;
	WSABUF		m_WSABUF;
	CHAR		m_MessageBuffer[MAXIMUM_TRANSFER_SIZE];
	size_t		m_BytesTransferred;
};
