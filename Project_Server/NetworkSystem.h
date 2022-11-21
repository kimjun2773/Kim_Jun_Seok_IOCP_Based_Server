#pragma once
#include "IOCP.h"

class IOCP_Message;
class Session;

class NetworkSystem : 
	public	Singleton<NetworkSystem>,
	public	IOCP

{
public: typedef std::unordered_map<SOCKET, Session*>	UMapSession;

public:						NetworkSystem(void);
public:						~NetworkSystem(void);

private: void				Init(void);

private: BOOL				PostAccept(Session* pSession);
private: BOOL				PostRecv(Session* pSession);
public: BOOL				PostSend(Session* pSession);
public: BOOL				PostClose(Session* pSession);

private: BOOL				OnRecv(IOCP_Message* pIOCP_Message);
private: BOOL				OnSend(IOCP_Message* pIOCP_Message);
private: BOOL				OnAccept(IOCP_Message* pIOCP_Message);

private: virtual void		ThreadMain(void) override;

private: SOCKET				m_hLisntenSock;
private: UMapSession		m_uMapSession;

private: std::mutex			m_lock;
};