//
// BServer.cpp 
//

#pragma comment (lib, "Ws2_32.lib")

#include "Profile.h"
#include <iostream>
#include "CmdInterpreter.h"

using namespace std;

// ==================================================================
//
// ==================================================================
#define DEFAULT_PORT "98452"

CSocket* Init()
{
	int iResult = CSocket::Initialize();
	if (iResult)
	{
		cout << "\nError during CSocket::Initialize";
		exit(EXIT_FAILURE);
	}

	CSocket* Server = new CSocket();
	if (Server->StartTCPServer(DEFAULT_PORT))
	{
		delete Server;
		exit(EXIT_FAILURE);
	}

	return Server;
}

// ==================================================================
//
// ==================================================================

int main()
{
#define GPUCMD

#ifdef GPUCMD
	CGPUCmdInterpreter CmdInterpreter;
#else
	CCPUCmdInterpreter CmdInterpreter;
#endif

	CmdInterpreter.Init();

	CSocket* Server = Init();
	CSocket* pCS = Server->AcceptTCPConnection();

	CmdInterpreter.Run(pCS);

	delete pCS;
	delete Server;
	exit(EXIT_SUCCESS);
}

