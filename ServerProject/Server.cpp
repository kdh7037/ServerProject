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
		sprintf(cBuffer, "�����κ��� �޼���: %s", sBuffer);
		cout << cBuffer << endl;
		for (iter = cSockets.begin(); iter != cSockets.end(); iter++) {
			send(iter->client, cBuffer, PACKET_SIZE, 0);
		}
	}
}

void Server::recvData(SOCKET s, int num) {
	char rBuffer[PACKET_SIZE];
	vector<ClientSocket>::iterator iter;

	cout << num << "�� ����" << endl;
	while (1) {
		ZeroMemory(&rBuffer, PACKET_SIZE);
		recv(s, rBuffer, PACKET_SIZE, 0);
		memcpy((char*)&pkInfo[num], rBuffer, PACKET_SIZE);

		if (WSAGetLastError()) {
			cout << num << "�� ����" << endl;
			return;
		}
		if (pkInfo[num].rClientNum != -1 && pkInfo[num].rClientNum <= cSockets.size() - 2) {
			cout << num << "���� " << pkInfo[num].rClientNum << "�Կ��� �޼��� ����: " << pkInfo[num].msg << endl;
			send(cSockets[pkInfo[num].rClientNum].client, rBuffer, PACKET_SIZE, 0);
		}
		else if (pkInfo[num].rClientNum != -1 && pkInfo[num].rClientNum > cSockets.size() -2) {
			cout << num << "���� �������� �޼��� �����°ſ���?: " << pkInfo[num].msg << endl;
			send(cSockets[pkInfo[num].rClientNum].client, rBuffer, PACKET_SIZE, 0);
		}
		else {
			cout << num << "���� ��ο��� �޼��� ����: " << pkInfo[num].msg << endl;
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
	WSADATA wsaData;							//������ ���� �ʱ�ȭ ����ü
	if (WSAStartup(MAKEWORD(2, 2), &wsaData))	//���� 2.2���� ���, unsigned short�� ����� ���� MAKEWORD ���
		return;

	server = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);	//IPV4, �����������, TCP

	SOCKADDR_IN addr = {};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);			//������ IP

	bind(server, (SOCKADDR*)&addr, sizeof(addr));			//����, ������� ����ü �ּ�(addr), ũ��
	listen(server, SOMAXCONN);							//�ִ� ���� ���� ���� ��

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