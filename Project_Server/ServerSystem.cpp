#include "stdafx.h" 
#include "ServerSystem.h"
#include "Packet.h"
#include "Session.h"

#include <iostream>

using namespace std;

ServerSystem::ServerSystem(void)
{

}

ServerSystem::~ServerSystem(void)
{

}

void ServerSystem::Init(void)
{

}

void ServerSystem::ProcessPacket(Session* pSession, AsemblyBuffer* pbuffer)
{
	DWORD packetSize	= 0;
	DWORD protocolNum	= 0;
	PacketUtil::PeakHeader(pbuffer, packetSize, protocolNum);

	switch (protocolNum)
	{
	case ePacket_ChatMessage: 
	{
		Packet_ChatMessage packet;
		packet.Read(pbuffer);
		
		cout << packet.m_chatMsg <<endl;

		pSession->SendPacket(&packet);

		break;
	}
	}
}