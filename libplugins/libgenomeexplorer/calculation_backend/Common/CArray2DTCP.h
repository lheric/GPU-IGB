// ========================================================================================
//	CArray2D-TCP.h - template for the CArray2D-TCP
//
//// A 2D array class with 0 based indexing for each dimension.
// ========================================================================================
#pragma  once
#include <assert.h>
#include <iostream>
#include "..\Common\CArray2D.h"
#include "..\Common\Sockets.h"

using namespace std;

template <class T>
class CArray2D_TCP : public CArray2D<T>
{
public:
	CArray2D_TCP(CSocket* ClientSocket, unsigned int h, unsigned int w, bool initflag = true, T init = 0);
	CArray2D_TCP(CSocket* ClientSocket);
	virtual ~CArray2D_TCP();

	void Create(unsigned int h, unsigned int w, bool initflag = true, T init = 0);
//	void Create(CSocket* ClientSocket, unsigned int h, unsigned int w, bool initflag = true, T init = 0);
	T* GetServerDataPtr(void) const { return m_pServerData; }

	void CopyToServer(void);
	void CopyFromServer(void);

private:
	CArray2D_TCP();
	CSocket *m_CliSock;
	T*  m_pServerData;
};

// ========================================================================================
// Constructor
// ========================================================================================

template <class T>
CArray2D_TCP<T>::CArray2D_TCP(CSocket* ClientSocket, unsigned int h, unsigned int w, bool initflag, T init) :
	CArray2D(h, w, initflag, init),
	m_CliSock(ClientSocket)
{
	//
	// Allocate the RAM on the server
	{
		CMD_t Cmd;

		Cmd.opcode = OPC_Malloc2D;
		Cmd.C.GMalloc2D.size = SizeOfMemory();

		int iResult = m_CliSock->SendTCPMessage(&Cmd, NULL);
		if (iResult != 0)
			throw("CArray2D_TCP<T>::ctor -- send cudaMalloc error.");

		CMD_t RetCmd;
		char *payload = NULL;
		unsigned int payload_size = 0;

		iResult = m_CliSock->ReceiveTCPMessage(&RetCmd, &payload, &payload_size);
		if (iResult != 0)
			throw("CArray2D_TCP<T>::ctor -- receive cudaMalloc error.");

		m_pServerData = (T*)RetCmd.pRetVal;		// save the pointer to the server ram
	}

	//
	// Now finish copying the initial state of the HOST ram to the server ram
	//
	if (initflag)
	{
		CMD_t Cmd;

		Cmd.opcode = OPC_COPYHost2Device;
		Cmd.payload_len = SizeOfMemory();
		Cmd.C.GCopyHost2Device.pDevice = m_pServerData;
		Cmd.C.GCopyHost2Device.size = SizeOfMemory();

		int iResult = m_CliSock->SendTCPMessage(&Cmd, (char*)m_pHostData);
		if (iResult != 0)
			throw("CArray2D_TCP<T>::ctor -- send COPYHost2Device error.");

		CMD_t RetCmd;
		char *payload = NULL;
		unsigned int payload_size = 0;

		iResult = m_CliSock->ReceiveTCPMessage(&RetCmd, &payload, &payload_size);
		if (iResult != 0)
			throw("CArray2D_TCP<T>::ctor -- receive COPYHost2Device error.");
	}
}

template <class T>
CArray2D_TCP<T>::CArray2D_TCP(CSocket* ClientSocket) :
	m_pServerData(0),
	m_CliSock(ClientSocket)
{
	// If the object is constructed this way, then the program
	// should be calling Create some time later.
}

//template <class T>
//CArray2D_TCP<T>::CArray2D_TCP() :
//	m_pServerData(0),
//	m_CliSock(NULL)
//{
//	// If the object is constructed this way, then the program
//	// should be calling Create some time later.
//}

// ========================================================================================
// Destructor
// ========================================================================================
template <class T>
CArray2D_TCP<T>::~CArray2D_TCP()
{
	if (m_pServerData)
	{
		CMD_t Cmd;

		Cmd.opcode = OPC_Free;
		Cmd.C.GFree.p = m_pServerData;

		int iResult = m_CliSock->SendTCPMessage(&Cmd, NULL);
		if (iResult != 0)
			cout << "CArray2D_TCP<T>::dtor -- send cudaFree error.";

		CMD_t RetCmd;
		char *payload = NULL;
		unsigned int payload_size = 0;

		iResult = m_CliSock->ReceiveTCPMessage(&RetCmd, &payload, &payload_size);
		if (iResult != 0)
			cout << "CArray2D_TCP<T>::dtor -- receive cudaFree error.";
	}
}

// ========================================================================================
// Create method
// ========================================================================================
template <class T>
void CArray2D_TCP<T>::Create(unsigned int y, unsigned int x, bool initflag, T init)
{
	CArray2D::Create(y, x, initflag, init);

	if (m_pServerData)
	{
		CMD_t Cmd;

		Cmd.opcode = OPC_Free;
		Cmd.C.GFree.p = m_pServerData;

		int iResult = m_CliSock->SendTCPMessage(&Cmd, NULL);
		if (iResult != 0)
			cout << "CArray2D_TCP<T>::dtor -- send cudaFree error.";

		CMD_t RetCmd;
		char *payload = NULL;
		unsigned int payload_size = 0;

		iResult = m_CliSock->ReceiveTCPMessage(&RetCmd, &payload, &payload_size);
		if (iResult != 0)
			cout << "CArray2D_TCP<T>::dtor -- receive cudaFree error.";
	}

	//
	// Allocate the RAM on the server
	{
		CMD_t Cmd;

		Cmd.opcode = OPC_Malloc2D;
		Cmd.C.GMalloc2D.size = SizeOfMemory();

		int iResult = m_CliSock->SendTCPMessage(&Cmd, NULL);
		if (iResult != 0)
			throw("CArray2D_TCP<T>::ctor -- send cudaMalloc error.");

		CMD_t RetCmd;
		char *payload = NULL;
		unsigned int payload_size = 0;

		iResult = m_CliSock->ReceiveTCPMessage(&RetCmd, &payload, &payload_size);
		if (iResult != 0)
			throw("CArray2D_TCP<T>::ctor -- receive cudaMalloc error.");

		m_pServerData = (T*)RetCmd.pRetVal;		// save the pointer to the server ram
	}

	if (initflag)
		CopyToServer();
}

//template <class T>
//void CArray2D_TCP<T>::Create(CSocket* ClientSocket, unsigned int y, unsigned int x, bool initflag, T init) 
//{
//	m_pServerData = 0;
//	m_CliSock = ClientSocket;
//
//	CArray2D::Create(y, x, initflag, init);
//
//	if (m_pServerData)
//	{
//		CMD_t Cmd;
//
//		Cmd.opcode = OPC_Free;
//		Cmd.C.GFree.p = m_pServerData;
//
//		int iResult = m_CliSock->SendTCPMessage(&Cmd, NULL);
//		if (iResult != 0)
//			cout << "CArray2D_TCP<T>::dtor -- send cudaFree error.";
//
//		CMD_t RetCmd;
//		char *payload = NULL;
//		unsigned int payload_size = 0;
//
//		iResult = m_CliSock->ReceiveTCPMessage(&RetCmd, &payload, &payload_size);
//		if (iResult != 0)
//			cout << "CArray2D_TCP<T>::dtor -- receive cudaFree error.";
//	}
//
//	//
//	// Allocate the RAM on the server
//	{
//		CMD_t Cmd;
//
//		Cmd.opcode = OPC_Malloc2D;
//		Cmd.C.GMalloc2D.size = SizeOfMemory();
//
//		int iResult = m_CliSock->SendTCPMessage(&Cmd, NULL);
//		if (iResult != 0)
//			throw("CArray2D_TCP<T>::ctor -- send cudaMalloc error.");
//
//		CMD_t RetCmd;
//		char *payload = NULL;
//		unsigned int payload_size = 0;
//
//		iResult = m_CliSock->ReceiveTCPMessage(&RetCmd, &payload, &payload_size);
//		if (iResult != 0)
//			throw("CArray2D_TCP<T>::ctor -- receive cudaMalloc error.");
//
//		m_pServerData = (T*)RetCmd.pRetVal;		// save the pointer to the server ram
//	}
//
//	if (initflag)
//		CopyToServer();
//}

template <class T>
void CArray2D_TCP<T>::CopyToServer(void)
{
	CMD_t Cmd;

	Cmd.opcode = OPC_COPYHost2Device;
	Cmd.payload_len = SizeOfMemory();
	Cmd.C.GCopyHost2Device.pDevice = m_pServerData;
	Cmd.C.GCopyHost2Device.size = SizeOfMemory();

	int iResult = m_CliSock->SendTCPMessage(&Cmd, (char*)m_pHostData);
	if (iResult != 0)
		throw("CArray2D_TCP<T>::CopyToServer -- send COPYHost2Device error.");

	CMD_t RetCmd;
	char *payload = NULL;
	unsigned int payload_size = 0;

	iResult = m_CliSock->ReceiveTCPMessage(&RetCmd, &payload, &payload_size);
	if (iResult != 0)
		throw("CArray2D_TCP<T>::CopyToServer -- receive COPYHost2Device error.");
}

template <class T>
void CArray2D_TCP<T>::CopyFromServer(void)
{
	CMD_t Cmd;

	Cmd.opcode = OPC_COPYDevice2Host;
	Cmd.C.GCopyDevice2Host.pDevice = m_pServerData;
	Cmd.C.GCopyDevice2Host.size = SizeOfMemory();

	int iResult = m_CliSock->SendTCPMessage(&Cmd, NULL);
	if (iResult != 0)
		throw("CArray2D_TCP<T>::CopyFromServer -- send COPYDevice2Host error.");

	CMD_t RetCmd;
	char *payload = (char*)m_pHostData;
	unsigned int payload_size = SizeOfMemory();

	iResult = m_CliSock->ReceiveTCPMessage(&RetCmd, &payload, &payload_size);
	if (iResult != 0)
		throw("CArray2D_TCP<T>::CopyFromServer -- receive COPYDevice2Host error.");
}



