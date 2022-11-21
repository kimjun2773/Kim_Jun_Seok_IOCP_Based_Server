#pragma once

#pragma(push,1)

struct AsemblyBuffer;
namespace PacketUtil
{
	void PeakHeader(AsemblyBuffer* pbuffer, DWORD& packetSize, DWORD& protocolNum);

	template<typename T>
	void Read(AsemblyBuffer* pbuffer, T& memeber)
	{
		memcpy(&memeber, &pbuffer->m_Buffer[pbuffer->m_curPos], sizeof(T));
		pbuffer->m_curPos += sizeof(T);
	};

	template<typename T>
	void Write(T& memeber, AsemblyBuffer* pbuffer)
	{
		memcpy(&pbuffer->m_Buffer[pbuffer->m_curPos], &memeber, sizeof(T));
		pbuffer->m_curPos += sizeof(T);
	};
}

enum PROTOCOL
{
	ePacket_None,
	ePacket_ChatMessage,
};

struct Packet
{
	DWORD m_packetSize;
	DWORD m_protocolNum;

	virtual void Read(AsemblyBuffer* pBuffer)	= 0;
	virtual void Write(AsemblyBuffer* pBuffer)	= 0;
};

struct Packet_ChatMessage : public Packet
{
	std::string m_chatMsg;

	virtual void Read(AsemblyBuffer* pBuffer);
	virtual void Write(AsemblyBuffer* pBuffer);
};

#pragma(pop)
