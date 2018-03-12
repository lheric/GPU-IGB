// ========================================================================================
//	CArray1D_TCP.h - template for the CArray1D_TCP
//
// A 1D array class with 0 based indexing for each dimension.
// ========================================================================================
#pragma  once
#include <assert.h>
#include "..\Common\CArray1D.h"
#include "..\Common\Sockets.h"

template <class T>
class CArray1D_TCP : public CArray1D<T>
{
public:
	CArray1D_TCP(CSocket* ClientSocket, int x, bool initflag = true, T init = 0);
	CArray1D_TCP(CSocket* ClientSocket);
	virtual ~CArray1D_TCP();

	void Create(int x, bool initflag = true, T init = 0);
	void Initialize(T init);
	T* GetServerDataPtr(void) const { return m_pServerData; }

	void CopyToServer(void);
	void CopyFromServer(void);

private:
	CArray1D_TCP();
	CSocket *m_CliSock;
	T*  m_pServerData;
};

// ========================================================================================
// Constructor
// ========================================================================================
template <class T>
CArray1D_TCP<T>::CArray1D_TCP(CSocket* ClientSocket, int x, bool initflag, T init) :
	CArray1D(x, initflag, init),
	m_CliSock(ClientSocket)
{
	//
	// Allocate the RAM on the server
	{
		CMD_t Cmd;

		Cmd.opcode = OPC_Malloc1D;
		Cmd.C.GMalloc1D.size = SizeOfMemory();

		int iResult = m_CliSock->SendTCPMessage(&Cmd, NULL);
		if (iResult != 0)
			throw("CArray1D_TCP<T>::ctor -- send malloc error.");

		CMD_t RetCmd;
		char *payload = NULL;
		unsigned int payload_size = 0;

		iResult = m_CliSock->ReceiveTCPMessage(&RetCmd, &payload, &payload_size);
		if (iResult != 0)
			throw("CArray1D_TCP<T>::ctor -- receive malloc error.");

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
			throw("CArray1D_TCP<T>::ctor -- send COPYHost2Device error.");

		CMD_t RetCmd;
		char *payload = NULL;
		unsigned int payload_size = 0;

		iResult = m_CliSock->ReceiveTCPMessage(&RetCmd, &payload, &payload_size);
		if (iResult != 0)
			throw("CArray1D_TCP<T>::ctor -- receive COPYHost2Device error.");
	}

}

template <class T>
CArray1D_TCP<T>::CArray1D_TCP(CSocket* ClientSocket) :
	m_pServerData(0),
	m_CliSock(ClientSocket)
{
	// If the object is constructed this way, then the program
	// should be calling Create some time later.
}

// ========================================================================================
// Destructor
// ========================================================================================
template <class T>
CArray1D_TCP<T>::~CArray1D_TCP()
{
	if (m_pServerData)
	{
		CMD_t Cmd;

		Cmd.opcode = OPC_Free;
		Cmd.C.GFree.p = m_pServerData;

		int iResult = m_CliSock->SendTCPMessage(&Cmd, NULL);
		if (iResult != 0)
			cout << "CArray1D_TCP<T>::dtor -- send cudaFree error.";

		CMD_t RetCmd;
		char *payload = NULL;
		unsigned int payload_size = 0;

		iResult = m_CliSock->ReceiveTCPMessage(&RetCmd, &payload, &payload_size);
		if (iResult != 0)
			cout << "CArray1D_TCP<T>::dtor -- receive cudaFree error.";
	}
}

// ========================================================================================
// Create method
// ========================================================================================
template <class T>
void CArray1D_TCP<T>::Create(int x, bool initflag, T init)
{
	CArray1D::Create(x, initflag, init);

	if (m_pServerData)
	{
		CMD_t Cmd;

		Cmd.opcode = OPC_Free;
		Cmd.C.GFree.p = m_pServerData;

		int iResult = m_CliSock->SendTCPMessage(&Cmd, NULL);
		if (iResult != 0)
			cout << "CArray1D_TCP<T>::dtor -- send free error.";

		CMD_t RetCmd;
		char *payload = NULL;
		unsigned int payload_size = 0;

		iResult = m_CliSock->ReceiveTCPMessage(&RetCmd, &payload, &payload_size);
		if (iResult != 0)
			cout << "CArray1D_TCP<T>::dtor -- receive free error.";
	}

	//
	// Allocate the RAM on the server
	//
	if (initflag)
	{
		CMD_t Cmd;

		Cmd.opcode = OPC_Malloc1D;
		Cmd.C.GMalloc1D.size = SizeOfMemory();

		int iResult = m_CliSock->SendTCPMessage(&Cmd, NULL);
		if (iResult != 0)
			throw("CArray1D_TCP<T>::ctor -- send malloc error.");

		CMD_t RetCmd;
		char *payload = NULL;
		unsigned int payload_size = 0;

		iResult = m_CliSock->ReceiveTCPMessage(&RetCmd, &payload, &payload_size);
		if (iResult != 0)
			throw("CArray1D_TCP<T>::ctor -- receive malloc error.");

		m_pServerData = (T*)RetCmd.pRetVal;		// save the pointer to the server ram
	}

	CopyToServer();
}

template <class T>
void CArray1D_TCP<T>::Initialize(T init)
{
	CArray1D::Initialize(init);

	CopyToServer();
}

template <class T>
void CArray1D_TCP<T>::CopyToServer(void)
{
	CMD_t Cmd;

	Cmd.opcode = OPC_COPYHost2Device;
	Cmd.payload_len = SizeOfMemory();
	Cmd.C.GCopyHost2Device.pDevice = m_pServerData;
	Cmd.C.GCopyHost2Device.size = SizeOfMemory();

	int iResult = m_CliSock->SendTCPMessage(&Cmd, (char*)m_pHostData);
	if (iResult != 0)
		throw("CArray1D_TCP<T>::CopyToServer -- send CopyToServer error.");

	CMD_t RetCmd;
	char *payload = NULL;
	unsigned int payload_size = 0;

	iResult = m_CliSock->ReceiveTCPMessage(&RetCmd, &payload, &payload_size);
	if (iResult != 0)
		throw("CArray1D_TCP<T>::CopyToServer -- receive CopyToServer error.");
}

template <class T>
void CArray1D_TCP<T>::CopyFromServer(void)
{
	CMD_t Cmd;

	Cmd.opcode = OPC_COPYDevice2Host;
	Cmd.C.GCopyDevice2Host.pDevice = m_pServerData;
	Cmd.C.GCopyDevice2Host.size = SizeOfMemory();

	int iResult = m_CliSock->SendTCPMessage(&Cmd, NULL);
	if (iResult != 0)
		throw("CArray1D_TCP<T>::CopyFromServer -- send COPYDevice2Host error.");

	CMD_t RetCmd;
	char *payload = (char*)m_pCPUData;
	unsigned int payload_size = SizeOfMemory();

	iResult = m_CliSock->ReceiveTCPMessage(&RetCmd, &payload, &payload_size);
	if (iResult != 0)
		throw("CArray1D_TCP<T>::CopyFromServer -- receive COPYDevice2Host error.");
}



