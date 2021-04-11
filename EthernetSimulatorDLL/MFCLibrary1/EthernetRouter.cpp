#include "pch.h"
#include "EthernetRouter.h"

#include <sstream>
#include <iomanip>
#include <string>

#define DEFAULT_BUFLEN		257
#define FRAME_IP            0x0800

#define SWAPB(Word)         ((WORD)((Word) << 8) | ((Word) >> 8))

static DWORD WINAPI HandlerThread(LPVOID routerPtr);
static void OnNewFrameReceivedCallback(BYTE* frame, DWORD frameSize, LPVOID args);

static void WriteWBE(BYTE* Add, WORD Data);
void WriteDWBE(unsigned char* Add, unsigned long Data);
unsigned short CalcChecksum(void* Start, WORD Count, BYTE IsTCP, WORD MyIP[2], WORD RemoteIP[2]);

EthernetRouter::EthernetRouter(EthernetHub& hub, BYTE RouterMACAddr[6]) : EthernetClient(hub) {
	
	this->MACAddr = std::vector<BYTE>(RouterMACAddr, RouterMACAddr + 6);
	this->ClientSocket = INVALID_SOCKET;

	this->ConnectToHub();
	this->StartHandling(OnNewFrameReceivedCallback, this);
}

void EthernetRouter::StartRouterHandling() {
	DWORD mainThreadId;
	HANDLE hThread;

	hThread = CreateThread(NULL, 0, HandlerThread, this, 0, &mainThreadId);
	if (hThread == NULL) throw std::exception("An error occurred while creating the handler thread");
}

static DWORD WINAPI HandlerThread(LPVOID routerPtr) {
	EthernetRouter *router;
	static char recvbuf[DEFAULT_BUFLEN];
	struct sockaddr_in server_bind;
	struct sockaddr_in client;
	SOCKET main_socket;
	WSADATA wsa;

	router = (EthernetRouter*)routerPtr;

	HANDLE hEvent = CreateEventA(NULL, TRUE, FALSE, "Global\\EVT:SYNACKREC");
	if (hEvent == NULL || hEvent == INVALID_HANDLE_VALUE) throw std::exception("A fatal error occurred during the creation of the Event");

	// Initialize Socket for external COMMs
	// Initializing WINSOCK api
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		// error
	}

	// Creating the socket and binding it to [::]:6589 actually
	if ((main_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		// error
	}

	server_bind.sin_family = AF_INET;
	server_bind.sin_addr.s_addr = INADDR_ANY; // inet_addr("127.0.0.1");
	server_bind.sin_port = htons(6589);

	if (bind(main_socket, (struct sockaddr*)&server_bind, sizeof(server_bind)) == SOCKET_ERROR)
	{
		// error
	}

	// Putting the socket in 'LISTENING' state
	listen(main_socket, SOMAXCONN);

	int c = sizeof(client);
	while (TRUE) {

		SOCKET new_socket = accept(main_socket, (struct sockaddr*)&client, &c);
		if (new_socket == INVALID_SOCKET)
		{
			closesocket(new_socket);
			continue;
		}

		router->ClientSocket = new_socket;

		router->TCPSeqNr = 0;
		router->TCPAckNr = 1;

		// We have to open a connection towards the client, so SYN
		auto tcpFrameOut = router->PrepareTCPFrame(TCP_CODE_SYN);
		router->SendData(&tcpFrameOut[0], tcpFrameOut.size());

		// Now we wait for a SYN ACK
		WaitForSingleObject(hEvent, INFINITE);
		ResetEvent(hEvent);

		// Inc the SEQ number
		router->TCPSeqNr++;

		// SYN ACK recevied. Sending ACK
		tcpFrameOut = router->PrepareTCPFrame(TCP_CODE_ACK);
		router->SendData(&tcpFrameOut[0], tcpFrameOut.size());


		// Receving Data - Ethernet Frame
		int iResult = 0;
		do {

			iResult = recv(new_socket, recvbuf, DEFAULT_BUFLEN, 0);
			recvbuf[iResult] = '\0';

			if (iResult > 0) {
				auto tcpFrame = router->PrepareTCPFrame(std::vector<BYTE>(recvbuf, recvbuf + iResult));
				router->SendData(&tcpFrame[0], tcpFrame.size());
			}
			
			/*std::stringstream ss;
			ss << std::hex;

			for (int i(0); i < tcpFrame.size(); ++i)
				ss << std::setw(2) << std::setfill('0') << (int)tcpFrame.at(i);

			auto x = ss.str();*/


		} while (iResult > 0);

		// We are here, means that the HOST connected to our socket
		// disconnected, so we can send a FIN 

		tcpFrameOut = router->PrepareTCPFrame(TCP_CODE_FIN | TCP_CODE_ACK);
		router->SendData(&tcpFrameOut[0], tcpFrameOut.size());

		// Now we wait for a FIN from the other side
		WaitForSingleObject(hEvent, INFINITE);
		ResetEvent(hEvent);


		router->ClientSocket = INVALID_SOCKET;
		closesocket(new_socket);
	}

	// Why are you here!? Something bad happened.. cleaning up
	WSACleanup();
	
	return 1;
}

static void OnNewFrameReceivedCallback(BYTE* frame, DWORD frameSize, LPVOID args) {
	EthernetRouter* router;

	// TODO: here we should filter all incoming packet
	// and leave only brodcast or towards our router's MAC addr

	router = (EthernetRouter*)args;

	HANDLE hEvent = OpenEventA(EVENT_ALL_ACCESS, TRUE, "Global\\EVT:SYNACKREC");
	if (hEvent == NULL || hEvent == INVALID_HANDLE_VALUE) return;

	std::stringstream ss;
	ss << std::hex;

	for (int i(0); i < frameSize; ++i)
		ss << std::setw(2) << std::setfill('0') << (int)frame[i];

	auto x = ss.str();

	switch (frame[TCP_DATA_CODE_OFS + 1]) {
	
	case (TCP_CODE_SYN | TCP_CODE_ACK): {

		router->TCPAckNr = (((((frame[TCP_SEQNR_OFS] << 8) | frame[TCP_SEQNR_OFS + 1]) << 8) | frame[TCP_SEQNR_OFS + 2]) << 8) | frame[TCP_SEQNR_OFS + 3];
		router->TCPAckNr++;

		// We have to say to the router that we received a SYN ACK
		SetEvent(hEvent);

		break;
	}

	case (TCP_CODE_FIN | TCP_CODE_ACK): {

		// We received the FIN from the other side too
		// SENDING FIN's ACK 
		router->TCPSeqNr++;
		router->TCPAckNr++;
		auto tcpFrameOut = router->PrepareTCPFrame(TCP_CODE_ACK);
		router->SendData(&tcpFrameOut[0], tcpFrameOut.size());

		// Notifying the router that we are done
		SetEvent(hEvent);

		break;
	}

	default: {

		// We received a normal TCP data packet, we have to forward it to the socket
		int datalength = frameSize - (TCP_CHKSUM_OFS + 4);
		
		// But we forward it only if its length is > 0
		// If it's == 0 means that it's not a data packet, maybe it's an ACK!
		// And we don't want to forward empty data packet and ACK them, 
		// else means that we are ACKing an ACK!
		if (datalength) {

			// Forwarding to the router
			send(router->ClientSocket, (char*)TCP_TX_BUF(frame), datalength, 0);

			// Next our SEQ will be the last packet correctly ACKed 
			router->TCPSeqNr = (((((frame[TCP_ACKNR_OFS] << 8) | frame[TCP_ACKNR_OFS + 1]) << 8) | frame[TCP_ACKNR_OFS + 2]) << 8) | frame[TCP_ACKNR_OFS + 3];
			
			// next ACK will ACK the received frame of length 'datalength'
			router->TCPAckNr += datalength; 

			// SENDING ACK
			auto tcpFrameOut = router->PrepareTCPFrame(TCP_CODE_ACK);
			router->SendData(&tcpFrameOut[0], tcpFrameOut.size());
		}

		break;
	}

	}

}

std::vector<BYTE> EthernetRouter::PrepareTCPFrame(unsigned short TCPCode) {
	static BYTE TxFrame[TCP_FRAME_SIZE];
	WORD TxFrame2Size;

	auto DestMAC = new BYTE[6]{ 0x1e, 0x30, 0x6c, 0xa2, 0x45, 0x5e };
	auto DestIP = new BYTE[4]{ 192, 168, 1, 154 };
	auto SrcMAC = new BYTE[6]{ 0x1f, 0x3f, 0x6f, 0xaf, 0x4f, 0x5a };
	auto SrcIP = new BYTE[4]{ 192, 168, 1, 254 };

	WORD TCPLocalPort = 6589;
	WORD TCPRemotePort = 6589;

	// Ethernet
	memcpy(&TxFrame[ETH_DA_OFS], DestMAC, 6);
	memcpy(&TxFrame[ETH_SA_OFS], SrcMAC, 6);
	*(unsigned short*)&TxFrame[ETH_TYPE_OFS] = SWAPB(FRAME_IP);

	// IP   
	*(unsigned short*)&TxFrame[IP_VER_IHL_TOS_OFS] = SWAPB(IP_VER_IHL | IP_TOS_D);

	if (TCPCode & TCP_CODE_SYN)                    // if SYN, we want to use the MSS option
		*(unsigned short*)&TxFrame[IP_TOTAL_LENGTH_OFS] = SWAPB(IP_HEADER_SIZE + TCP_HEADER_SIZE + TCP_OPT_MSS_SIZE);
	else
		*(unsigned short*)&TxFrame[IP_TOTAL_LENGTH_OFS] = SWAPB(IP_HEADER_SIZE + TCP_HEADER_SIZE);

	*(unsigned short*)&TxFrame[IP_IDENT_OFS] = 0;
	*(unsigned short*)&TxFrame[IP_FLAGS_FRAG_OFS] = 0;
	*(unsigned short*)&TxFrame[IP_TTL_PROT_OFS] = SWAPB((DEFAULT_TTL << 8) | PROT_TCP);
	*(unsigned short*)&TxFrame[IP_HEAD_CHKSUM_OFS] = 0;
	memcpy(&TxFrame[IP_SOURCE_OFS], SrcIP, 4);
	memcpy(&TxFrame[IP_DESTINATION_OFS], DestIP, 4);
	*(unsigned short*)&TxFrame[IP_HEAD_CHKSUM_OFS] = CalcChecksum(&TxFrame[IP_VER_IHL_TOS_OFS], IP_HEADER_SIZE, 0, (WORD*)SrcIP, (WORD*)DestIP);

	// TCP
	WriteWBE(&TxFrame[TCP_SRCPORT_OFS], TCPLocalPort);
	WriteWBE(&TxFrame[TCP_DESTPORT_OFS], TCPRemotePort);

	WriteDWBE(&TxFrame[TCP_SEQNR_OFS], TCPSeqNr);
	WriteDWBE(&TxFrame[TCP_ACKNR_OFS], TCPAckNr);

	*(unsigned short*)&TxFrame[TCP_WINDOW_OFS] = SWAPB(MAX_TCP_RX_DATA_SIZE);    // data bytes to accept
	*(unsigned short*)&TxFrame[TCP_CHKSUM_OFS] = 0;             // initalize checksum
	*(unsigned short*)&TxFrame[TCP_URGENT_OFS] = 0;

	if (TCPCode & TCP_CODE_SYN) {                 // if SYN, we want to use the MSS option
		*(unsigned short*)&TxFrame[TCP_DATA_CODE_OFS] = SWAPB(0x6000 | TCPCode);   // TCP header length = 24
		*(unsigned short*)&TxFrame[TCP_DATA_OFS] = SWAPB(TCP_OPT_MSS);             // MSS option
		*(unsigned short*)&TxFrame[TCP_DATA_OFS + 2] = SWAPB(MAX_TCP_RX_DATA_SIZE);// max. length of TCP-data we accept
		*(unsigned short*)&TxFrame[TCP_CHKSUM_OFS] = CalcChecksum(&TxFrame[TCP_SRCPORT_OFS], TCP_HEADER_SIZE + TCP_OPT_MSS_SIZE, 1, (WORD*)SrcIP, (WORD*)DestIP);
		TxFrame2Size = ETH_HEADER_SIZE + IP_HEADER_SIZE + TCP_HEADER_SIZE + TCP_OPT_MSS_SIZE;
	} else {
		*(unsigned short*)&TxFrame[TCP_DATA_CODE_OFS] = SWAPB(0x5000 | TCPCode);   // TCP header length = 20
		*(unsigned short*)&TxFrame[TCP_CHKSUM_OFS] = CalcChecksum(&TxFrame[TCP_SRCPORT_OFS], TCP_HEADER_SIZE, 1, (WORD*)SrcIP, (WORD*)DestIP);
		TxFrame2Size = ETH_HEADER_SIZE + IP_HEADER_SIZE + TCP_HEADER_SIZE;
	}

	return std::vector<BYTE>(TxFrame, TxFrame + TxFrame2Size);
}

std::vector<BYTE> EthernetRouter::PrepareTCPFrame(std::vector<BYTE> data) {
	static BYTE TxFrame[TCP_FRAME_SIZE];

	auto DestMAC = new BYTE[6]{ 0x1e, 0x30, 0x6c, 0xa2, 0x45, 0x5e };
	auto DestIP = new BYTE[4]{ 192, 168, 1, 154 };
	auto SrcMAC = new BYTE[6]{ 0x1f, 0x3f, 0x6f, 0xaf, 0x4f, 0x5a };
	auto SrcIP = new BYTE[4]{ 192, 168, 1, 254 };

	WORD TCPLocalPort = 6589;
	WORD TCPRemotePort = 6589;

	memcpy(&TxFrame[ETH_DA_OFS], DestMAC, 6);
	memcpy(&TxFrame[ETH_SA_OFS], SrcMAC, 6);
	*(WORD *)&TxFrame[ETH_TYPE_OFS] = SWAPB(FRAME_IP);

	// IP   
	*(WORD *)&TxFrame[IP_VER_IHL_TOS_OFS] = SWAPB(IP_VER_IHL | IP_TOS_D);
	WriteWBE(&TxFrame[IP_TOTAL_LENGTH_OFS], IP_HEADER_SIZE + TCP_HEADER_SIZE + data.size());
	*(WORD *)&TxFrame[IP_IDENT_OFS] = 0;
	*(WORD *)&TxFrame[IP_FLAGS_FRAG_OFS] = 0;
	*(WORD *)&TxFrame[IP_TTL_PROT_OFS] = SWAPB((DEFAULT_TTL << 8) | PROT_TCP);
	*(WORD *)&TxFrame[IP_HEAD_CHKSUM_OFS] = 0;
	memcpy(&TxFrame[IP_SOURCE_OFS], SrcIP, 4);
	memcpy(&TxFrame[IP_DESTINATION_OFS], DestIP, 4);
	*(unsigned short*)&TxFrame[IP_HEAD_CHKSUM_OFS] = CalcChecksum(&TxFrame[IP_VER_IHL_TOS_OFS], IP_HEADER_SIZE, 0, (WORD *)SrcIP, (WORD *)DestIP);

	// TCP
	WriteWBE(&TxFrame[TCP_SRCPORT_OFS], TCPLocalPort);
	WriteWBE(&TxFrame[TCP_DESTPORT_OFS], TCPRemotePort);

	WriteDWBE(&TxFrame[TCP_SEQNR_OFS], TCPSeqNr /*TCPSeqNr*/);
	WriteDWBE(&TxFrame[TCP_ACKNR_OFS], TCPAckNr /*TCPAckNr*/);
	*(unsigned short*)&TxFrame[TCP_DATA_CODE_OFS] = SWAPB(0x5000 | TCP_CODE_ACK);   // TCP header length = 20
	*(unsigned short*)&TxFrame[TCP_WINDOW_OFS] = SWAPB(MAX_TCP_RX_DATA_SIZE);       // data bytes to accept
	*(unsigned short*)&TxFrame[TCP_CHKSUM_OFS] = 0;
	*(unsigned short*)&TxFrame[TCP_URGENT_OFS] = 0;
	*(WORD *)&TxFrame[TCP_CHKSUM_OFS] = CalcChecksum(&TxFrame[TCP_SRCPORT_OFS], TCP_HEADER_SIZE + data.size(), 1, (WORD*)SrcIP, (WORD*)DestIP);

	memcpy(TCP_TX_BUF(TxFrame), &data[0], data.size());
	return std::vector<BYTE>(TxFrame, TxFrame + ETH_HEADER_SIZE + IP_HEADER_SIZE + TCP_HEADER_SIZE + data.size());
}

static void WriteWBE(BYTE *Add, WORD Data) {
	*Add++ = Data >> 8;
	*Add = (char)Data;
}

void WriteDWBE(unsigned char* Add, unsigned long Data) {
	*Add++ = Data >> 24;
	*Add++ = Data >> 16;
	*Add++ = Data >> 8;
	*Add = (char)Data;
}


unsigned short CalcChecksum(void* Start, WORD Count, BYTE IsTCP, WORD MyIP[2], WORD RemoteIP[2]) {
	unsigned long Sum = 0;
	WORD *piStart;                        // Keil: Pointer added to correct expression

	if (IsTCP) {                                   // if we've a TCP frame...
		Sum += MyIP[0];                              // ...include TCP pseudo-header
		Sum += MyIP[1];
		Sum += RemoteIP[0];
		Sum += RemoteIP[1];
		Sum += SWAPB(Count);                     // TCP header length plus data length
		Sum += SWAPB(PROT_TCP);
	}

	piStart = (WORD *)Start;
	while (Count > 1) {                            // sum words
		Sum += *piStart++;
		Count -= 2;
	}

	if (Count)                                     // add left-over byte, if any
		Sum += *(unsigned char*)piStart;

	while (Sum >> 16)                              // fold 32-bit sum to 16 bits
		Sum = (Sum & 0xFFFF) + (Sum >> 16);

	return ~Sum;
}