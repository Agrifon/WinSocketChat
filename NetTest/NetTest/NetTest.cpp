#include <iostream>
#include <stdio.h>
#include <vector>
#include <thread>

//Network library
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

#define IP "127.0.0.1"
#define PORT 1234
#define BUFF_SIZE 1024

void reception(SOCKET ClientConn, std::vector<char> serverBuff);
void broadcast(SOCKET ClientConn, std::vector<char> clientBuff);

int main()
{
	WSADATA wsData;

	int erStat = WSAStartup(MAKEWORD(2,2), &wsData);
	if (erStat != 0)
	{
		std::cout << "Error WinSock version initializaion #" << std::endl;
		std::cout << WSAGetLastError() << std::endl;
		WSACleanup();

		return 1;
	}
	else
	{
		std::cout << "WinSock initialization is OK" << std::endl;
	}

	SOCKET ServSock = socket(AF_INET, SOCK_STREAM, 0);
	if (ServSock == INVALID_SOCKET)
	{
		std::cout << "Error initialization socket # " << WSAGetLastError() << std::endl;
		closesocket(ServSock);
		WSACleanup();

		return 1;
	}
	else
	{
		std::cout << "Server socket initialization is OK" << std::endl;
	}

	in_addr ip_to_num;
	erStat = inet_pton(AF_INET, IP, &ip_to_num);
	if (erStat <= 0)
	{
		std::cout << "Error in IP translation to special numeric format" << std::endl;

		return 1;
	}

	sockaddr_in servInfo;
	ZeroMemory(&servInfo, sizeof(servInfo));

	servInfo.sin_family = AF_INET;
	servInfo.sin_addr = ip_to_num;
	servInfo.sin_port = htons(PORT);

	erStat = bind(ServSock, (sockaddr*)&servInfo, sizeof(servInfo));
	if (erStat != 0)
	{
		std::cout << "Error Socket binding to server info. Error # " << WSAGetLastError() << std::endl;
		closesocket(ServSock);
		WSACleanup();

		return 1;
	}
	else
	{
		std::cout << "Binding socket to Server info is OK" << std::endl;
	}

	erStat = listen(ServSock, SOMAXCONN);
	if (erStat != 0)
	{
		std::cout << "Can't start to listen to. Error # " << WSAGetLastError() << std::endl;
		closesocket(ServSock);
		WSACleanup();
		return 1;
	}
	else
	{
		std::cout << "Listening... " << IP << ":" << PORT << std::endl;
	}

	sockaddr_in clientInfo;
	ZeroMemory(&clientInfo, sizeof(clientInfo));

	int clientInfo_size = sizeof(clientInfo);
	
	SOCKET ClientConn = accept(ServSock, (sockaddr*)&clientInfo, &clientInfo_size);
	if (ClientConn == INVALID_SOCKET) {
		std::cout << "Client detected, but can't connect to a client. Error # " << WSAGetLastError() << std::endl;
		closesocket(ServSock);
		WSACleanup();

		return 1;
	}
	else
	{
		std::cout << "Connection to a client established successfully" << std::endl;
	}

	std::vector<char> servBuff(BUFF_SIZE), clientBuff(BUFF_SIZE);

	std::thread thSend(broadcast, std::ref(ClientConn), std::ref(clientBuff));
	std::thread thRecv(reception, std::ref(ClientConn), std::ref(servBuff));

	while (true)
	{

	}

	thSend.join();
	thRecv.join();
	return 0;
}

void reception(SOCKET ClientConn, std::vector<char> servBuff)
{
	while (true)
	{
		short packet_size = 0;
		packet_size = recv(ClientConn, servBuff.data(), servBuff.size(), 0);
		if (packet_size == SOCKET_ERROR)
		{
			std::cout << "Can't accept message from Client. Error # " << WSAGetLastError() << std::endl;
			closesocket(ClientConn);
			WSACleanup();

			exit(1);
		}
		else
		{
			std::cout << '\r' << "                                                                    " << std::endl;
			std::cout << "Client's message: " << servBuff.data() << std::endl;
		}
	}
}

void broadcast(SOCKET ClientConn, std::vector<char> clientBuff)
{
	while (true)
	{
		short packet_size = 0;
		std::cout << "Your (host) message: ";
		fgets(clientBuff.data(), clientBuff.size(), stdin);

		if (clientBuff.at(0) == 'x' && clientBuff.at(1) == 'x' && clientBuff.at(2) == 'x') {
			shutdown(ClientConn, SD_BOTH);
			closesocket(ClientConn);
			WSACleanup();

			exit(1);
		}

		packet_size = send(ClientConn, clientBuff.data(), clientBuff.size(), 0);
		if (packet_size == SOCKET_ERROR)
		{
			std::cout << "Can't send message to Client. Error # " << WSAGetLastError() << std::endl;
			closesocket(ClientConn);
			WSACleanup();

			exit(1);
		}
	}
}