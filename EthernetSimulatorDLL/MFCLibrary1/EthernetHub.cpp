#include "pch.h"
#include "EthernetHub.h"
#include "EthernetCommon.h"
#include <string>
#include <exception>
#include <memory>

static DWORD WINAPI HandlerThread(LPVOID param);

EthernetHub::EthernetHub(std::string hubName) {
	this->clientsCount = 0;
	this->hubName = hubName;
	this->PipeName = std::string("\\\\.\\pipe\\" + hubName);

	this->hPipe = CreateNamedPipeA(
		this->PipeName.c_str(), 
		PIPE_ACCESS_DUPLEX | FILE_FLAG_FIRST_PIPE_INSTANCE | FILE_FLAG_WRITE_THROUGH,
		PIPE_TYPE_BYTE | PIPE_WAIT,
		0x1, 0x0, 0x0, 0x0, NULL);
}

EthernetHub::~EthernetHub() {
	TerminateThread(this->hThread, 0x0);
	CloseHandle(this->hPipe);
}

BOOL EthernetHub::CreationSucceded() {
	return !(this->hPipe == NULL || this->hPipe == INVALID_HANDLE_VALUE);
}

BOOL EthernetHub::FailedBcsAlreadyExists() {
	return GetLastError() == ERROR_ACCESS_DENIED;
}

void EthernetHub::StartHandling() {
	DWORD mainThreadId;

	this->hThread = CreateThread(NULL, 0, HandlerThread, this, 0, &mainThreadId);
	if (this->hThread == NULL) throw std::exception("An error occurred while creating the handler thread");
}

HANDLE EthernetHub::GetPipe() {
	return this->hPipe;
}

std::string EthernetHub::GetHubName() {
	return this->hubName;
}

std::string EthernetHub::GetPipeName() {
	return this->PipeName;
}

DWORD EthernetHub::GetClientsCount() {
	return this->clientsCount;
}

std::map<DWORD, std::string>& EthernetHub::GetClients() {
	return this->clientHandle;
}

void EthernetHub::UpdateClientsCount(DWORD offset) {
	this->clientsCount += offset;
}

static DWORD WINAPI HandlerThread(LPVOID hubPtr) {
	BOOL fConnected, retVal;
	EthernetHub* hub;
	DWORD count, nBytes;
	ETHNOTIFY data;

	hub = (EthernetHub*)hubPtr;
	while (1) {

		fConnected = ConnectNamedPipe(hub->GetPipe(), NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);

		if (fConnected) {

			retVal = ReadFile(hub->GetPipe(), &data, sizeof data, &nBytes, NULL);
			if (!retVal) continue;

			if (data == NEWCLIENT) {
				count = hub->GetClientsCount();
				WriteFile(hub->GetPipe(), &count, sizeof count, &nBytes, NULL);

				// Waiting for ACK
				retVal = ReadFile(hub->GetPipe(), &data, sizeof data, &nBytes, NULL);
				if (retVal && data == ACK_NEWCLIENT) {
					size_t len;
					retVal &= ReadFile(hub->GetPipe(), &len, sizeof len, &nBytes, NULL);

					std::unique_ptr<char> buffer(new char[len + 1]);
					buffer.get()[len] = '\0';
					retVal &= ReadFile(hub->GetPipe(), buffer.get(), len, &nBytes, NULL);

					/*HANDLE hPipe = CreateFileA(
						buffer.get(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
						OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

					if (!retVal || hPipe == INVALID_HANDLE_VALUE) break;*/

					hub->GetClients().insert(std::pair<DWORD, std::string>(count, std::string(buffer.get())));
					hub->UpdateClientsCount();
				}
			} else if (data == NEWDATA) {
				DWORD clientIndex;
				retVal = ReadFile(hub->GetPipe(), &clientIndex, sizeof clientIndex, &nBytes, NULL);
				
				if (retVal) {
					DWORD frameSize = 0;
					HANDLE clientPipeH;
					std::unique_ptr<BYTE> buffer;

					const char* clientPipe = hub->GetClients().at(clientIndex).c_str();

					// First we need to wait for the client to start its server (the client is the server for its own pipe)
					WaitNamedPipeA(clientPipe, NMPWAIT_WAIT_FOREVER);
					
					// Reading the frame the clients wants to send to us
					clientPipeH = CreateFileA(
						clientPipe, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
						OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

					retVal = clientPipeH != NULL && clientPipeH != INVALID_HANDLE_VALUE;
					if (retVal) {
						retVal &= ReadFile(clientPipeH, &frameSize, sizeof frameSize, &nBytes, NULL);
						retVal &= (nBytes == sizeof frameSize);

						buffer = std::unique_ptr<BYTE>(new BYTE[frameSize]);
						retVal &= ReadFile(clientPipeH, buffer.get(), frameSize, &nBytes, NULL);
						//CloseHandle(clientPipeH);
					}

					// Data received: now ready to forward to others clients
					if (retVal) {
						for (auto kv : hub->GetClients()) {
							if (kv.first == clientIndex) continue;

							// Waking up the client
							HANDLE hEvent = OpenEventA(EVENT_ALL_ACCESS, TRUE, std::string("Global\\EVT:" + std::to_string(kv.first)).c_str());
							if (hEvent == NULL || hEvent == INVALID_HANDLE_VALUE) continue;
							
							SetEvent(hEvent);
							clientPipe = kv.second.c_str();
							WaitNamedPipeA(clientPipe, NMPWAIT_WAIT_FOREVER);
							
							clientPipeH = CreateFileA(
								clientPipe, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
								OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

							if (clientPipeH == INVALID_HANDLE_VALUE) continue;

							WriteFile(clientPipeH, (LPCVOID)&frameSize, sizeof frameSize, &nBytes, NULL);
							WriteFile(clientPipeH, buffer.get(), frameSize, &nBytes, NULL);
							
							//CloseHandle(clientPipeH);
						}
					}
				}
			} else if (data == DELCLIENT) {
				DWORD clientIndex;
				
				retVal = ReadFile(hub->GetPipe(), &clientIndex, sizeof clientIndex, &nBytes, NULL);
				if (retVal) {
					hub->GetClients().erase(clientIndex);
					hub->UpdateClientsCount(-1);
				}
			}

			DisconnectNamedPipe(hub->GetPipe());
			Sleep(500);
		}
	}

	return -1;
}

/*DWORD WINAPI PoolerThread(LPVOID arg) {

}*/