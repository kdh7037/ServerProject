#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <WInSock2.h>				//소켓 헤더
#include <thread>
#include <atomic>
#pragma comment(lib, "ws2_32")		//헤더를 사용하기 위한 라이브러리 링크
#define	PORT		4567
#define	PACKET_SIZE	1024
#define SERVER_IP	"127.0.0.1"

using namespace std;

atomic<bool> broken(false);

typedef struct {
	int sClientNum;
	int rClientNum;
	char msg[PACKET_SIZE];
} PacketInfo;

PacketInfo sendInfo, recvInfo;

void recvData(SOCKET& server) {
	char buff[PACKET_SIZE] = {};
	while (1) {
		ZeroMemory(&buff, PACKET_SIZE);
		recv(server, buff, PACKET_SIZE, 0);
		memcpy((char*)&recvInfo, buff, PACKET_SIZE);
		if (WSAGetLastError()) {
			broken.store(true, memory_order_release);
			return;
		}
		if (recvInfo.rClientNum != -1)
			cout << recvInfo.sClientNum << "님의 메세지: " << recvInfo.msg << endl;
		else
			cout << "모두에게 메세지: " << recvInfo.msg << endl;
	}
}

void sendData(SOCKET& server) {
	char msg[PACKET_SIZE] = {};
	char rNum[100] = {};
	while (1) {
		cin >> msg >> rNum;
		if (atoi(rNum) >= -1) {
			strcpy_s(sendInfo.msg, msg);
			sendInfo.rClientNum = atoi(rNum);
			send(server, (char*)&sendInfo, PACKET_SIZE, 0);
		}
	}
}

int main()
{
	WSADATA wsaData;							//윈도우 소켓 초기화 구조체
	if (WSAStartup(MAKEWORD(2, 2), &wsaData))	//소켓 2.2버전 사용, unsigned short로 만들기 위해 MAKEWORD 사용
		return -1;

	SOCKET server = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);	//IPV4, 연결지향소켓, TCP
	if (server == INVALID_SOCKET) return -1;

	SOCKADDR_IN addr = {};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);
	addr.sin_addr.s_addr = inet_addr(SERVER_IP);		//서버의 IP

	while (connect(server, (SOCKADDR*)&addr, sizeof(addr)));

	char num[100] = {};
	recv(server, num, 100, 0);
	sendInfo.sClientNum = atoi(num);

	thread(recvData, ref(server)).detach();
	thread(sendData, ref(server)).detach();

	while (!broken.load(memory_order_acquire));
	closesocket(server);

	WSACleanup();
	return 0;
}