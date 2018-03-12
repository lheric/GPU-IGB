//
// ServerKernels.cuh
//
#pragma once

cudaError_t DoKernelAnova(float *VoxelSubject,
	int *SNPSubject,
	float *VoxelSNP,
	int NumberOfSNPs,
	int NumberOfSubjects,
	int NumberOfVoxels,
	int *VoxelList,
	int VoxelListCount,
	int *SNPList,
	int SNPListCount);


cudaError_t DoKernelVegasTest(float *VoxelGeneObserved,
	float *VoxelGeneSim,
	float *VoxelSNP,
	int *SNPPosLenPairs,
	uint64_t *LDMatrixList,
	int NumberOfVoxels,
	int NumberOfGenes,
	int NumberOfSNPs,
	int NumberOfIterations,
	int NumberOfSNPPositionLength);

