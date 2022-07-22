#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <thread>
#include "Server.h"
#include "Database.h"
#pragma comment(lib, "ws2_32")	
#pragma comment(lib, "libmySQL.lib")		

Server::Server() {
	server = {};
}

void Server::sendData(SOCKET s) {
	vector<ClientSocket>::iterator iter;
	char sBuffer[PACKET_SIZE];
	char cBuffer[PACKET_SIZE];

	while (1) {
		ZeroMemory(&sBuffer, PACKET_SIZE);
		cin >> sBuffer;
		ZeroMemory(&cBuffer, PACKET_SIZE);
		sprintf(cBuffer, "서버로부터 메세지: %s", sBuffer);
		cout << cBuffer << endl;
		for (iter = cSockets.begin(); iter != cSockets.end(); iter++) {
			send(iter->client, cBuffer, PACKET_SIZE, 0);
		}
	}
}

void Server::recvData(SOCKET s, int num) {
	char rBuffer[PACKET_SIZE];
	vector<ClientSocket>::iterator iter;

	cout << num << "번 입장" << endl;
	while (1) {
		ZeroMemory(&rBuffer, PACKET_SIZE);
		recv(s, rBuffer, PACKET_SIZE, 0);
		memcpy((char*)&pkInfo[num], rBuffer, PACKET_SIZE);

		if (WSAGetLastError()) {
			cout << num << "번 퇴장" << endl;
			return;
		}
		if (pkInfo[num].rClientNum != -1 && pkInfo[num].rClientNum <= cSockets.size() - 2) {
			cout << num << "님이 " << pkInfo[num].rClientNum << "님에게 메세지 전달: " << pkInfo[num].msg << endl;
			send(cSockets[pkInfo[num].rClientNum].client, rBuffer, PACKET_SIZE, 0);
		}
		else if (pkInfo[num].rClientNum != -1 && pkInfo[num].rClientNum > cSockets.size() -2) {
			cout << num << "님이 누구에게 메세지 보내는거에요?: " << pkInfo[num].msg << endl;
			send(cSockets[pkInfo[num].rClientNum].client, rBuffer, PACKET_SIZE, 0);
		}
		else {
			cout << num << "님이 모두에게 메세지 전달: " << pkInfo[num].msg << endl;
			for (iter = cSockets.begin(); iter != cSockets.end(); iter++) {
				send(iter->client, rBuffer, PACKET_SIZE, 0);
			}
		}
	}
}

void Server::acceptClients() {
	int number = 0;
	char dsf[100] = {};
	while (1) {
		cSockets.push_back(ClientSocket());
		pkInfo.push_back(PacketInfo());
		cSockets[number].client = accept(server, (SOCKADDR*)&cSockets[number].client_info, &cSockets[number].client_size);
		cSockets[number].number = number;
		_itoa_s(number, dsf, 10);				//int -> char*
		send(cSockets[number].client, dsf, strlen(dsf), 0);
		thread(&Server::recvData, this, cSockets[number].client, number).detach();
		number++;
	}
}

void Server::Open() {
	WSADATA wsaData;							//윈도우 소켓 초기화 구조체
	if (WSAStartup(MAKEWORD(2, 2), &wsaData))	//소켓 2.2버전 사용, unsigned short로 만들기 위해 MAKEWORD 사용
		return;

	server = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);	//IPV4, 연결지향소켓, TCP

	SOCKADDR_IN addr = {};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);			//서버의 IP

	bind(server, (SOCKADDR*)&addr, sizeof(addr));			//소켓, 구성요소 구조체 주소(addr), 크기
	listen(server, SOMAXCONN);							//최대 접속 가능 소켓 수

	thread(&Server::acceptClients, this).detach();
	thread(&Server::sendData, this, server).detach();
}

void Server::Close() {
	for (int i = 0; i < cSockets.size(); i++) closesocket(cSockets[i].client);
	closesocket(server);
	WSACleanup();
}

void Server::Init()
{
	Open();

	Database db;
	db.Connect();

	while (1);

	Close();
}