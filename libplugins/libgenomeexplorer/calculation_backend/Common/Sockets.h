//
// Sockets.h
//

#pragma once
#include <winsock2.h>
#include <stdint.h>

// =======================================================================
// Individual command structures
// =======================================================================
typedef struct CMDMalloc1D
{
	size_t size;
} CMDMalloc1D_t;

typedef struct CMDMalloc2D
{
	//	size_t width;	// in bytes
	//	size_t height;
	size_t size;
} CMDMalloc2D_t;

typedef struct CMDFree
{
	void *p;
} CMDFree_t;

typedef struct CMDCopyHost2Device
{
	void *pDevice;
	size_t size;
} CMDCopyHost2Device_t;

typedef struct CMDCopyDevice2Host
{
	void *pDevice;
	size_t size;
} CMDCopyDevice2Host_t;

// =======================================================================
// User defined kernel functions
// =======================================================================

typedef struct CMDFunctionAnova
{
	// INPUT PARAMETERS
	float *VoxelSubject;	// ROWS = NumberOfVoxels, COLS = NumberOfSubjects, RowMajor format
	int *SNPSubject;		// ROWS = NumberOfSNPs,   COLS = NumberOfSubjects, RowMajor format
	float *VoxelSNP;		// ROWS = NumberOfVoxels, COLS = NumberOfSNPs, RowMajor format

	int NumberOfSubjects;
	int NumberOfVoxels;
	int NumberOfSNPs;

	int *VoxelList;			// pointer to array of integers describing which voxels to process
	int VoxelListCount;		// number of integers in the VoxelList

	int *SNPList;			// pointer to array of integers describing which SNPs to process
	int SNPListCount;		// number of integers in the SNPList
} CMDFunctionAnova_t;

typedef struct CMDFunctionVegasTest
{
	float *VoxelGeneObserved;	
	float *VoxelGeneSim;			// Output parameter
	float *VoxelSNP;
	int *SNPPosLenPairs;
	uint64_t *LDMatrixList;
	int NumberOfVoxels;
	int NumberOfGenes;
	int NumberOfSNPs;
	int NumberOfIterations;
	int NumberOfSNPPositionLength;
} CMDFunctionVegasTest_t;

// =======================================================================
// CMD -- the union of all command structures
// =======================================================================

typedef struct CMD
{
	unsigned char opcode;
	unsigned int  payload_len;

	union
	{
		CMDMalloc1D_t			GMalloc1D;
		CMDMalloc2D_t			GMalloc2D;
		CMDFree_t				GFree;
		CMDCopyHost2Device_t	GCopyHost2Device;
		CMDCopyDevice2Host_t	GCopyDevice2Host;
		CMDFunctionAnova_t		FunctionAnova;
		CMDFunctionVegasTest_t	FunctionVegasTest;
	} C;

	int errorcode;
	void *pRetVal;

	CMD() { memset(this, 0, sizeof(*this)); };

} CMD_t;


const unsigned char OPC_NOOP = 0;
const unsigned char OPC_Malloc1D = 1;
const unsigned char OPC_Free = 2;
const unsigned char OPC_SHUTDOWN = 3;
const unsigned char OPC_COPYHost2Device = 4;
const unsigned char OPC_COPYDevice2Host = 5;
const unsigned char OPC_Malloc2D = 6;

const unsigned char OPC_FunctionAnova = 64 + 1;
const unsigned char OPC_FunctionVegasTest = 64 + 2;


// =======================================================================
// CSocket
// =======================================================================

class CSocket
{
public:
	CSocket();
	CSocket(SOCKET);
	~CSocket();

	CSocket* AcceptTCPConnection();
	int StartTCPServer(char* port);

    int ConnectToTCPServer(const char* name, const char* port);
	int SendTCPMessage(CMD_t *cmd, char* payload);
	int ReceiveTCPMessage(CMD_t *cmd, char** payload, unsigned int *payload_size);

	static int Initialize(void);

private:
	SOCKET m_ConnectSocket;
	SOCKET m_ListenSocket;

};


