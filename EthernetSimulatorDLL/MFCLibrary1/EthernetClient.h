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
	HANDLE hThread;

	void(*callback_a)(BYTE* frame, DWORD frameSize);
	void(*callback_b)(BYTE* frame, DWORD frameSize, LPVOID args);
	LPVOID callback_args;

public:
	EthernetClient(EthernetHub& hub);
	~EthernetClient();

	BOOL ConnectToHub();
	BOOL FailedBcsNotValidHubPipe();
	void SendData(BYTE* frame, DWORD frameSize);
	void StartHandling(void(*callback)(BYTE* frame, DWORD frameSize));
	void StartHandling(void(*callback)(BYTE* frame, DWORD frameSize, LPVOID args), LPVOID args);
	HANDLE GetPipe();
	DWORD GetClientIndex();
	void InvokeCallback(BYTE* frame, DWORD frameSize);

};

