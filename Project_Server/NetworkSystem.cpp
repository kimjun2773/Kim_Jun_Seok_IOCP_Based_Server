#include "stdafx.h" 
#include "NetworkSystem.h"

#include <iostream>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "mswsock.lib")

using namespace std;

NetworkSystem::NetworkSystem(void)
{
	Init();
}

NetworkSystem::~NetworkSystem(void)
{
	WSACleanup();
}

void NetworkSystem::Init(void)
{
	//StartUp Winsock DLL
	{
		WSADATA wsaData;

		auto ret = ::WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (0 != ret)
		{
			assert(false);

			::WSACleanup();
			return;
		}
	}

	//if someting goes wrong routine!
	auto SomethingWrong = [&](void) -> void
	{
		printf("WSAGetLastError : %d \n", WSAGetLastError());
		assert(false);

		::closesocket(m_hLisntenSock);
		::WSACleanup();

		return;
	};

	//Create Listen Socket
	{
		m_hLisntenSock = ::WSASocketW(PF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
		if (INVALID_SOCKET == m_hLisntenSock)
		{
			SomethingWrong();
			return;
		}
	}

	//Bind Socket
	{
		SOCKADDR_IN sLisntenSockAddr = {};
		sLisntenSockAddr.sin_family			= AF_INET;
		sLisntenSockAddr.sin_addr.s_addr	= htonl(INADDR_ANY);
		sLisntenSockAddr.sin_port			= htons(1443);

		auto ret = ::bind(m_hLisntenSock,(SOCKADDR*)&sLisntenSockAddr, sizeof(SOCKADDR_IN));
		if (SOCKET_ERROR == ret)
		{
			SomethingWrong();
			return;
		}
	}

	//Create IOCP
	{
		//스레드 4개생성, 최대 활성화 스레드 4개
		auto ret = IOCP::Create(4,4);
		if (TRUE != ret)
		{
			SomethingWrong();
			return;
		}
	}

	//Register ListenSocket to IOCP.
	{
		auto ret = IOCP::Register((HANDLE)m_hLisntenSock);
		if (TRUE != ret)
		{
			SomethingWrong();
			return;
		}
	}

	//Listen
	{
		auto ret = ::listen(m_hLisntenSock, SOMAXCONN);
		if (SOCKET_ERROR == ret)
		{
			SomethingWrong();
			return;
		}
	}

	//Run IOCP Thread
	{
		IOCP::Run();
	}

	//AsyncAccept
	{
		//최대 10개의 비동기 Accept를 지원한다
		for (int i=0 ; i<10 ; ++i)
		{
			Session* newSession = new Session();
			auto ret = PostAccept(newSession);
			if (FALSE == ret)
			{
				SomethingWrong();
				return;
			}
		}
	}
}

void NetworkSystem::ThreadMain(void)
{
	while (1)
	{
		DWORD			lpNumberOfBytesTransferred;
		void*			CompletionKey;
		IOCP_Message*	pMessage;

		auto GQCSSuccess = ::GetQueuedCompletionStatus(	m_hIOCP,
														&lpNumberOfBytesTransferred,
														(PULONG_PTR)&CompletionKey,
														(LPOVERLAPPED *)&pMessage,
														INFINITE
		);

		if(TRUE == GQCSSuccess)
		{
			pMessage->m_BytesTransferred = lpNumberOfBytesTransferred;

			switch (pMessage->m_MessageType)
			{
			case IOCP_Message::eRecv: {OnRecv(pMessage);		break; };
			case IOCP_Message::eSend: {OnSend(pMessage);		break; };
			case IOCP_Message::eAccept: {OnAccept(pMessage);	break; };
			default: break;
			}
		}
		else
		{
			auto iRet = GetLastError();
			switch (iRet){
			//Clien Hard Close
			case ERROR_NETNAME_DELETED: {PostClose((Session*)(pMessage->m_pOderer)); break; };

			//else...
			default:
			{
				cout<<"NetworkThreadMain::GetQueuedCompletionStatus FAILES!!! : " << GetLastError( )<< endl;
				return;
			}}
		}
	}
}

BOOL NetworkSystem::PostAccept(Session* pSession)
{
	auto ret = ::AcceptEx(	m_hLisntenSock,
							pSession->m_socket,
							pSession->m_AcceptMessage.m_MessageBuffer,
							0,
							sizeof(sockaddr_in) + 16,
							sizeof(sockaddr_in) + 16,
							NULL,
							&pSession->m_AcceptMessage
	);

	if (FALSE == ret && WSA_IO_PENDING != WSAGetLastError())
	{
		return FALSE;
	}

	return TRUE;
}

BOOL NetworkSystem::PostRecv(Session* pSession)
{
	DWORD dwNumberOfBytesRecvd				= 0;
	DWORD dwFlag							= 0;

	auto iRet = WSARecv(pSession->m_socket,
						&pSession->m_RecvMessage.m_WSABUF,
						1,
						&dwNumberOfBytesRecvd,
						&dwFlag,
						&pSession->m_RecvMessage,
						NULL
	);

	if (SOCKET_ERROR == iRet && (ERROR_IO_PENDING != WSAGetLastError()))
	{
		PostClose(pSession);
		return FALSE;
	}

	return TRUE;
}

BOOL NetworkSystem::PostSend(Session* pSession)
{
	IOCP_Message* pMessage = nullptr;

	//Lock and Pop Message
	if (!pSession->m_lSendMessage.empty())
	{
		std::lock_guard<std::mutex> lock(pSession->m_lock);
		pMessage = pSession->m_lSendMessage.front();
		pSession->m_lSendMessage.pop_front();
	}

	//Send a Message
	if(nullptr != pMessage)
	{
		DWORD dwNumberOfBytesSent	= 0;
		DWORD dwFlag				= 0;

		auto iRet = WSASend(pSession->m_socket,
							&pMessage->m_WSABUF,
							1,
							&dwNumberOfBytesSent,
							dwFlag,
							pMessage,
							NULL
		);

		if (SOCKET_ERROR == iRet && (ERROR_IO_PENDING != WSAGetLastError()))
		{
			PostClose(pSession);
			return FALSE;
		}
	}

	return TRUE;
}

BOOL NetworkSystem::PostClose(Session* pSession)
{
	IOCP_Message* pMessage	= new IOCP_Message;
	pMessage->m_MessageType = IOCP_Message::eSend;
	pMessage->m_pOderer		= pSession;
	pMessage->m_WSABUF.len	= 0;

	//Unregister Session To NetworkSystem
	{
		std::lock_guard<std::mutex> lock(m_lock);

		m_uMapSession.erase(pSession->m_socket);
	}

	//Register Close Message;
	{
		std::lock_guard<std::mutex> lock(pSession->m_lock);

		pSession->m_lSendMessage.push_back(pMessage);
	}

	PostSend(pSession);
	return TRUE;
}

BOOL NetworkSystem::OnRecv(IOCP_Message* pIOCP_Message)
{
	Session* pSession = reinterpret_cast<Session*>(pIOCP_Message->m_pOderer);

	//Recieved Graceful Socket Close
	if (pIOCP_Message->m_BytesTransferred <= 0)
	{
		PostClose(pSession);
		return TRUE;
	}

	std::vector<AsemblyBuffer*> vAssemblyComplete;
	CHAR* messageBuffer		= pIOCP_Message->m_MessageBuffer;
	auto pos				= 0;
	auto remainBytes		= pIOCP_Message->m_BytesTransferred;

	//Assembly Packets
	while (0 < remainBytes)
	{
		if (nullptr == pSession->m_pReadBuffer)
		{
			pSession->m_pReadBuffer = new AsemblyBuffer();
		}

		auto completeBytes	= pSession->m_pReadBuffer->Copy(&messageBuffer[pos], remainBytes);
		pos					+= completeBytes;
		remainBytes			-= completeBytes;

		if (FALSE == pSession->m_pReadBuffer->IsAssembling())
		{
			pSession->m_pReadBuffer->m_curPos = 0;
			vAssemblyComplete.push_back(pSession->m_pReadBuffer);
			pSession->m_pReadBuffer = nullptr;
		}
	}

	//Process All Packets Received
	for (auto pAssemblyBuffer : vAssemblyComplete)
	{
		ServerSystem::GetInstance().ProcessPacket(pSession, pAssemblyBuffer);

		delete pAssemblyBuffer;
	}

	//Repolling Post Recv
	PostRecv(pSession);

	return TRUE;
}

BOOL NetworkSystem::OnSend(IOCP_Message* pIOCP_Message)
{
	Session* pSession = reinterpret_cast<Session*>(pIOCP_Message->m_pOderer);
	
	//Close Sent
	if (0 == pIOCP_Message->m_BytesTransferred == pIOCP_Message->m_WSABUF.len)
	{
		delete pSession;
	}

	//Network Peer TCP Socket Buffer almost run out You Must Shut Session
	else if (pIOCP_Message->m_BytesTransferred < pIOCP_Message->m_WSABUF.len)
	{
		PostClose(pSession);
	}

	//Send Remaining Messages
	else
	{
		PostSend(pSession);
	}

	delete pIOCP_Message;

	return TRUE;
}

BOOL NetworkSystem::OnAccept(IOCP_Message* pIOCP_Message)
{
	Session* pSession = reinterpret_cast<Session*>(pIOCP_Message->m_pOderer);

	//Register New Session To NetworkSystem
	{
		std::lock_guard<std::mutex> lock(m_lock);

		auto pRet = m_uMapSession.insert(UMapSession::value_type(pSession->m_socket, pSession));
		if (false == pRet.second)
		{
			return FALSE;
		}
	}

	//Fill Session Member
	{
		SOCKADDR_IN sAddr = {};
		int	addrLen = sizeof(SOCKADDR);
		getpeername(pSession->m_socket, (SOCKADDR*)(&sAddr), &addrLen);
		pSession->m_addr = sAddr;
	}

	//Adjust Socket Option
	{
		//Nagle Off
		{
			int opt_val = 1;
			setsockopt(pSession->m_socket, IPPROTO_TCP, TCP_NODELAY, (char*)(&opt_val), sizeof(int));
		}

		//Zero Copy
		{
			int opt_val = 0;
			setsockopt(pSession->m_socket, SOL_SOCKET, SO_SNDBUF, (char*)(&opt_val), sizeof(int));
		}

		//No Linger
		{
			linger opt_val = { 1, 0 };
			setsockopt(pSession->m_socket, SOL_SOCKET, SO_LINGER, (char*)(&opt_val), sizeof(linger));
		}
	}

	//Register Client Socket to IOCP.
	{
		auto bRet = IOCP::Register((HANDLE)pSession->m_socket);
		if (TRUE != bRet)
		{
			return FALSE;
		}
	}

	//First Polling Post Recv
	{
		PostRecv(pSession);
	}

	return TRUE;
}