#pragma once

class IOCP
{
public: IOCP();
public: ~IOCP();

public: BOOL							Create(size_t createThreadCnt, size_t maxActiveThreadCnt);
public: BOOL							Register(HANDLE handle);
public: void							Run(void);

protected: virtual void					ThreadMain(void) = 0;

protected: HANDLE						m_hIOCP;
protected: size_t						m_iThreadCnt;
protected: std::vector<std::thread>		m_vNetworkThread;
};