#include "stdafx.h" 
#include "IOCP_Message.h"

IOCP_Message::IOCP_Message(void)
{
	ZeroMemory(this, sizeof(IOCP_Message));

	m_WSABUF.buf = m_MessageBuffer;
}

IOCP_Message::IOCP_Message(MessegeType MessageType, void* pOderer)
{
	ZeroMemory(this, sizeof(IOCP_Message));
	m_MessageType	= MessageType;
	m_pOderer		= pOderer;
	m_WSABUF.buf	= m_MessageBuffer;
}