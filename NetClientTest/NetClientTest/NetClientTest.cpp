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

void reception(SOCKET ClientSock, std::vector<char> servBuff);
void broadcast(SOCKET ClientSock, std::vector<char> clientBuff);

int main()
{
	WSADATA wsData;

	int erStat = WSAStartup(MAKEWORD(2, 2), &wsData);
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

	SOCKET ClientSock = socket(AF_INET, SOCK_STREAM, 0);
	if (ClientSock == INVALID_SOCKET)
	{
		std::cout << "Error initialization socket # " << WSAGetLastError() << std::endl;
		closesocket(ClientSock);
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

	sockaddr_in serverInfo;
	ZeroMemory(&serverInfo, sizeof(serverInfo));

	serverInfo.sin_family = AF_INET;
	serverInfo.sin_addr = ip_to_num;
	serverInfo.sin_port = htons(PORT);

	erStat = connect(ClientSock, (sockaddr*)&serverInfo, sizeof(serverInfo));
	if (erStat != 0) {
		std::cout << "Connection to Server is FAILED. Error # " << WSAGetLastError() << std::endl;
		closesocket(ClientSock);
		WSACleanup();
		return 1;
	}
	else
	{
		std::cout << "Connection established SUCCESSFULLY. Ready to send a message to Server" << std::endl;
	}

	std::vector<char> servBuff(BUFF_SIZE), clientBuff(BUFF_SIZE);

	std::thread thSend(broadcast, std::ref(ClientSock), std::ref(clientBuff));
	std::thread thRecv(reception, std::ref(ClientSock), std::ref(servBuff));
		/*
		packet_size = send(ClientSock, clientBuff.data(), clientBuff.size(), 0);
		if (packet_size == SOCKET_ERROR) {
			std::cout << "Can't send message to Client. Error # " << WSAGetLastError() << std::endl;
			closesocket(ClientSock);
			WSACleanup();

			return 1;
		}

		packet_size = recv(ClientSock, servBuff.data(), servBuff.size(), 0);
		if (packet_size == SOCKET_ERROR) {
			std::cout << "Can't send message to Client. Error # " << WSAGetLastError() << std::endl;
			closesocket(ClientSock);
			WSACleanup();

			return 1;
		}
		else
		{
			std::cout << "Server message: " << servBuff.data() << std::endl;
		}
		*/
	while (true)
	{

	}

	thSend.join();
	thRecv.join();
	return 0;
}

void reception(SOCKET ClientSock, std::vector<char> servBuff)
{
	short packet_size = 0;

	while(true)
	{
		packet_size = recv(ClientSock, servBuff.data(), servBuff.size(), 0);

		if (packet_size == SOCKET_ERROR) {
			std::cout << "Can't send message to Client. Error # " << WSAGetLastError() << std::endl;
			closesocket(ClientSock);
			WSACleanup();

			exit(1);
		}
		else
		{
			std::cout << '\r' << "                                                                    " << std::endl;
			std::cout << "Server message: " << servBuff.data() << std::endl;
		}
	}
}

void broadcast(SOCKET ClientSock, std::vector<char> clientBuff)
{
	short packet_size = 0;

	while(true)
	{
		std::cout << "Your (Client) message to Server: ";
		fgets(clientBuff.data(), clientBuff.size(), stdin);

		if (clientBuff.at(0) == 'x' && clientBuff.at(1) == 'x' && clientBuff.at(2) == 'x')
		{
			shutdown(ClientSock, SD_BOTH);
			closesocket(ClientSock);
			WSACleanup();

			exit(1);
		}

		packet_size = send(ClientSock, clientBuff.data(), clientBuff.size(), 0);

		if (packet_size == SOCKET_ERROR) {
			std::cout << "Can't send message to Client. Error # " << WSAGetLastError() << std::endl;
			closesocket(ClientSock);
			WSACleanup();

			exit(1);
		}
	}
}