#include "pch.h"
#include "EthernetClient.h"
#include "EthernetCommon.h"
#include <exception>
#include <string>

static DWORD WINAPI HandlerThread(LPVOID arg);

EthernetClient::EthernetClient(EthernetHub& hub) {
	this->hub = &hub;
	this->hPipe = NULL;
	this->clientIndex = -1;
}

EthernetClient::~EthernetClient() {

	if (this->hub->CreationSucceded()) { // local hub
		this->hub->GetClients().erase(this->clientIndex);
		this->hub->UpdateClientsCount(-1);
	} else { // remote hub
		WaitNamedPipeA(this->hub->GetPipeName().c_str(), NMPWAIT_WAIT_FOREVER);
		HANDLE hubPipe = CreateFileA(
			hub->GetPipeName().c_str(), GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		if (hubPipe != INVALID_HANDLE_VALUE) {
			DWORD nbytes;
			ETHNOTIFY data = DELCLIENT;
			
			WriteFile(hubPipe, &data, sizeof data, &nbytes, NULL);
			WriteFile(hubPipe, &this->clientIndex, sizeof this->clientIndex, &nbytes, NULL);
		}
	}
	
	TerminateThread(this->hThread, 0x0);
	CloseHandle(this->hPipe);
}

BOOL EthernetClient::ConnectToHub() {
	HANDLE handshakePipe;
	BOOL result;
	DWORD nbytes;
	ETHNOTIFY data;

	WaitNamedPipeA(hub->GetPipeName().c_str(), NMPWAIT_WAIT_FOREVER);
	handshakePipe = CreateFileA(
		hub->GetPipeName().c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (handshakePipe == INVALID_HANDLE_VALUE) return (CloseHandle(handshakePipe), FALSE);

	// Asking for new ID
	data = NEWCLIENT;
	result = WriteFile(handshakePipe, &data, sizeof data, &nbytes, NULL);
	if (!result) return (CloseHandle(handshakePipe), FALSE);

	// Waiting for response
	result = ReadFile(handshakePipe, &this->clientIndex, sizeof this->clientIndex, &nbytes, NULL);
	if (!result) return (CloseHandle(handshakePipe), FALSE);

	// Received: creating new pipe towards the hub with the received ID
	this->PipeName = std::string("\\\\.\\pipe\\" + this->hub->GetHubName() + ":" + std::to_string(this->clientIndex));
	this->hPipe = CreateNamedPipeA(
		this->PipeName.c_str(),
		PIPE_ACCESS_DUPLEX | FILE_FLAG_FIRST_PIPE_INSTANCE | FILE_FLAG_WRITE_THROUGH,
		PIPE_TYPE_BYTE | PIPE_WAIT,
		0x1, 0x0, 0x0, 0x0, NULL);

	if (this->hPipe == INVALID_HANDLE_VALUE) return (CloseHandle(handshakePipe), FALSE);
	
	// Done: notifying the HUB we are done
	data = ACK_NEWCLIENT;
	result = WriteFile(handshakePipe, &data, sizeof data, &nbytes, NULL);
	if (!result) return (CloseHandle(handshakePipe), CloseHandle(this->hPipe), FALSE);

	// Sending back pipe name length
	size_t len = this->PipeName.length();
	result = WriteFile(handshakePipe, &len, sizeof len, &nbytes, NULL);
	if (!result) return (CloseHandle(handshakePipe), CloseHandle(this->hPipe), FALSE);

	// Sending back pipe name
	result = WriteFile(handshakePipe, this->PipeName.c_str(), len, &nbytes, NULL);
	if (!result) return (CloseHandle(handshakePipe), CloseHandle(this->hPipe), FALSE);

	DisconnectNamedPipe(handshakePipe);
	CloseHandle(handshakePipe);
	return TRUE;
}

BOOL EthernetClient::FailedBcsNotValidHubPipe() {
	return GetLastError() == ERROR_ACCESS_DENIED;
}

void EthernetClient::SendData(BYTE* frame, DWORD frameSize) {
	HANDLE hPipe;
	DWORD nbytes;
	ETHNOTIFY data;

	// Notifying new data to hub
	WaitNamedPipeA(hub->GetPipeName().c_str(), NMPWAIT_WAIT_FOREVER);
	hPipe = CreateFileA(
		hub->GetPipeName().c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hPipe == INVALID_HANDLE_VALUE) return;

	data = NEWDATA;
	WriteFile(hPipe, &data, sizeof data, &nbytes, NULL);
	WriteFile(hPipe, &this->clientIndex, sizeof this->clientIndex, &nbytes, NULL);

	DisconnectNamedPipe(hPipe);
	CloseHandle(hPipe);
	
	// Sending now the ethernet frame
	BOOL fConnected = ConnectNamedPipe(this->hPipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);

	if (fConnected) {
		WriteFile(this->hPipe, &frameSize, sizeof frameSize, &nbytes, NULL);
		WriteFile(this->hPipe, frame, frameSize, &nbytes, NULL);

		DisconnectNamedPipe(this->hPipe);
	}
}

void EthernetClient::StartHandling(void(*callback)(BYTE* frame, DWORD frameSize)) {
	DWORD mainThreadId;

	this->callback_a = callback;
	this->callback_args = NULL;

	this->hThread = CreateThread(NULL, 0, HandlerThread, this, 0, &mainThreadId);
	if (this->hThread == NULL) throw std::exception("An error occurred while creating the handler thread");
}

void EthernetClient::StartHandling(void(*callback)(BYTE* frame, DWORD frameSize, LPVOID args), LPVOID args) {
	DWORD mainThreadId;

	this->callback_b = callback;
	this->callback_args = args;

	this->hThread = CreateThread(NULL, 0, HandlerThread, this, 0, &mainThreadId);
	if (this->hThread == NULL) throw std::exception("An error occurred while creating the handler thread");
}

HANDLE EthernetClient::GetPipe() {
	return this->hPipe;
}

DWORD EthernetClient::GetClientIndex() {
	return this->clientIndex;
}

void EthernetClient::InvokeCallback(BYTE* frame, DWORD frameSize) {
	
	if (this->callback_args == NULL) {
		this->callback_a(frame, frameSize);
	} else {
		this->callback_b(frame, frameSize, this->callback_args);
	}

}

static DWORD WINAPI HandlerThread(LPVOID clientPtr) {
	BOOL fConnected, retVal;
	EthernetClient* client;
	DWORD frameSize, nBytes;
	ETHNOTIFY data;
	std::unique_ptr<BYTE> buffer;

	client = (EthernetClient*)clientPtr;

	auto name_s = std::string("Global\\EVT:" + std::to_string(client->GetClientIndex()));
	HANDLE hEvent = CreateEventA(NULL, TRUE, FALSE, name_s.c_str());
	if (hEvent == NULL || hEvent == INVALID_HANDLE_VALUE) throw std::exception("An fatal error occurred during the creation of the Event");

	while (TRUE) {

		WaitForSingleObject(hEvent, INFINITE);
		ResetEvent(hEvent);
		
		retVal = false;
		fConnected = ConnectNamedPipe(client->GetPipe(), NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
		if (fConnected) {
			retVal = ReadFile(client->GetPipe(), &frameSize, sizeof frameSize, &nBytes, NULL);

			buffer = std::unique_ptr<BYTE>(new BYTE[frameSize]);
			retVal &= ReadFile(client->GetPipe(), buffer.get(), frameSize, &nBytes, NULL);
		}
		
		// We free the pipe before calling the Callback
		// Maybe it'll need it so let's free it!
		DisconnectNamedPipe(client->GetPipe());

		// If the flag is set, we can call the callback without problems (I hope! :) )
		if (retVal) {
			client->InvokeCallback(buffer.get(), frameSize);
		}

	}

	return -1;
}