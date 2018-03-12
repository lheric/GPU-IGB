//
// BaseCmdInterpreter.cpp
//

#include <crtdbg.h>
#include "..\Common\dbgnew.h"

#include "Profile.h"
#include <assert.h>
#include <omp.h>
#include <iostream>
#include "CmdInterpreter.h"

using namespace std;

#ifndef _WIN64
#error You must use the 64 bit compiler.
#endif


CBaseCmdInterpreter::CBaseCmdInterpreter()
{

}

CBaseCmdInterpreter::~CBaseCmdInterpreter()
{

}

// ==================================================================
//
// ==================================================================
void CBaseCmdInterpreter::Run(CSocket* pCS)
{
	if (pCS)
	{
		bool done = false;

		while (!done)
		{
			CMD_t incmd;
			char* payload = NULL;
			unsigned int payload_size = 0;

			pCS->ReceiveTCPMessage(&incmd, &payload, &payload_size);

			switch (incmd.opcode)
			{
			case OPC_NOOP:
				done = true;	// this can happen when the client shuts down
				break;

			case OPC_Malloc1D:
				DoMalloc1D(pCS, &incmd);
				break;

			case OPC_Malloc2D:
				DoMalloc2D(pCS, &incmd);
				break;

			case OPC_Free:
				DoFree(pCS, &incmd);
				break;


			case OPC_SHUTDOWN:
				DoShutDown(pCS);
				done = true;
				break;

			case OPC_COPYHost2Device:
				DoCopyHost2Device(pCS, &incmd, payload, payload_size);
				break;

			case OPC_COPYDevice2Host:
				DoCopyDevice2Host(pCS, &incmd);
				break;

			case OPC_FunctionAnova:
				DoFunctionAnova(pCS, &incmd);
				break;

			case OPC_FunctionVegasTest:
				DoFunctionVegasTest(pCS, &incmd);
				break;

			default: cout << "****Unknown opcode " << (int)incmd.opcode << " received by server" << endl;
				exit(EXIT_FAILURE);
			}

			if (payload)
				delete[] payload;
		}
	}

}

// ==================================================================
//
// ==================================================================
void CBaseCmdInterpreter::DoShutDown(CSocket *pCS)
{
	DTRACE(1 == 1, __FUNCTION__);

	CMD_t outcmd;

	outcmd.errorcode = 0;
	pCS->SendTCPMessage(&outcmd, NULL);
}

// ==================================================================
//
// ==================================================================
void CBaseCmdInterpreter::DoFunctionAnova(CSocket* pCS, CMD_t *pCmd)
{
	assert(pCmd->C.FunctionAnova.SNPSubject != NULL);
	assert(pCmd->C.FunctionAnova.VoxelSubject != NULL);
	assert(pCmd->C.FunctionAnova.VoxelSNP != NULL);

	assert(pCmd->C.FunctionAnova.VoxelListCount > 0);
	assert(pCmd->C.FunctionAnova.SNPListCount > 0);


	DPRINT2("\tNumber of SNPs     = ", pCmd->C.FunctionAnova.NumberOfSNPs);
	DPRINT2("\tNumber of Voxels   = ", pCmd->C.FunctionAnova.NumberOfVoxels);
	DPRINT2("\tNumber of Subjects = ", pCmd->C.FunctionAnova.NumberOfSubjects);
	DPRINT2("\tSNP List Count     = ", pCmd->C.FunctionAnova.SNPListCount);
	DPRINT2("\tVoxel List Count   = ", pCmd->C.FunctionAnova.VoxelListCount);
}

// ==================================================================
//
// ==================================================================
void CBaseCmdInterpreter::DoFunctionVegasTest(CSocket* pCS, CMD_t *pCmd)
{
	assert(pCmd->C.FunctionVegasTest.VoxelGeneObserved != NULL);
	assert(pCmd->C.FunctionVegasTest.VoxelGeneSim != NULL);
	assert(pCmd->C.FunctionVegasTest.VoxelSNP != NULL);
	assert(pCmd->C.FunctionVegasTest.SNPPosLenPairs != NULL);
	assert(pCmd->C.FunctionVegasTest.LDMatrixList != NULL);

//	assert(pCmd->C.FunctionVegasTest.VoxelListCount > 0);
//	assert(pCmd->C.FunctionVegasTest.SNPListCount > 0);

	DPRINT2("\tNumber of Voxels     = ", pCmd->C.FunctionVegasTest.NumberOfVoxels);
	DPRINT2("\tNumber of Genes      = ", pCmd->C.FunctionVegasTest.NumberOfGenes);
	DPRINT2("\tNumber of SNPs       = ", pCmd->C.FunctionVegasTest.NumberOfSNPs);
	DPRINT2("\tNumber of Iterations = ", pCmd->C.FunctionVegasTest.NumberOfIterations);
	DPRINT2("\tNumber of SNPPosLen  = ", pCmd->C.FunctionVegasTest.NumberOfSNPPositionLength);
}