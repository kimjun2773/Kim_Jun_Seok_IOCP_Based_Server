#include "stdafx.h" 
#include "IOCP.h"

IOCP::IOCP(): m_hIOCP(INVALID_HANDLE_VALUE), m_iThreadCnt(0)
{
	m_vNetworkThread.clear();
}

IOCP::~IOCP()
{

}


BOOL IOCP::Create(size_t createThreadCnt, size_t maxActiveThreadCnt)
{
	m_hIOCP = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, maxActiveThreadCnt);
	if (NULL == m_hIOCP)
	{
		return FALSE;
	}

	m_iThreadCnt = createThreadCnt;
	
	return TRUE;
}

BOOL IOCP::Register(HANDLE Device)
{
	auto ret = ::CreateIoCompletionPort(Device, m_hIOCP, 0, 0);
	if (NULL == ret)
	{
		return FALSE;
	}

	return TRUE;
}

void IOCP::Run(void)
{
	for (int i = 0; i<m_iThreadCnt; ++i)
	{
		m_vNetworkThread.emplace_back(std::thread(&IOCP::ThreadMain, this));
	}
}
