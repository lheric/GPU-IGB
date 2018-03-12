//
// AnovaTestClient.cpp
//

#pragma comment (lib, "Ws2_32.lib")

//
// Enable this symbol if you want the indexes
// checked when indexing into the arrays (CArray1DCPU.h).
//
#define DEBUG_INDEXING 1

#include <stdlib.h>
#include <crtdbg.h>
#include "..\Common\dbgnew.h"

#include <stdio.h>
#include <assert.h>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <chrono>
#include <list>
#include <map>
#include <random>


#include "..\Common\Timer.h"
#include "..\Common\Sockets.h"
#include "..\Common\CArray1D.h"
#include "..\Common\CArray1DTCP.h"
#include "..\Common\CArray2D.h"
#include "..\Common\CArray2DTCP.h"


using namespace std;

#define DEFAULT_PORT "98452"
#define SUFFIX "-CPU"

// =======================================================================
// DumpMatrix -- for float matrices.
// =======================================================================
void DumpMatrix(CArray2D_TCP<float> &M, char *filename, char *extension, int id, bool writetext = true)
{
	stringstream ss;

	ss << filename << '-';
	ss.fill('0');
	ss.width(5);
	ss << id << extension;

	ofstream file;
	if (writetext)
	{
		file.open(ss.str(), ofstream::out);

		if (file.is_open())
		{
			for (unsigned int i = 0; i < M.GetHeight(); i++)\
			{
				file.width(5);
				file << i << " : ";
				for (unsigned int j = 0; j < M.GetWidth(); j++)
				{
					file << fixed << setprecision(4) << setw(10) << M(i, j) << ", ";
				}
				file << endl;
			}

			file.close();
		}
	}
	else
	{
		file.open(ss.str(), ofstream::out | ofstream::binary);

		if (file.is_open())
		{
			file.write((char*)M.GetHostDataPtr(), M.SizeOfMemory());
			file.close();
		}
	}
}

// =======================================================================
// DumpMatrix -- for integer matrices.
// =======================================================================
void DumpMatrix(CArray2D_TCP<int> &M, char *filename, char *extension, int id, bool writetext = true)
{
	stringstream ss;

	ss << filename << '-';
	ss.fill('0');
	ss.width(5);
	ss << id << extension;

	ofstream file;
	if (writetext)
	{
		file.open(ss.str(), ofstream::out);

		if (file.is_open())
		{
			for (unsigned int i = 0; i < M.GetHeight(); i++)\
			{
				file.width(5);
				file << i << " : ";
				for (unsigned int j = 0; j < M.GetWidth(); j++)
				{
					file << fixed << setw(3) << M(i, j) << ", ";
				}
				file << endl;
			}

			file.close();
		}
	}
	else
	{
		file.open(ss.str(), ofstream::out | ofstream::binary);

		if (file.is_open())
		{
			file.write((char*)M.GetHostDataPtr(), M.SizeOfMemory());
			file.close();
		}
	}
}

// =======================================================================
// DoFunctionANova -- this is the client side function that communicates
// to a server to request that the ANOVA be computed.
// =======================================================================

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
	CSocket* CliSock)
{
	CMD_t Cmd;

	Cmd.opcode = OPC_FunctionAnova;
	Cmd.C.FunctionAnova.VoxelSubject = pVoxelSubject;
	Cmd.C.FunctionAnova.SNPSubject = pSNPSubject;
	Cmd.C.FunctionAnova.VoxelSNP = pVoxelSNP;

	Cmd.C.FunctionAnova.NumberOfSubjects = NumberOfSubjects;
	Cmd.C.FunctionAnova.NumberOfVoxels = NumberOfVoxels;
	Cmd.C.FunctionAnova.NumberOfSNPs = NumberOfSNPs;

	Cmd.C.FunctionAnova.VoxelList = pVoxelList;
	Cmd.C.FunctionAnova.VoxelListCount = VoxelListCount;
	Cmd.C.FunctionAnova.SNPList = pSNPList;
	Cmd.C.FunctionAnova.SNPListCount = SNPListCount;

	int iResult = CliSock->SendTCPMessage(&Cmd, NULL);
	if (iResult != 0)
	{
		cout << "DoFunctionANova -- send error" << endl;
		return false;
	}

	CMD_t RetCmd;
	char *payload = NULL;
	unsigned int payload_size = 0;

	iResult = CliSock->ReceiveTCPMessage(&RetCmd, &payload, &payload_size);
	if (iResult != 0)
	{
		cout << "DoFunctionANova -- recv error" << endl;
		return false;
	}

	return true;
}

// =======================================================================
// DoFunctionVegas -- this is the client side function that communicates
// to a server to request that the VegasTest be computed.
// =======================================================================

bool DoFunctionVegasTest(float *pVoxelGeneObserved,
	float *pVoxelGeneSim,
	float *pVoxelSNP,
	int *pSNPPosLenPairs,
	uint64_t *pLDMatrixList,
	int NumberOfVoxels,
	int NumberOfGenes,
	int NumberOfSNPs,
	int NumberOfIterations,
	int NumberOfSNPPositionLength,
	CSocket* CliSock)
{
	CMD_t Cmd;

	Cmd.opcode = OPC_FunctionVegasTest;
	Cmd.C.FunctionVegasTest.VoxelGeneObserved = pVoxelGeneObserved;
	Cmd.C.FunctionVegasTest.VoxelGeneSim = pVoxelGeneSim;
	Cmd.C.FunctionVegasTest.VoxelSNP = pVoxelSNP;
	Cmd.C.FunctionVegasTest.SNPPosLenPairs = pSNPPosLenPairs;
	Cmd.C.FunctionVegasTest.LDMatrixList = pLDMatrixList;
	Cmd.C.FunctionVegasTest.NumberOfVoxels = NumberOfVoxels;
	Cmd.C.FunctionVegasTest.NumberOfGenes = NumberOfGenes;
	Cmd.C.FunctionVegasTest.NumberOfSNPs = NumberOfSNPs;
	Cmd.C.FunctionVegasTest.NumberOfIterations = NumberOfIterations;
	Cmd.C.FunctionVegasTest.NumberOfSNPPositionLength = NumberOfSNPPositionLength;

	int iResult = CliSock->SendTCPMessage(&Cmd, NULL);
	if (iResult != 0)
	{
		cout << "DoFunctionVegasTest -- send error" << endl;
		return false;
	}

	CMD_t RetCmd;
	char *payload = NULL;
	unsigned int payload_size = 0;

	iResult = CliSock->ReceiveTCPMessage(&RetCmd, &payload, &payload_size);
	if (iResult != 0)
	{
		cout << "DoFunctionVegasTest -- recv error" << endl;
		return false;
	}

	return true;
}

float FRand(void)
{
	return (float)rand() / (float)RAND_MAX;
}

int randbetween(int low, int hi)
{
	return (int)((hi - low)*FRand() + low);
}

// =======================================================================
// AnovaTestTallSkinny -- this is the driver function that runs tests.
// =======================================================================
bool AnovaTestTallSkinny(const int NumberOfSubjects, int NumberOfVoxels, int NumberOfSNPs, int Id, CSocket* ClientSocket)
{
	cout << "\n*** Running AnovaTestTallSkinny ***" << endl;

	//
	// Allocate some vectors that can be used on the host.
	//
	CArray2D_TCP<float> VoxelSubject(ClientSocket, NumberOfVoxels, NumberOfSubjects, false);
	CArray2D_TCP<int> SNPSubject(ClientSocket, NumberOfSNPs, NumberOfSubjects, false);

	//
	// The VoxelSNP matrix will probably act like the "window" into the problem. 
	// The size of the matrix will probably be something like 400 x 400 because that
	// is the matrix that will be displayed on the screen.
	//
	// I currently have it initialized to be NumberOfVoxels by NumberOfSNPs.
	//
	CArray2D_TCP<float> VoxelSNP(ClientSocket, NumberOfVoxels, 1, true, -1.0f);
	CArray1D_TCP<int> VoxelList(ClientSocket, VoxelSNP.GetHeight(), true, 0);
	CArray1D_TCP<int> SNPList(ClientSocket, 1, true, 0);

	cout << "\n\tsize of window " << VoxelSNP.GetHeight() << " by " << VoxelSNP.GetWidth() << endl;

	//
	// Randomly initialize the VoxelSubject and SNPSubject matrices.
	//
	for (unsigned int i = 0; i < VoxelSubject.GetHeight(); i++)
	{
		for (unsigned int j = 0; j < VoxelSubject.GetWidth(); j++)
		{
			VoxelSubject(i, j) = FRand() * 100.0f;
		}
	}

	for (unsigned int i = 0; i < SNPSubject.GetHeight(); i++)
		for (unsigned int j = 0; j < SNPSubject.GetWidth(); j++)
		{
			float R = FRand();

			if (R < 0.33f) SNPSubject(i, j) = 0;
			else
				if (R < 0.67f) SNPSubject(i, j) = 1;
				else
					SNPSubject(i, j) = 2;
		}

	//
	// Intialize the 2 vectors that describe which voxels and which SNPs
	// to process on the server.
	//
	for (unsigned int i = 0; i < VoxelList.GetNumElements(); i++)
		VoxelList(i) = i;

	for (unsigned int i = 0; i < SNPList.GetNumElements(); i++)
		SNPList(i) = 1;


	VoxelSubject.CopyToServer();
	SNPSubject.CopyToServer();
	VoxelList.CopyToServer();
	SNPList.CopyToServer();

	bool Result = true;


	cout << "Begin " <<  endl;

	Timer tmr;

	Result |= DoFunctionANova(
		VoxelSubject.GetServerDataPtr(),
		SNPSubject.GetServerDataPtr(),
		VoxelSNP.GetServerDataPtr(),
		NumberOfVoxels,
		NumberOfSubjects,
		NumberOfSNPs,
		VoxelList.GetServerDataPtr(),
		VoxelList.GetNumElements(),
		SNPList.GetServerDataPtr(),
		SNPList.GetNumElements(),
		ClientSocket);


	double DeltaTime = tmr.elapsed();

	cout << "Elapsed time " << DeltaTime << " seconds." << endl;

	if (Result)
	{
		VoxelSNP.CopyFromServer();

		// Dump the matrices to a file. Since the client app doesn't know
		// if it is talking to a CPU or GPU server, just give the file a generic
		// name. Then rename it when the program exits and compare the CPU version
		// of the file against the GPU version of the file using a DIFF tool.
		//
		DumpMatrix(VoxelSNP, "AnovaTestTallSkinny" SUFFIX, ".txt", Id);
		//			DumpMatrix(VoxelSNP, "VoxelSNP", ".bin", c, false);
		//			DumpMatrix(VoxelSubject, "VoxelSubject", ".txt", c);
		//			DumpMatrix(SNPSubject, "SNPSubject", ".txt", c);
	}



	return Result;
}

// =======================================================================
// AnovaTestFullWindow -- this is the driver function that runs tests.
// =======================================================================
bool AnovaTestFullWindow(const int NumberOfSubjects, int NumberOfVoxels, int NumberOfSNPs, int Id, CSocket* ClientSocket)
{
	cout << "\n*** Running AnovaTestFullWindow ***" << endl;

	//
	// Allocate some vectors that can be used on the host.
	//
	CArray2D_TCP<float> VoxelSubject(ClientSocket, NumberOfVoxels, NumberOfSubjects, false);
	CArray2D_TCP<int> SNPSubject(ClientSocket, NumberOfSNPs, NumberOfSubjects, false);

	//
	// The VoxelSNP matrix will probably act like the "window" into the problem. 
	// The size of the matrix will probably be something like 400 x 400 because that
	// is the matrix that will be displayed on the screen.
	//
	// I currently have it initialized to be NumberOfVoxels by NumberOfSNPs.
	//
	CArray2D_TCP<float> VoxelSNP(ClientSocket, NumberOfVoxels, NumberOfSNPs, true, 0.0f);
	CArray1D_TCP<int> VoxelList(ClientSocket, VoxelSNP.GetHeight(), true, 0);
	CArray1D_TCP<int> SNPList(ClientSocket, VoxelSNP.GetWidth(), true, 0);

	cout << "\n\tsize of window " << VoxelSNP.GetHeight() << " by " << VoxelSNP.GetWidth() << endl;
	//
	// Randomly initialize the VoxelSubject and SNPSubject matrices.
	//
	for (unsigned int i = 0; i < VoxelSubject.GetHeight(); i++)
	{
		for (unsigned int j = 0; j < VoxelSubject.GetWidth(); j++)
		{
			VoxelSubject(i, j) = FRand() * 100.0f;
		}
	}

	for (unsigned int i = 0; i < SNPSubject.GetHeight(); i++)
		for (unsigned int j = 0; j < SNPSubject.GetWidth(); j++)
		{
			float R = FRand();

			if (R < 0.33f) SNPSubject(i, j) = 0;
			else
				if (R < 0.67f) SNPSubject(i, j) = 1;
				else
					SNPSubject(i, j) = 2;
		}

	//
	// Intialize the 2 vectors that describe which voxels and which SNPs
	// to process on the server.
	//
	for (unsigned int i = 0; i < VoxelList.GetNumElements(); i++)
		VoxelList(i) = i;

	for (unsigned int i = 0; i < SNPList.GetNumElements(); i++)
		SNPList(i) = i;


	VoxelSubject.CopyToServer();
	SNPSubject.CopyToServer();
	VoxelList.CopyToServer();
	SNPList.CopyToServer();

	bool Result = true;

	cout << "Begin ..." << endl;

	Timer tmr;

	Result |= DoFunctionANova(
		VoxelSubject.GetServerDataPtr(),
		SNPSubject.GetServerDataPtr(),
		VoxelSNP.GetServerDataPtr(),
		NumberOfVoxels,
		NumberOfSubjects,
		NumberOfSNPs,
		VoxelList.GetServerDataPtr(),
		VoxelList.GetNumElements(),
		SNPList.GetServerDataPtr(),
		SNPList.GetNumElements(),
		ClientSocket);


	double DeltaTime = tmr.elapsed();

	cout << "Elapsed time " << DeltaTime << " seconds." << endl;

	if (Result)
	{
		VoxelSNP.CopyFromServer();

		// Dump the matrices to a file. Since the client app doesn't know
		// if it is talking to a CPU or GPU server, just give the file a generic
		// name. Then rename it when the program exits and compare the CPU version
		// of the file against the GPU version of the file using a DIFF tool.
		//
		DumpMatrix(VoxelSNP, "AnovaTestFullWindow" SUFFIX, ".txt", Id);
		//			DumpMatrix(VoxelSNP, "VoxelSNP", ".bin", c, false);
		//			DumpMatrix(VoxelSubject, "VoxelSubject", ".txt", c);
		//			DumpMatrix(SNPSubject, "SNPSubject", ".txt", c);
	}

	return Result;
}

// =======================================================================
// AnovaTestPartialWindow -- this is the driver function that runs tests.
// =======================================================================
bool AnovaTestPartialWindow(const int NumberOfSubjects, int NumberOfVoxels, int NumberOfSNPs, int WWidth, int WHeight, int Id, CSocket* ClientSocket)
{
	cout << "\n*** Running AnovaTestFullWindow ***" << endl;

	//
	// Allocate some vectors that can be used on the host.
	//
	CArray2D_TCP<float> VoxelSubject(ClientSocket, NumberOfVoxels, NumberOfSubjects, false);
	CArray2D_TCP<int> SNPSubject(ClientSocket, NumberOfSNPs, NumberOfSubjects, false);

	//
	// The VoxelSNP matrix will probably act like the "window" into the problem. 
	// The size of the matrix will probably be something like 400 x 400 because that
	// is the matrix that will be displayed on the screen.
	//
	// I currently have it initialized to be NumberOfVoxels by NumberOfSNPs.
	//
	CArray2D_TCP<float> VoxelSNP(ClientSocket, WHeight, WWidth, true, 0.0f);
	CArray1D_TCP<int> VoxelList(ClientSocket, VoxelSNP.GetHeight(), true, 0);
	CArray1D_TCP<int> SNPList(ClientSocket, VoxelSNP.GetWidth(), true, 0);

	cout << "\n\tsize of window " << VoxelSNP.GetHeight() << " by " << VoxelSNP.GetWidth() << endl;
	//
	// Randomly initialize the VoxelSubject and SNPSubject matrices.
	//
	for (unsigned int i = 0; i < VoxelSubject.GetHeight(); i++)
	{
		for (unsigned int j = 0; j < VoxelSubject.GetWidth(); j++)
		{
			VoxelSubject(i, j) = FRand() * 100.0f;
		}
	}

	for (unsigned int i = 0; i < SNPSubject.GetHeight(); i++)
		for (unsigned int j = 0; j < SNPSubject.GetWidth(); j++)
		{
			float R = FRand();

			if (R < 0.33f) SNPSubject(i, j) = 0;
			else
				if (R < 0.67f) SNPSubject(i, j) = 1;
				else
					SNPSubject(i, j) = 2;
		}

	//
	// Intialize the 2 vectors that describe which voxels and which SNPs
	// to process on the server.
	//
	for (unsigned int i = 0; i < VoxelList.GetNumElements(); i++)
		VoxelList(i) = i;

	for (unsigned int i = 0; i < SNPList.GetNumElements(); i++)
		SNPList(i) = i;


	VoxelSubject.CopyToServer();
	SNPSubject.CopyToServer();
	VoxelList.CopyToServer();
	SNPList.CopyToServer();

	bool Result = true;

	cout << "Begin ..." << endl;

	Timer tmr;

	Result |= DoFunctionANova(
		VoxelSubject.GetServerDataPtr(),
		SNPSubject.GetServerDataPtr(),
		VoxelSNP.GetServerDataPtr(),
		NumberOfVoxels,
		NumberOfSubjects,
		NumberOfSNPs,
		VoxelList.GetServerDataPtr(),
		VoxelList.GetNumElements(),
		SNPList.GetServerDataPtr(),
		SNPList.GetNumElements(),
		ClientSocket);


	double DeltaTime = tmr.elapsed();

	cout << "Elapsed time " << DeltaTime << " seconds." << endl;

	if (Result)
	{
		VoxelSNP.CopyFromServer();

		// Dump the matrices to a file. Since the client app doesn't know
		// if it is talking to a CPU or GPU server, just give the file a generic
		// name. Then rename it when the program exits and compare the CPU version
		// of the file against the GPU version of the file using a DIFF tool.
		//
		DumpMatrix(VoxelSNP, "AnovaTestPartialWindow" SUFFIX, ".txt", Id);
		//			DumpMatrix(VoxelSNP, "VoxelSNP", ".bin", c, false);
		//			DumpMatrix(VoxelSubject, "VoxelSubject", ".txt", c);
		//			DumpMatrix(SNPSubject, "SNPSubject", ".txt", c);
	}

	return Result;
}

// =======================================================================
// VegasTestOne -- this is the driver function that runs tests.
// =======================================================================

typedef struct LDNode
{
	int width;
	float *data;
} LDNode_t;


typedef struct SNPNode
{
	string name;
	vector<char> vals;
} SNPNode_t;

typedef struct GeneNode
{
	string GeneName;
	vector<SNPNode_t> SNP;
} GeneNode_t;

vector<string> split(const string &text, char sep)
{
	vector<string> tokens;
	size_t start = 0, end = 0;
	while ((end = text.find(sep, start)) != string::npos)
	{
		tokens.push_back(text.substr(start, end - start));
		start = end + 1;
	}
	tokens.push_back(text.substr(start));
	return tokens;
}

list<GeneNode_t> ReadGeneSnpFile(string GeneSnpFileName)
{
	string line;
	int linecount = 0;

	cout << endl << endl << "ReadGeneSnpFile-reading " << GeneSnpFileName << endl;

	ifstream infile(GeneSnpFileName);
	if (!infile.is_open())
	{
		cout << "Error opening GeneSnpFile " << GeneSnpFileName << endl << endl;
		exit(1);
	}

	list<GeneNode_t> GNList;
	GeneNode_t GN;
	SNPNode_t SN;

	while (getline(infile, line))
	{
		linecount++;
		if (linecount == 1)
			continue;			// skip the header

		vector<string> tokens = split(line, '\t');
		//		cout << "ReadGeneSnpFile line " << linecount << ", tokencount " << tokens.size() << endl;

		if (GN.GeneName.compare(tokens[5]) != 0)	// start a new gene
		{
			if (linecount > 2) GNList.push_back(GN);
			GN.GeneName = tokens[5];
			GN.SNP.clear();
			SN.name = tokens[1];
			GN.SNP.push_back(SN);
		}
		else
		{
			SN.name = tokens[1];
			GN.SNP.push_back(SN);
		}
	}

	GNList.push_back(GN);

	cout << linecount << " lines in GeneSnpFile" << endl;
	cout << GNList.size() << " genes in the file" << endl;

	return GNList;
}


map<string, float> ReadSNPPvaluesFile(string SNPPvaluesFileName)
{
	string line;
	int linecount = 0;

	cout << endl << endl << "ReadSNPPvaluesFile-reading " << SNPPvaluesFileName << endl;

	ifstream infile(SNPPvaluesFileName);
	if (!infile.is_open())
	{
		cout << "Error opening ReadSNPPvaluesFile " << SNPPvaluesFileName << endl << endl;
		exit(1);
	}

	map<string, float> SPList;

	while (getline(infile, line))
	{
		linecount++;
		vector<string> tokens = split(line, ' ');
		float P = stof(tokens[1]);

		if (P < 1.0E-5f) P = 1.0E-5f;	// Clamp P because we are using floats

		pair<map<string, float>::iterator, bool> ret;
		ret = SPList.insert(pair<string, float>(tokens[0], P));
	}

	return SPList;
}


//
// These 2 variables are used to hold the state for GetFirstGeneSNPValue and
// GetNextGeneSNPValue.
//
list<GeneNode_t>::iterator LastGene;
int LastSNPNameIndex;

float GetFirstGeneSNPValue(list<GeneNode_t> &GL, map<string, float> SNPPvalueMap)
{
	LastGene = GL.begin();
	LastSNPNameIndex = 0;

	string SNPName = LastGene->SNP[LastSNPNameIndex].name;

	map<string, float>::iterator it;
	it = SNPPvalueMap.find(SNPName);

	if (it == SNPPvalueMap.end())
	{
		cout << "Can't find SNP " << SNPName << " in map. " << endl << endl;
		exit(1);
	}

	return it->second;
}

float GetNextGeneSNPValue(list<GeneNode_t> &GL, map<string, float> SNPPvalueMap)
{
	if (LastGene == GL.end())
		return -99.0f;		// if you call this function too many times it returns -99.

	LastSNPNameIndex++;
	if (LastSNPNameIndex == LastGene->SNP.size())
	{
		LastSNPNameIndex = 0;
		LastGene++;
		if (LastGene == GL.end())
			return -99.0f;
	}

	string SNPName = LastGene->SNP[LastSNPNameIndex].name;

	map<string, float>::iterator it;
	it = SNPPvalueMap.find(SNPName);

	if (it == SNPPvalueMap.end())
	{
		cout << "Can't find SNP " << SNPName << " in map. " << endl << endl;
		exit(1);
	}

	return it->second;
}

int GetNumberOfGenes(list<GeneNode_t> &GL)
{
	return (int)GL.size();
}

int GetNumberOfSNPs(list<GeneNode_t> &GL)
{
	int sum = 0;

	for (list<GeneNode_t>::iterator git = GL.begin(); git != GL.end(); git++)
	{
		sum += (int)git->SNP.size();
	}

	return sum;
}

//
// This function returns a vector of <position,length> pairs.
// The pairs describe where the SNPs are located within the
// VoxelSNP matrix.
//
// e.g.
// G1  S1 S2 S3
// G2  S4 S5 S6 S7
// G3  S8 S9 S10 S11 S12
//
// the return pairs are (0,3) (3,4), (7, 5).
//
// The number of pairs in the list is equal to the number of genes,
// or the number of integers in the list is equal to number of genes * 2.
//
vector<int> ComputeSNPPositionLengthList(list<GeneNode_t> &GL)
{
	vector<int> TheList;

	int Posn = 0;
	int Length = 0;

	for (list<GeneNode_t>::iterator git = GL.begin(); git != GL.end(); git++)
	{
		Length = (int)git->SNP.size();
		TheList.push_back(Posn);
		TheList.push_back(Length);

		Posn += Length;
	}

	return TheList;
}

void PopulateLDMatrix(CArray2D_TCP<float> *Q, int GeneNumber, vector<string> CholeskyLines)
{
	vector<string> tokens = split(CholeskyLines[GeneNumber], ' ');

	int inx = 0;
	for (unsigned int y = 0; y < Q->GetHeight(); y++)
		for (unsigned int x = 0; x < Q->GetWidth(); x++)
		{
			float P = stof(tokens[inx++]);
			isfinite(P);		// Let's be certain that there are no NaN's in the factorization.
			(*Q)(y, x) = P;
		}
}

// Read the file that contains the "LD" matrix for each gene.
// There are 3 lines for each gene.
// a comment
// the covariance (or Pearson correlation)
// the factored matrix
//
// We will return a vector containing just the rows from the
// factored matrices.
//
vector<string> ReadSNPCovFactored(string SNPCovFactoredFileName)
{
	vector <string> Lines;
	string line;

	cout << endl << endl << "ReadSNPCovFactored-reading " << SNPCovFactoredFileName << endl;

	ifstream infile(SNPCovFactoredFileName);
	if (!infile.is_open())
	{
		cout << "Error opening ReadSNPCovFactored " << SNPCovFactoredFileName << endl << endl;
		exit(1);
	}

	while(getline(infile, line))	// The comment line
	{
		getline(infile, line);		// The covariance line
		getline(infile, line);		// The cholesky line
		Lines.push_back(line);
	}

	return Lines;
}


bool VegasTestOne(CSocket* ClientSocket)
{
	cout << "\n*** Running VegasTestOne ***" << endl;

	const int NumberOfVoxels			= 1;
	const int NumberOfIterations		= 1000000;
	const char *SNPPvaluesFileName		= "..\\SNPCov\\data\\snp-pvalue-1.txt";
	const char *GeneSNPFileName			= "..\\SNPCov\\data\\gene-snp-1.txt";
	const char *SNPCovFactoredFileName	= "..\\SNPCov\\data\\snpcov-factored-3.txt";

	//
	// Read in each gene, along with all of the SNP's associated with it.
	//
	// Then read in the file containing the (snp, pvalue) pairs.
	//
	list<GeneNode_t> GL					= ReadGeneSnpFile(GeneSNPFileName);
	map<string, float> SNPPvalues		= ReadSNPPvaluesFile(SNPPvaluesFileName);

	const int NumberOfGenes				= GetNumberOfGenes(GL);
	const int NumberOfSNPs				= GetNumberOfSNPs(GL);

	//
	// Overview -- we need to setup these variables.
	//
	// VoxelSNP
	// VoxelGeneObserved
	// VoxelGeneSim
	// SNPPosLenPairs
	// LDMatrixList
	// 

	CArray2D_TCP<float> VoxelSNP(ClientSocket, NumberOfVoxels, NumberOfSNPs, false);
	assert(VoxelSNP.GetHeight() == 1);

	float P = GetFirstGeneSNPValue(GL, SNPPvalues);

	for (unsigned int j = 0; j < VoxelSNP.GetWidth(); j++)
	{
		VoxelSNP(0, j) = P;

		assert(P >= 0.0f && P < 1.0f);
		P = GetNextGeneSNPValue(GL, SNPPvalues);
	}
	VoxelSNP.CopyToServer();


	CArray2D_TCP<float> VoxelGeneObserved(ClientSocket, NumberOfVoxels, NumberOfGenes, true);
	CArray2D_TCP<float> VoxelGeneSim(ClientSocket, NumberOfVoxels, NumberOfGenes, true);

	vector<int> SNPPositionLength = ComputeSNPPositionLengthList(GL);

	CArray1D_TCP<int> SNPPosLenPairs(ClientSocket, (int)SNPPositionLength.size(), true);

	for (int i = 0; i < SNPPositionLength.size(); i += 2)
	{
		SNPPosLenPairs(i) = SNPPositionLength[i];
		SNPPosLenPairs(i+1) = SNPPositionLength[i+1];
	}
	SNPPosLenPairs.CopyToServer();

	//
	// The LDMatrixList consists of pairs of 64 bit unsigned ints.
	// The first integer is the size N of an N x N matrix.
	// The second integer is the 64 bit GPU pointer to the RAM for the matrix.
	//
	CArray1D_TCP<uint64_t> LDMatrixList(ClientSocket, NumberOfGenes * 2, true);

	vector<string> SNPCovFactoredLines = ReadSNPCovFactored(SNPCovFactoredFileName);

	vector<CArray2D_TCP<float>*> LDMatrixVec;

	for (unsigned int i = 0; i < LDMatrixList.GetNumElements(); i += 2)
	{
		CArray2D_TCP<float> *Q = new CArray2D_TCP<float>(ClientSocket, SNPPositionLength[i + 1], SNPPositionLength[i + 1], true);

		LDMatrixVec.push_back(Q);

		//
		// Now populate Q, the pointer to the LDMatrix for Gene_i.
		//
		PopulateLDMatrix(Q, i / 2, SNPCovFactoredLines);
		Q->CopyToServer();

		LDMatrixList(i)		= SNPPositionLength[i + 1];
		LDMatrixList(i + 1) = (uint64_t)Q->GetServerDataPtr();
	}
	LDMatrixList.CopyToServer();

	bool Result = true;

	cout << "\nBegin ..." << endl;

	Timer tmr;

	Result |= DoFunctionVegasTest(
		VoxelGeneObserved.GetServerDataPtr(),
		VoxelGeneSim.GetServerDataPtr(),
		VoxelSNP.GetServerDataPtr(),
		SNPPosLenPairs.GetServerDataPtr(),
		LDMatrixList.GetServerDataPtr(),
		NumberOfVoxels,
		NumberOfGenes,
		NumberOfSNPs,
		NumberOfIterations,
		(int)SNPPositionLength.size(),
		ClientSocket);


	double DeltaTime = tmr.elapsed() * 1000.0;

	cout << "Elapsed time " << DeltaTime << " milliseconds." << endl;

	if (Result)
	{
		VoxelGeneSim.CopyFromServer();
		DumpMatrix(VoxelGeneSim, "VegasTestOne-VoxGeneSim" SUFFIX, ".txt", 0);
	}

	while (!LDMatrixVec.empty())
	{
		CArray2D_TCP<float> *Q = LDMatrixVec.back();
		LDMatrixVec.pop_back();
		delete Q;
	}

	return Result;
}

// =======================================================================
// VegasTestTwo -- this is the driver function that runs tests.
// =======================================================================

bool VegasTestTwo(const int NumberOfSubjects, int NumberOfVoxels, int NumberOfGenes, int NumberOfSNPs, int WWidth, int WHeight, list<LDNode_t*> LD, CSocket* ClientSocket)
{
	cout << "\n*** Running VegasTestTwo ***" << endl;
#if 0
	//
	// Allocate some vectors that can be used on the host.
	//
	CArray2D_TCP<float> VoxelSubject(ClientSocket, NumberOfVoxels, NumberOfSubjects, false);
	CArray2D_TCP<int> SNPSubject(ClientSocket, NumberOfSNPs, NumberOfSubjects, false);

	//
	// The VoxelSNP matrix will probably act like the "window" into the problem. 
	// The size of the matrix will probably be something like 400 x 400 because that
	// is the matrix that will be displayed on the screen.
	//
	// I currently have it initialized to be NumberOfVoxels by NumberOfSNPs.
	//
	CArray2D_TCP<float> VoxelGene(ClientSocket, WHeight, WWidth, true, 0.0f);

	CArray2D_TCP<float> VoxelSNP(ClientSocket, WHeight, WWidth, true, 0.0f);
	CArray1D_TCP<int> VoxelList(ClientSocket, VoxelSNP.GetHeight(), true, 0);
	CArray1D_TCP<int> SNPList(ClientSocket, VoxelSNP.GetWidth(), true, 0);

	cout << "\n\tsize of window " << VoxelSNP.GetHeight() << " by " << VoxelSNP.GetWidth() << endl;
	//
	// Randomly initialize the VoxelSubject and SNPSubject matrices.
	//
	for (unsigned int i = 0; i < VoxelSubject.GetHeight(); i++)
	{
		for (unsigned int j = 0; j < VoxelSubject.GetWidth(); j++)
		{
			VoxelSubject(i, j) = FRand() * 100.0f;
		}
	}

	for (unsigned int i = 0; i < SNPSubject.GetHeight(); i++)
		for (unsigned int j = 0; j < SNPSubject.GetWidth(); j++)
		{
			float R = FRand();

			if (R < 0.33f) SNPSubject(i, j) = 0;
			else
				if (R < 0.67f) SNPSubject(i, j) = 1;
				else
					SNPSubject(i, j) = 2;
		}

	//
	// Intialize the 2 vectors that describe which voxels and which SNPs
	// to process on the server.
	//
	for (unsigned int i = 0; i < VoxelList.GetNumElements(); i++)
		VoxelList(i) = i;

	for (unsigned int i = 0; i < SNPList.GetNumElements(); i++)
		SNPList(i) = i;


	VoxelSubject.CopyToServer();
	SNPSubject.CopyToServer();
	VoxelList.CopyToServer();
	SNPList.CopyToServer();

	bool Result = true;

	cout << "Begin ..." << endl;

	Timer tmr;

	Result |= DoFunctionVegas(
		VoxelSubject.GetServerDataPtr(),
		SNPSubject.GetServerDataPtr(),
		VoxelSNP.GetServerDataPtr(),
		NumberOfVoxels,
		NumberOfSubjects,
		NumberOfSNPs,
		VoxelList.GetServerDataPtr(),
		VoxelList.GetNumElements(),
		SNPList.GetServerDataPtr(),
		SNPList.GetNumElements(),
		ClientSocket);


	double DeltaTime = tmr.elapsed();

	cout << "Elapsed time " << DeltaTime << " seconds." << endl;

	if (Result)
	{
		VoxelSNP.CopyFromServer();

		// Dump the matrices to a file. Since the client app doesn't know
		// if it is talking to a CPU or GPU server, just give the file a generic
		// name. Then rename it when the program exits and compare the CPU version
		// of the file against the GPU version of the file using a DIFF tool.
		//
		DumpMatrix(VoxelGene, "VegasTestOne" SUFFIX, ".txt", 0);
		//			DumpMatrix(VoxelSNP, "VoxelSNP", ".bin", c, false);
		//			DumpMatrix(VoxelSubject, "VoxelSubject", ".txt", c);
		//			DumpMatrix(SNPSubject, "SNPSubject", ".txt", c);
	}
#endif
	return true;
}

float fRand_uniform(const float & lo, const float & hi)
{
	static thread_local mt19937* generator = nullptr;
//f (!generator) generator = new mt19937(std::random_device{}());
	if (!generator) generator = new mt19937(1);
	uniform_real_distribution<float> distribution(lo, hi);
	return distribution(*generator);
}

// =======================================================================
// MAIN program logic
// =======================================================================


int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	cout << "Basic Anova Client is running ..." << endl;

	srand(1);

	int iResult = CSocket::Initialize();
	if (iResult)
	{
		cout << "\nError during CSocket::Initialize";
		exit(EXIT_FAILURE);
	}

	CSocket* ClientSocket = new CSocket();
	if (ClientSocket->ConnectToTCPServer("localhost", DEFAULT_PORT))
	{
		delete ClientSocket;
		exit(EXIT_FAILURE);
	}

#if 0
	try
	{
		const int NumberOfVoxels = 1024*1024-16;
		const int NumberOfSubjects = 64;
		const int NumberOfSNPs = 32;

		cout << endl;
		cout << "Number Of Voxels   = " << NumberOfVoxels << endl;
		cout << "Number Of Subjects = " << NumberOfSubjects << endl;
		cout << "Number Of SNPS     = " << NumberOfSNPs << endl;
		cout << endl;

		{
			bool b = AnovaTestTallSkinny(NumberOfSubjects, NumberOfVoxels, NumberOfSNPs, 10, ClientSocket);
			cout << "Result = " << (b ? "Success" : "Failure") << endl << endl;
		}
	}
	catch (char *msg)
	{
		cout << "Exception Thrown :: " << msg << endl;
	}
#endif

#if 0
	try
	{
		const int NumberOfVoxels = 1024 * 4000;
		const int NumberOfSubjects = 64;
		const int NumberOfSNPs = 256 ;

		cout << endl;
		cout << "Number Of Voxels   = " << NumberOfVoxels << endl;
		cout << "Number Of Subjects = " << NumberOfSubjects << endl;
		cout << "Number Of SNPS     = " << NumberOfSNPs << endl;
		cout << endl;

		{
			bool b = AnovaTestTallSkinny(NumberOfSubjects, NumberOfVoxels, NumberOfSNPs, 20, ClientSocket);
			cout << "Result = " << (b ? "Success" : "Failure") << endl << endl;
		}
	}
	catch (char *msg)
	{
		cout << "Exception Thrown :: " << msg << endl;
	}
#endif

#if 0
	try
	{
		const int NumberOfVoxels = 1024;
		const int NumberOfSubjects = 1024;
		const int NumberOfSNPs = 1024;

		cout << endl;
		cout << "Number Of Voxels   = " << NumberOfVoxels << endl;
		cout << "Number Of Subjects = " << NumberOfSubjects << endl;
		cout << "Number Of SNPS     = " << NumberOfSNPs << endl;
		cout << endl;

		{
			bool b = AnovaTestFullWindow(NumberOfSubjects, NumberOfVoxels, NumberOfSNPs, 10, ClientSocket);
			cout << "Result = " << (b ? "Success" : "Failure") << endl << endl;
		}
	}
	catch (char *msg)
	{
		cout << "Exception Thrown :: " << msg << endl;
	}
#endif

#if 0
	try
	{
		const int NumberOfVoxels = 1024;
		const int NumberOfSubjects = 1024;
		const int NumberOfSNPs = 1024 * 16;

		cout << endl;
		cout << "Number Of Voxels   = " << NumberOfVoxels << endl;
		cout << "Number Of Subjects = " << NumberOfSubjects << endl;
		cout << "Number Of SNPS     = " << NumberOfSNPs << endl;
		cout << endl;

		{
			bool b = AnovaTestFullWindow(NumberOfSubjects, NumberOfVoxels, NumberOfSNPs, 20, ClientSocket);
			cout << "Result = " << (b ? "Success" : "Failure") << endl << endl;
		}
	}
	catch (char *msg)
	{
		cout << "Exception Thrown :: " << msg << endl;
	}
#endif

#if 0
	try
	{
		const int NumberOfVoxels = 774656;
		const int NumberOfSubjects = 10;
		const int NumberOfSNPs = 2000000;
		const int WWidth = 128;
		const int WHeight = 127;

		cout << endl;
		cout << "Number Of Voxels   = " << NumberOfVoxels << endl;
		cout << "Number Of Subjects = " << NumberOfSubjects << endl;
		cout << "Number Of SNPS     = " << NumberOfSNPs << endl;
		cout << "Window Width       = " << WWidth << endl;
		cout << "Windwow Height     = " << WHeight << endl;
		cout << endl;

		{
			bool b = AnovaTestPartialWindow(NumberOfSubjects, NumberOfVoxels, NumberOfSNPs, WWidth, WHeight, 10, ClientSocket);
			cout << "Result = " << (b ? "Success" : "Failure") << endl << endl;
		}
	}
	catch (char *msg)
	{
		cout << "Exception Thrown :: " << msg << endl;
	}
#endif

#if 1
	try
	{
		{
			//
			// VEGAS - VErsatile Gene based Assocation Study
			//
			// This test just proves that the basic algorithm is correct.
			// The API associated with this function will not be used inside of BECA
			// because BECA will compute voxel-snp pvalues using an ANOVA.
			//
			bool b = VegasTestOne(ClientSocket);
			cout << "Result = " << (b ? "Success" : "Failure") << endl << endl;
		}
	}
	catch (char *msg)
	{
		cout << "Exception Thrown :: " << msg << endl;
	}
#endif

	delete ClientSocket;

	return 0;
}

