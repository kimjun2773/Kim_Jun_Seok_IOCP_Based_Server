#include <stdio.h>
#include <tchar.h>
#include <assert.h>
#include <WinSock2.h>
#include <mswsock.h>
#include<Windows.h>
#include <thread>
#include <vector>
#include <WS2tcpip.h>
#include <iostream>

#pragma comment(lib,"ws2_32")

using namespace std;

int main()
{
	//StartUp Winsock DLL
	{
		WSADATA wsaData;

		auto ret = ::WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (0 != ret)
		{
			assert(false);

			::WSACleanup();
			return 0;
		}
	}

	Sleep(5000);
	
	SOCKET socket			= ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	SOCKADDR_IN serverInfo	= {};
	serverInfo.sin_family	= AF_INET;
	serverInfo.sin_port		= htons(1443);
	ULONG addr;
	auto result = inet_pton(AF_INET, "127.0.0.1", &addr);
	if (result != SOCKET_ERROR)
	{
		serverInfo.sin_addr.s_addr = addr;
	}

	connect(
		socket,
		(SOCKADDR*)(&serverInfo),
		sizeof(serverInfo)
	);

	while (1)
	{
		char str[1024] = "";
		cin.getline(str, 1024);
		int StrLen			= strlen(str) + 1;
		int PacketSize		= sizeof(int) + sizeof(int) + sizeof(int) + StrLen;
		int PacketNumber	= 1;


		char Packet[2048] = {0};
		memcpy(&Packet[0], &PacketSize, sizeof(int));
		memcpy(&Packet[4], &PacketNumber, sizeof(int));
		memcpy(&Packet[8], &StrLen, sizeof(int));
		memcpy(&Packet[12], str, StrLen - 10);
		send(socket, Packet, PacketSize-10, 0);

		memcpy(&Packet[0], &str[StrLen - 10], 10);
		send(socket, Packet, 10, 0);


		char buff[2048] = { 0 };
		auto ret = recv(socket, buff, 2048, 0);
		cout << &buff[12] << endl;
	}

	while (1)
	{

	}
	return 0;
}

