#pragma once

#include <string>
#include <vector>
#include <map>

class EthernetHub {
	
private:
	std::string hubName;
	std::string PipeName;
	HANDLE hPipe;
	HANDLE hThread;
	DWORD clientsCount;
	std::map<DWORD, std::string> clientHandle;

public:
	EthernetHub(std::string hubName);
	~EthernetHub();

	BOOL CreationSucceded();
	BOOL FailedBcsAlreadyExists();
	void StartHandling();
	HANDLE GetPipe();
	std::string GetHubName();
	std::string GetPipeName();
	std::map<DWORD, std::string>& GetClients();
	DWORD GetClientsCount();
	void UpdateClientsCount(DWORD offset = 1);

};

