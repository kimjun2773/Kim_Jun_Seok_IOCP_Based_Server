#include "stdafx.h" 
#include "Packet.h"

void PacketUtil::PeakHeader(AsemblyBuffer* pbuffer, DWORD& packetSize, DWORD& protocolNum)
{
	packetSize = *((DWORD*)(&pbuffer->m_Buffer[0]));
	protocolNum = *((DWORD*)(&pbuffer->m_Buffer[4]));
};

template<>
void PacketUtil::Read(AsemblyBuffer* pbuffer, std::string& str)
{
	int strlen;
	memcpy(&strlen, &pbuffer->m_Buffer[pbuffer->m_curPos], sizeof(int));
	pbuffer->m_curPos += sizeof(int);

	char buffer[2048] = { 0, };
	memcpy(buffer, &pbuffer->m_Buffer[pbuffer->m_curPos], strlen);
	pbuffer->m_curPos += strlen;

	str = buffer;
};

template<>
void PacketUtil::Write(std::string& str, AsemblyBuffer* pbuffer)
{
	int strlen = str.length();
	memcpy(&pbuffer->m_Buffer[pbuffer->m_curPos], &strlen, sizeof(int));
	pbuffer->m_curPos += sizeof(int);

	memcpy(&pbuffer->m_Buffer[pbuffer->m_curPos], str.c_str(), strlen);
	pbuffer->m_curPos += strlen;
};

void Packet_ChatMessage::Read(AsemblyBuffer* pBuffer)
{
	PacketUtil::Read(pBuffer, m_packetSize);
	PacketUtil::Read(pBuffer, m_protocolNum);
	PacketUtil::Read(pBuffer, m_chatMsg);
}

void Packet_ChatMessage::Write(AsemblyBuffer* pBuffer)
{
	PacketUtil::Write(m_packetSize, pBuffer);
	PacketUtil::Write(m_protocolNum, pBuffer);
	PacketUtil::Write(m_chatMsg, pBuffer);

	pBuffer->m_endPos = pBuffer->m_curPos;
}