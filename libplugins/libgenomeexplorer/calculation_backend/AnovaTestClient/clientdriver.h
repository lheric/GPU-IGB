#ifndef CLIENTDRIVER_H
#define CLIENTDRIVER_H

//
// Enable this symbol if you want the indexes
// checked when indexing into the arrays (CArray1DCPU.h).
//
#define DEBUG_INDEXING 1


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <chrono>
#include "calculation_backend/Common/Timer.h"
#include "calculation_backend/Common/Sockets.h"
#include "calculation_backend/Common/CArray1D.h"
#include "calculation_backend/Common/CArray1DTCP.h"
#include "calculation_backend/Common/CArray2D.h"
#include "calculation_backend/Common/CArray2DTCP.h"



#define DEFAULT_PORT "98452"

void DumpMatrix(CArray2D_TCP<float> &M, const char *filename, const char *extension, int seqnumber, bool writetext = true);

void DumpMatrix(CArray2D_TCP<int> &M, char *filename, char *extension, int seqnumber, bool writetext = true);

bool DoFunctionANova(float *pVoxelSubject,
    int *pSNPSubject,
    float *pVoxelSNP,
    int NumberOfVoxels,
    int NumberOfSubjects,
    int NumberOfSNPs,
    int *pVoxelList,
    int VoxelListCount,
    int *pSNPList,
    int SNPListCount,
    CSocket* CliSock);
float FRand(void);
bool AnovaTestOne(const int NumberOfSubjects, int NumberOfVoxels, int NumberOfSNPs, CSocket* ClientSocket);
bool AnovaTestTwo(const int NumberOfSubjects, int NumberOfVoxels, int NumberOfSNPs, CSocket* ClientSocket, CArray2D_TCP<float>* VoxelSNP);

#endif // CLIENTDRIVER_H
