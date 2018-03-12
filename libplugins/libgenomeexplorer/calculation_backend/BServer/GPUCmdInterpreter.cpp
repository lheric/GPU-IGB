//
// GPUCmdInterpreter.cpp
//
#include <crtdbg.h>
#include "..\Common\dbgnew.h"

#include "Profile.h"
#include <assert.h>
#include <iostream>
#include <cuda_runtime.h>
#include "ServerKernels.cuh"
#include "CmdInterpreter.h"

using namespace std;

#ifndef _WIN64
#error You must use the 64 bit compiler.
#endif


CGPUCmdInterpreter::CGPUCmdInterpreter() :
	m_XferBuf(0)
{

}

CGPUCmdInterpreter::~CGPUCmdInterpreter()
{
	delete[] m_XferBuf;
}

// ==================================================================
//
// ==================================================================
void CGPUCmdInterpreter::Init(void)
{
	cout << "Basic GPU Server is running ..." << endl;

	cudaDeviceProp prop;
	const int DeviceNumber = 0;

	cudaSetDevice(DeviceNumber);
	cudaGetDeviceProperties(&prop, DeviceNumber);

	cout << "\n** " << prop.name << " present.";
	cout << "\n\tMajor = " << prop.major << ", Minor = " << prop.minor << endl;

	if (strcmp(prop.name, "Tesla K40c") != 0)
	{

	}

	//
	// Allocate this 2GB buffer once. It is used repeatedly by CopyDevice2Host.
	//
	m_XferBuf = new char[2147483647];
}

// ==================================================================
//
// ==================================================================
void CGPUCmdInterpreter::DoMalloc1D(CSocket* pCS, CMD_t *pCmd)
{
	void *pGPU;

	DLABEL(__FUNCTION__);
	DPRINT2("\tsize request (bytes)  = ", pCmd->C.GMalloc1D.size);

	cudaError_t err = cudaMalloc((void**)&pGPU, pCmd->C.GMalloc1D.size);

	DTRACE(err == cudaSuccess, "cudaMalloc in " __FUNCTION__);

	CMD_t outcmd;

	outcmd.errorcode = err;
	outcmd.pRetVal = pGPU;

	pCS->SendTCPMessage(&outcmd, NULL);
}

// ==================================================================
//
// ==================================================================
void CGPUCmdInterpreter::DoMalloc2D(CSocket* pCS, CMD_t *pCmd)
{
	void *pGPU;

	DLABEL(__FUNCTION__);
	DPRINT2("\tsize request (bytes)  = ", pCmd->C.GMalloc2D.size);

	cudaError_t err = cudaMalloc((void**)&pGPU, pCmd->C.GMalloc2D.size);

	DTRACE(err == cudaSuccess, "cudaMalloc in " __FUNCTION__);

	CMD_t outcmd;

	outcmd.errorcode = err;
	outcmd.pRetVal = pGPU;

	pCS->SendTCPMessage(&outcmd, NULL);
}

// ==================================================================
//
// ==================================================================
void CGPUCmdInterpreter::DoFree(CSocket* pCS, CMD_t *pCmd)
{
	DLABEL(__FUNCTION__);
	DPRINT2("\tFreeing Pointer  = ", pCmd->C.GFree.p);

	cudaError_t err = cudaFree(pCmd->C.GFree.p);

	DTRACE(err == cudaSuccess, "cudaFree in " __FUNCTION__);

	CMD_t outcmd;
	outcmd.errorcode = err;
	pCS->SendTCPMessage(&outcmd, NULL);
}

// ==================================================================
//
// ==================================================================
void CGPUCmdInterpreter::DoCopyHost2Device(CSocket* pCS, CMD_t *pCmd, char* payload, int payload_size)
{
	assert(payload_size == pCmd->C.GCopyHost2Device.size);
	DLABEL(__FUNCTION__);
	DPRINT2("\tsize request (bytes)  = ", pCmd->C.GCopyHost2Device.size);

	cudaError_t err = cudaMemcpy(pCmd->C.GCopyHost2Device.pDevice, payload, pCmd->C.GCopyHost2Device.size, cudaMemcpyHostToDevice);

	DTRACE(err == cudaSuccess, "cudaMemcpy in " __FUNCTION__);

	CMD_t outcmd;
	outcmd.errorcode = err;
	pCS->SendTCPMessage(&outcmd, NULL);
}

// ==================================================================
//
// ==================================================================
void CGPUCmdInterpreter::DoCopyDevice2Host(CSocket* pCS, CMD_t *pCmd)
{
	DLABEL(__FUNCTION__);
	DPRINT2("\tsize request (bytes)  = ", pCmd->C.GCopyDevice2Host.size);

	cudaError_t err = cudaMemcpy(m_XferBuf, pCmd->C.GCopyDevice2Host.pDevice, pCmd->C.GCopyDevice2Host.size, cudaMemcpyDeviceToHost);

	DTRACE(err == cudaSuccess, "cudaMemcpy in " __FUNCTION__);

	CMD_t outcmd;
	outcmd.errorcode = err;
	outcmd.payload_len = (unsigned int)pCmd->C.GCopyDevice2Host.size;

	pCS->SendTCPMessage(&outcmd, m_XferBuf);
}

// ==================================================================
//
// ==================================================================
void CGPUCmdInterpreter::DoFunctionAnova(CSocket* pCS, CMD_t *pCmd)
{
	DLABEL(__FUNCTION__);

	CBaseCmdInterpreter::DoFunctionAnova(pCS, pCmd);

	cudaError_t err = DoKernelAnova(
		pCmd->C.FunctionAnova.VoxelSubject,
		pCmd->C.FunctionAnova.SNPSubject,
		pCmd->C.FunctionAnova.VoxelSNP,

		pCmd->C.FunctionAnova.NumberOfSNPs,
		pCmd->C.FunctionAnova.NumberOfSubjects,
		pCmd->C.FunctionAnova.NumberOfVoxels,

		pCmd->C.FunctionAnova.VoxelList,
		pCmd->C.FunctionAnova.VoxelListCount,
		pCmd->C.FunctionAnova.SNPList,
		pCmd->C.FunctionAnova.SNPListCount);

	DTRACE(err == 0, __FUNCTION__);
	CMD_t outcmd;
	outcmd.errorcode = err;
	pCS->SendTCPMessage(&outcmd, NULL);

}

// ==================================================================
//
// ==================================================================
void CGPUCmdInterpreter::DoFunctionVegasTest(CSocket* pCS, CMD_t *pCmd)
{
	DLABEL(__FUNCTION__);

	CBaseCmdInterpreter::DoFunctionVegasTest(pCS, pCmd);

	cudaError_t err = DoKernelVegasTest(
		pCmd->C.FunctionVegasTest.VoxelGeneObserved,
		pCmd->C.FunctionVegasTest.VoxelGeneSim,
		pCmd->C.FunctionVegasTest.VoxelSNP,
		pCmd->C.FunctionVegasTest.SNPPosLenPairs,
		pCmd->C.FunctionVegasTest.LDMatrixList,
		pCmd->C.FunctionVegasTest.NumberOfVoxels,
		pCmd->C.FunctionVegasTest.NumberOfGenes,
		pCmd->C.FunctionVegasTest.NumberOfSNPs,
		pCmd->C.FunctionVegasTest.NumberOfIterations,
		pCmd->C.FunctionVegasTest.NumberOfSNPPositionLength
	);

	DTRACE(err == 0, __FUNCTION__);
	CMD_t outcmd;
	outcmd.errorcode = err;
	pCS->SendTCPMessage(&outcmd, NULL);
}



