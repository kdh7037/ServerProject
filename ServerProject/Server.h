#pragma once
#ifndef SERVER_H
#define SERVER_H


#define	PORT		4567
#define	PACKET_SIZE	1024

#include <vector>
#include <thread>
#include <WInSock2.h>				//소켓 헤더
#include <mysql.h>
#pragma comment(lib, "ws2_32")	
#pragma comment(lib, "libmySQL.lib")		

using namespace std;

typedef struct {
	int sClientNum;
	int rClientNum;
	char msg[PACKET_SIZE];
} PacketInfo;

class ClientSocket {
public:
	SOCKET client;
	SOCKADDR_IN client_info;
	int client_size;
	int number;
	ClientSocket() {
		client = {};
		client_info = {};
		client_size = sizeof(client_info);
		number = -1;
	}
	~ClientSocket() {
		client = {};
		client_info = {};
		client_size = -1;
		number = -1;
	}
};


class Server {
public:
	Server();
	~Server() {}
	vector<ClientSocket> cSockets;
	vector<PacketInfo> pkInfo;

public:
	void Init();

private:
	SOCKET server;
	void Open();
	void Close();
	void acceptClients();
	void recvData(SOCKET s, int num);
	void sendData(SOCKET s);
};
#endif