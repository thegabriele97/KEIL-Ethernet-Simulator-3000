#pragma once

#include "EthernetClient.h"
#include <vector>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib") //Winsock Library

#define MAX_TCP_TX_DATA_SIZE 512                 // max. outgoing TCP data size (even!)
#define MAX_TCP_RX_DATA_SIZE 256                 // max. incoming TCP data size (even!)

#define DEFAULT_TTL          64                  // Time To Live sent with packets

// Ethernet network layer definitions
#define ETH_DA_OFS           0                   // Destination MAC address (48 Bit)
#define ETH_SA_OFS           6                   // Source MAC address (48 Bit)
#define ETH_TYPE_OFS         12                  // Type field (16 Bit)
#define ETH_DATA_OFS         14                  // Frame Data
#define ETH_HEADER_SIZE      14

#define FRAME_ARP            0x0806              // frame types (stored in Type/Length field)
#define FRAME_IP             0x0800

// IPv4 layer definitions
#define IP_VER_IHL_TOS_OFS   ETH_DATA_OFS + 0    // Version, Header Length, Type of Service
#define IP_TOTAL_LENGTH_OFS  ETH_DATA_OFS + 2    // IP Frame's Total Length
#define IP_IDENT_OFS         ETH_DATA_OFS + 4    // Identifying Value
#define IP_FLAGS_FRAG_OFS    ETH_DATA_OFS + 6    // Flags and Fragment Offset
#define IP_TTL_PROT_OFS      ETH_DATA_OFS + 8    // Frame's Time to Live, Protocol
#define IP_HEAD_CHKSUM_OFS   ETH_DATA_OFS + 10   // IP Frame's Header Checksum
#define IP_SOURCE_OFS        ETH_DATA_OFS + 12   // Source Address (32 Bit)
#define IP_DESTINATION_OFS   ETH_DATA_OFS + 16   // Destination Address (32 Bit)
#define IP_DATA_OFS          ETH_DATA_OFS + 20   // Frame Data (if no options)
#define IP_HEADER_SIZE       20                  // w/o options

#define IP_VER_IHL           0x4500              // IPv4, Header Length = 5x32 bit
#define IP_TOS_D             0x0010              // TOS low delay
#define IP_TOS_T             0x0008              // TOS high throughput
#define IP_TOS_R             0x0004              // TOS high reliability

#define IP_FLAG_DONTFRAG     0x4000              // don't fragment IP frame
#define IP_FLAG_MOREFRAG     0x2000              // more fragments available
#define IP_FRAGOFS_MASK      0x1FFF              // indicates where this fragment belongs

#define PROT_ICMP            1                   // Internet Control Message Protocol
#define PROT_TCP             6                   // Transmission Control Protocol
#define PROT_UDP             17                  // User Datagram Protocol

// ARP definitions
#define ARP_HARDW_OFS        ETH_DATA_OFS + 0    // Hardware address type
#define ARP_PROT_OFS         ETH_DATA_OFS + 2    // Protocol
#define ARP_HLEN_PLEN_OFS    ETH_DATA_OFS + 4    // byte length of each hardw. / prot. address
#define ARP_OPCODE_OFS       ETH_DATA_OFS + 6    // Opcode
#define ARP_SENDER_HA_OFS    ETH_DATA_OFS + 8    // Hardw. address of sender of this packet
#define ARP_SENDER_IP_OFS    ETH_DATA_OFS + 14   // IP address of sender
#define ARP_TARGET_HA_OFS    ETH_DATA_OFS + 18   // Hardw. address of target of this packet
#define ARP_TARGET_IP_OFS    ETH_DATA_OFS + 24   // IP address of target
#define ARP_FRAME_SIZE       28

#define HARDW_ETH10          1                   // hardware-type 10Mbps Ethernet
#define IP_HLEN_PLEN         0x0604              // MAC = 6 byte long, IP = 4 byte long
#define OP_ARP_REQUEST       1                   // operations for ARP-frames
#define OP_ARP_ANSWER        2

// ICMP definitions
#define ICMP_TYPE_CODE_OFS   IP_DATA_OFS + 0     // type of message
#define ICMP_CHKSUM_OFS      IP_DATA_OFS + 2     // checksum of ICMP-message (16 bit)
#define ICMP_DATA_OFS        IP_DATA_OFS + 4
#define ICMP_HEADER_SIZE     4

#define ICMP_ECHO            8                   // message is an echo request
#define ICMP_ECHO_REPLY      0                   // message is an echo reply

// TCP layer definitions
#define TCP_SRCPORT_OFS      IP_DATA_OFS + 0     // Source Port (16 bit)
#define TCP_DESTPORT_OFS     IP_DATA_OFS + 2     // Destination Port (16 bit)
#define TCP_SEQNR_OFS        IP_DATA_OFS + 4     // Sequence Number (32 bit)
#define TCP_ACKNR_OFS        IP_DATA_OFS + 8     // Acknowledge Number (32 bit)
#define TCP_DATA_CODE_OFS    IP_DATA_OFS + 12    // Data Offset and Control Bits (16 bit)
#define TCP_WINDOW_OFS       IP_DATA_OFS + 14    // Window Size (16 bit)
#define TCP_CHKSUM_OFS       IP_DATA_OFS + 16    // Checksum Field (16 bit)
#define TCP_URGENT_OFS       IP_DATA_OFS + 18    // Urgent Pointer (16 bit)
#define TCP_DATA_OFS         IP_DATA_OFS + 20    // Frame Data (if no options)
#define TCP_HEADER_SIZE      20                  // size w/o options

#define TCP_CODE_FIN         0x0001
#define TCP_CODE_SYN         0x0002
#define TCP_CODE_RST         0x0004
#define TCP_CODE_PSH         0x0008
#define TCP_CODE_ACK         0x0010
#define TCP_CODE_URG         0x0020

#define TCP_OPT_MSS          0x0204              // Type 2, Option Length 4 (Max. Segment Size)
#define TCP_OPT_MSS_SIZE     4

#define TCP_FRAME_SIZE		((ETH_HEADER_SIZE + IP_HEADER_SIZE + TCP_HEADER_SIZE + MAX_TCP_TX_DATA_SIZE) / 2)
#define TCP_TX_BUF(v)		((unsigned char *)v + ETH_HEADER_SIZE + IP_HEADER_SIZE + TCP_HEADER_SIZE)

class EthernetRouter : public EthernetClient {

private:
	std::vector<BYTE> MACAddr;
	std::vector<BYTE> IPAddr;
	WORD TCPState;

public:
	EthernetRouter(EthernetHub& hub, BYTE RouterMACAddr[6], BYTE RouterIPAddr[4]);
	//EthernetRouter(EthernetHub& hub, std::string RouterMACAddr);
	

	//~EthernetRouter();

	DWORD TCPSeqNr;
	DWORD TCPAckNr;

	SOCKET ClientSocket;

	BYTE* GetMACAddress();
	BYTE* GetIPAddress();

	void StartRouterHandling();
	std::vector<BYTE> PrepareARPRequest(std::vector<BYTE> RemoteIP);
	std::vector<BYTE> PrepareTCPFrame(unsigned short TCPCode);
	std::vector<BYTE> PrepareTCPFrame(std::vector<BYTE> data);

};

