#pragma once

#include "EthernetHub.h"
#include <string>
#include <memory>

class EthernetClient {

private:
	DWORD clientIndex;
	HANDLE hPipe;
	EthernetHub *hub;
	std::string PipeName;
	void(*callback)(BYTE* frame, DWORD frameSize);
	HANDLE hThread;

public:
	EthernetClient(EthernetHub& hub);
	~EthernetClient();

	BOOL ConnectToHub();
	BOOL FailedBcsNotValidHubPipe();
	void SendData(BYTE* frame, DWORD frameSize);
	void StartHandling(void(*callback)(BYTE* frame, DWORD frameSize));
	HANDLE GetPipe();
	DWORD GetClientIndex();
	void InvokeCallback(BYTE* frame, DWORD frameSize);

};

