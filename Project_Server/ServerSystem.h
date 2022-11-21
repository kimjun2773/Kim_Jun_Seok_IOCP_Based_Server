#pragma once

class ServerSystem : public	Singleton<ServerSystem>
{
public:						ServerSystem(void);
public:						~ServerSystem(void);

public: void				Init(void);
public: void				ProcessPacket(Session* pSession, AsemblyBuffer* pbuffer);

private: std::mutex			m_lock;
};