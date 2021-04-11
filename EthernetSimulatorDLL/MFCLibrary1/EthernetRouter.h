#pragma once

#include "EthernetClient.h"
#include <vector>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib") //Winsock Library

class EthernetRouter : public EthernetClient {

private:

	HANDLE hThread;
	SOCKET main_socket;

	std::vector<BYTE> MACAddr;
	std::vector<BYTE> IPAddr;
	WORD TCPState;

public:
	EthernetRouter(EthernetHub& hub, BYTE RouterMACAddr[6], BYTE RouterIPAddr[4]);
	~EthernetRouter();
	
	SOCKET GetMainSocket();
	SOCKET SetMainSocket(SOCKET s);

	DWORD TCPSeqNr;
	DWORD TCPAckNr;

	// Temporary NAT table
	WORD TCPRouterPort = 6589;
	WORD TCPRemotePort = 6589;
	std::vector<BYTE> IPRemoteAddr = { 192, 168, 2, 154 };

	SOCKET ClientSocket;
	struct sockaddr_in ClientInfo;
	std::vector<BYTE> ClientMACAddr;

	BYTE* GetMACAddress();
	BYTE* GetIPAddress();

	void StartRouterHandling();
	std::vector<BYTE> PrepareARPRequest(std::vector<BYTE> RemoteIP);
	std::vector<BYTE> PrepareTCPFrame(unsigned short TCPCode);
	std::vector<BYTE> PrepareTCPFrame(std::vector<BYTE> data);

};

