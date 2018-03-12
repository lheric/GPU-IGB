//
// CPUCmdInterpreter.cpp
//
#define DEBUG_INDEXING 1

#include <crtdbg.h>
#include "..\Common\dbgnew.h"

#include "Profile.h"
#include <assert.h>
#include <omp.h>
#include <iostream>
#include <vector>
#include "CmdInterpreter.h"
#include "..\Common\CArray1D.h"
#include "..\Common\CArray2D.h"
#include <memory.h>
#include <random>
#include <mutex>
#include <iomanip>
#include <omp.h>
#include <algorithm>

using namespace std;

#ifndef _WIN64
#error You must use the 64 bit compiler.
#endif


CCPUCmdInterpreter::CCPUCmdInterpreter()
{

}

CCPUCmdInterpreter::~CCPUCmdInterpreter()
{

}

// ==================================================================
//
// ==================================================================
void CCPUCmdInterpreter::Init(void)
{
	cout << "Basic CPU-OpenMP Server is running ..." << endl;
	cout << "\tNumber of processors = " << omp_get_num_procs() << endl;
#ifdef DEBUG_INDEXING
	cout << "\t** Debug indexing is enabled." << endl;
#else
	cout << "\t** Debug indexing is NOT enabled." << endl;
#endif

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
}

// ==================================================================
//
// ==================================================================
void CCPUCmdInterpreter::DoMalloc1D(CSocket* pCS, CMD_t *pCmd)
{
	DLABEL(__FUNCTION__);
	DPRINT2("\tsize request (bytes)  = ", pCmd->C.GMalloc1D.size);

	void *pCPU = new unsigned char[pCmd->C.GMalloc1D.size];

	DPRINT2("\tReturned Pointer  = ", pCPU);
	DTRACE(pCPU != NULL, "NEW in " __FUNCTION__);

	CMD_t outcmd;

	outcmd.errorcode = (pCPU != NULL) ? 0 : 1;
	outcmd.pRetVal = pCPU;

	pCS->SendTCPMessage(&outcmd, NULL);
}

// ==================================================================
//
// ==================================================================
void CCPUCmdInterpreter::DoMalloc2D(CSocket* pCS, CMD_t *pCmd)
{
	DLABEL(__FUNCTION__);
	DPRINT2("\tsize request (bytes)  = ", pCmd->C.GMalloc2D.size);

	void *pCPU = new unsigned char[pCmd->C.GMalloc2D.size];

	DPRINT2("\tReturned Pointer  = ", pCPU);
	DTRACE(pCPU != NULL, "NEW in " __FUNCTION__);

	CMD_t outcmd;

	outcmd.errorcode = (pCPU != NULL) ? 0 : 1;
	outcmd.pRetVal = pCPU;

	pCS->SendTCPMessage(&outcmd, NULL);
}

// ==================================================================
//
// ==================================================================
void CCPUCmdInterpreter::DoFree(CSocket* pCS, CMD_t *pCmd)
{
	DLABEL(__FUNCTION__);
	DPRINT2("\tFreeing Pointer  = ", pCmd->C.GFree.p);

	delete[] pCmd->C.GFree.p;

	DTRACE(1 == 1, "DELETE in " __FUNCTION__);

	CMD_t outcmd;

	outcmd.errorcode = 1;
	pCS->SendTCPMessage(&outcmd, NULL);
}

// ==================================================================
//
// ==================================================================
void CCPUCmdInterpreter::DoCopyHost2Device(CSocket* pCS, CMD_t *pCmd, char* payload, int payload_size)
{
	assert(payload_size == pCmd->C.GCopyHost2Device.size);
	DLABEL(__FUNCTION__);
	DPRINT2("\tsize request (bytes)  = ", pCmd->C.GCopyHost2Device.size);

	memcpy(pCmd->C.GCopyHost2Device.pDevice, payload, pCmd->C.GCopyHost2Device.size);

	DTRACE(1 == 1, __FUNCTION__);

	CMD_t outcmd;
	outcmd.errorcode = 0;
	pCS->SendTCPMessage(&outcmd, NULL);
}

// ==================================================================
//
// ==================================================================
void CCPUCmdInterpreter::DoCopyDevice2Host(CSocket* pCS, CMD_t *pCmd)
{
	DLABEL(__FUNCTION__);
	DPRINT2("\tsize request (bytes)  = ", pCmd->C.GCopyDevice2Host.size);

	CMD_t outcmd;
	outcmd.errorcode = 0;
	outcmd.payload_len = (unsigned int)pCmd->C.GCopyDevice2Host.size;

	pCS->SendTCPMessage(&outcmd, (char*)(pCmd->C.GCopyHost2Device.pDevice));
}

// ==================================================================
// Necessary function prototypes to access the HOST functions in the 
// GPU kernel code.
// ==================================================================

float polevlfB(float xx);
float polevlfC(float xx);
float lgamf(float xx);
float incbcff(float aa, float bb, float xx);
float incbdf(float aa, float bb, float xx);
float incbpsf(float aa, float bb, float xx);
float incbetf(float aa, float bb, float xx);
float fdtrcf(float a, float b, float x);

float chdtrf(float dff, float xx);
float chdtrif(float dff, float yy);
float chdtrcf(float dff, float xx);

float CumulativeFDistributionComplimentary(float dof1, float dof2, float x);


// ==================================================================
//
// ==================================================================

void CCPUCmdInterpreter::DoFunctionAnova(CSocket* pCS, CMD_t *pCmd)
{
	DLABEL(__FUNCTION__);

	CBaseCmdInterpreter::DoFunctionAnova(pCS, pCmd);

	const int NumberOfSNPs		= pCmd->C.FunctionAnova.NumberOfSNPs;
	const int NumberOfSubjects	= pCmd->C.FunctionAnova.NumberOfSubjects;
	const int NumberOfVoxels	= pCmd->C.FunctionAnova.NumberOfVoxels;

	int *VoxelList				= pCmd->C.FunctionAnova.VoxelList;
	const int VoxelListCount	= pCmd->C.FunctionAnova.VoxelListCount;
	int *SNPList				= pCmd->C.FunctionAnova.SNPList;
	const int SNPListCount		= pCmd->C.FunctionAnova.SNPListCount;


	//
	// We know the following 3 pointers represent 2D arrays, so instance
	// the CArray2D class that will allow us to provide a pointer to the data.
	//
	// Note : The CArray2D is threadsafe. The VS and SS arrays are read only, but
	//     the VoxSNP array can be written to by multiple OpenMP threads.
	//
	CArray2D<float> VS(NumberOfVoxels, NumberOfSubjects, pCmd->C.FunctionAnova.VoxelSubject);
	CArray2D<int> SS(NumberOfSNPs, NumberOfSubjects, pCmd->C.FunctionAnova.SNPSubject);
	CArray2D<float> VoxSNP(VoxelListCount, SNPListCount, pCmd->C.FunctionAnova.VoxelSNP);


#pragma omp parallel for
	for (int ii = 0; ii < VoxelListCount; ii++)
	{
		float MeanTotal = 0.0f;;
		float N = 0.0f;
		const int i = VoxelList[ii];

		for (int s = 0; s < NumberOfSubjects; s++)
		{
			N++;
			float delta = VS(i, s) - MeanTotal;
			MeanTotal += delta / N;					// Compute the mean QT value for this voxel
		}


		for (int kk = 0; kk < SNPListCount; kk++)
		{
			float N0 = 0.0f;
			float N1 = 0.0f;
			float N2 = 0.0f;

			float Mean0 = 0.0f;
			float Mean1 = 0.0f;
			float Mean2 = 0.0f;

			const int k = SNPList[kk];

			//
			// Drop into a loop that computes the bg_var (between group variance).
			//
			for (int s = 0; s < NumberOfSubjects; s++)
			{
				const int kSS = SS(k, s);

				if (kSS == 0)
				{
					N0++;
					float delta = VS(i, s) - Mean0;
					Mean0 += delta / N0;
				}
				else
					if (kSS == 1)
					{
						N1++;
						float delta = VS(i, s) - Mean1;
						Mean1 += delta / N1;
					}
					else
					{
						N2++;
						float delta = VS(i, s) - Mean2;
						Mean2 += delta / N2;
					}
			}

			// The divisor of 2.0 is equal to 3-1 (groups).

			const float bg_var = (N0*(Mean0 - MeanTotal)*(Mean0 - MeanTotal) + N1*(Mean1 - MeanTotal)*(Mean1 - MeanTotal) + N2*(Mean2 - MeanTotal)*(Mean2 - MeanTotal)) / 2.0f;


			float SS0 = 0.0f;
			float SS1 = 0.0f;
			float SS2 = 0.0f;

			//
			// Drop into a loop that computes the wg_var (within group variance).
			//
			for (int s = 0; s < NumberOfSubjects; s++)
			{
				const int kSS = SS(k, s);
				const float kVS = VS(i, s);

				if (kSS == 0)
				{
					SS0 += (kVS - Mean0)*(kVS - Mean0);
				}
				else
					if (kSS == 1)
					{
						SS1 += (kVS - Mean1)*(kVS - Mean1);
					}
					else
					{
						SS2 += (kVS - Mean2)*(kVS - Mean2);
					}
			}

			const float wg_var = (SS0 + SS1 + SS2) / ((float)NumberOfSubjects - 3.0f);

			VoxSNP(ii, kk) = CumulativeFDistributionComplimentary(2.0f, N - 3.0f, bg_var / wg_var);
		}
	}

	DTRACE(1 == 1, __FUNCTION__);
	CMD_t outcmd;
	outcmd.errorcode = 0;
	pCS->SendTCPMessage(&outcmd, NULL);
}

// ==================================================================
//
// ==================================================================

float fRand_normal(const float & mu, const float & sigma)
{
	static thread_local mt19937* generator = nullptr;
	if (!generator) generator = new mt19937(std::random_device{}());	// this will generate a 5000 byte leak
	normal_distribution<float> distribution(mu, sigma);
	return distribution(*generator);
}

class CGene
{
public:
	CGene() : 
		m_Count(0),
		m_SnpPos(0),
		m_SnpLen(0),
		m_pLDMatrix(0),
		m_LDMatrixSize(0),
		m_ZVector(0)
	{};

	~CGene()
	{
		delete[] m_ZVector;
	}

	void AddSnpPos(int pos, int len);
	void AddLDMatrix(uint64_t N, uint64_t MatrixPtr);
	void ComputeRandomZ(void);
	void TransformZ(float VoxelGeneObservedPValue);

	int GetSnpPos(void) { return m_SnpPos;  }
	int GetSnpLen(void) { return m_SnpLen;  }
	int GetCount(void) { return m_Count; }
	void SetCount(int x) { m_Count = x; }

private:
	int		m_Count;
	int		m_SnpPos;
	int		m_SnpLen;
	float*	m_pLDMatrix;
	int		m_LDMatrixSize;
	float*	m_ZVector;
};

void CGene::AddSnpPos(int pos, int len)
{
	m_SnpPos = pos;
	m_SnpLen = len;
}

void CGene::AddLDMatrix(uint64_t N, uint64_t MatrixPtr)
{
	m_pLDMatrix = (float *)MatrixPtr;
	m_LDMatrixSize = (int)N;

	// Just a sanity check to insure the values
	// are equal. Unfortunately this implies that
	// AddSnpPos() is called before AddLDMatrix().
	//
	assert(m_LDMatrixSize == m_SnpLen);

	m_ZVector = new float[m_LDMatrixSize];
}

void CGene::ComputeRandomZ()
{
	for (int i = 0; i < m_LDMatrixSize; i++)
	{
		m_ZVector[i] = fRand_normal(0.0f, 1.0f);
	}
}

void CGene::TransformZ(float VoxelGeneObservedPValue)
{
	double nulltest = 0.0f;

	for (int i = 0; i < m_LDMatrixSize; i++)
	{
		double sum = 0;

		for (int j = 0; j < m_LDMatrixSize; j++)
		{
			sum += m_pLDMatrix[i*m_LDMatrixSize + j] * m_ZVector[j];
		}
		nulltest += sum*sum;
	}

	if (nulltest > VoxelGeneObservedPValue)
		m_Count++;
}

// ========================================================================================================================
// Huang found this code on github.
// https://github.com/arpg/sdtrack/blob/master/applications/common/chi2inv.h
//
double  normalCDF(double u)
{
	static const double a[5] =
	{
		1.161110663653770e-002, 3.951404679838207e-001, 2.846603853776254e+001,
		1.887426188426510e+002, 3.209377589138469e+003
	};
	static const double b[5] =
	{
		1.767766952966369e-001, 8.344316438579620e+000, 1.725514762600375e+002,
		1.813893686502485e+003, 8.044716608901563e+003
	};
	static const double c[9] =
	{
		2.15311535474403846e-8, 5.64188496988670089e-1, 8.88314979438837594e00,
		6.61191906371416295e01, 2.98635138197400131e02, 8.81952221241769090e02,
		1.71204761263407058e03, 2.05107837782607147e03, 1.23033935479799725E03
	};
	static const double d[9] =
	{
		1.00000000000000000e00, 1.57449261107098347e01, 1.17693950891312499e02,
		5.37181101862009858e02, 1.62138957456669019e03, 3.29079923573345963e03,
		4.36261909014324716e03, 3.43936767414372164e03, 1.23033935480374942e03
	};
	static const double p[6] =
	{
		1.63153871373020978e-2, 3.05326634961232344e-1, 3.60344899949804439e-1,
		1.25781726111229246e-1, 1.60837851487422766e-2, 6.58749161529837803e-4
	};
	static const double q[6] =
	{
		1.00000000000000000e00, 2.56852019228982242e00, 1.87295284992346047e00,
		5.27905102951428412e-1, 6.05183413124413191e-2, 2.33520497626869185e-3
	};
	double y, z;

	/*	CHECK(!std::isnan(u));
	CHECK(std::isfinite(u));*/

	y = fabs(u);
	if (y <= 0.46875 * 1.4142135623730950488016887242097)
	{
		/* evaluate erf() for |u| <= sqrt(2)*0.46875 */
		z = y * y;
		y = u * ((((a[0] * z + a[1]) * z + a[2]) * z + a[3]) * z + a[4])
			/ ((((b[0] * z + b[1]) * z + b[2]) * z + b[3]) * z + b[4]);
		return 0.5 + y;
	}

	z = ::exp(-y * y / 2) / 2;
	if (y <= 4.0)
	{
		/* evaluate erfc() for sqrt(2)*0.46875 <= |u| <= sqrt(2)*4.0 */
		y = y / 1.4142135623730950488016887242097;
		y =
			((((((((c[0] * y + c[1]) * y + c[2]) * y + c[3]) * y + c[4])
				* y + c[5]) * y + c[6]) * y + c[7]) * y + c[8])
			/ ((((((((d[0] * y + d[1]) * y + d[2]) * y + d[3]) *
				y + d[4]) * y + d[5]) * y + d[6]) * y + d[7]) * y + d[8]);

		y = z*y;
	}
	else
	{
		/* evaluate erfc() for |u| > sqrt(2)*4.0 */
		z = z *  1.4142135623730950488016887242097 / y;
		y = 2 / (y * y);
		y = y * (((((p[0] * y + p[1]) * y + p[2]) * y + p[3])
			* y + p[4]) * y + p[5]) /
			(((((q[0] * y + q[1]) * y + q[2]) * y + q[3]) * y + q[4]) * y + q[5]);
		y = z * (0.564189583547756286948 - y);
	}
	return (u < 0.0 ? y : 1 - y);
}

double  normalQuantile(double p)
{
	double q, t, u;

	static const double a[6] =
	{
		-3.969683028665376e+01,  2.209460984245205e+02,
		-2.759285104469687e+02,  1.383577518672690e+02,
		-3.066479806614716e+01,  2.506628277459239e+00
	};
	static const double b[5] =
	{
		-5.447609879822406e+01,  1.615858368580409e+02,
		-1.556989798598866e+02,  6.680131188771972e+01,
		-1.328068155288572e+01
	};
	static const double c[6] =
	{
		-7.784894002430293e-03, -3.223964580411365e-01,
		-2.400758277161838e+00, -2.549732539343734e+00,
		4.374664141464968e+00,  2.938163982698783e+00
	};
	static const double d[4] =
	{
		7.784695709041462e-03,  3.224671290700398e-01,
		2.445134137142996e+00,  3.754408661907416e+00
	};

	/*	CHECK(!std::isnan(p));
	CHECK(p < 1.0 && p > 0.0);*/

	q = min(p, 1 - p);

	if (q > 0.02425)
	{
		/* Rational approximation for central region. */
		u = q - 0.5;
		t = u * u;
		u = u * (((((a[0] * t + a[1]) * t + a[2]) * t + a[3])
			* t + a[4]) * t + a[5]) /
			(((((b[0] * t + b[1]) * t + b[2]) * t + b[3]) * t + b[4]) * t + 1);
	}
	else
	{
		/* Rational approximation for tail region. */
		t = sqrt(-2 * ::log(q));
		u = (((((c[0] * t + c[1]) * t + c[2]) * t + c[3]) * t + c[4]) * t + c[5])
			/ ((((d[0] * t + d[1]) * t + d[2]) * t + d[3]) * t + 1);
	}

	/* The relative error of the approximation has absolute value less
	than 1.15e-9.  One iteration of Halley's rational method (third
	order) gives full machine precision... */
	t = normalCDF(u) - q;    /* error */
	t = t* 2.506628274631000502415765284811 * ::exp(u * u / 2);   /* f(u)/df(u) */
	u = u - t / (1 + u * t / 2);     /* Halley's method */

	return (p > 0.5 ? -u : u);
}

double chi2inv(double P, unsigned int dim)
{
	assert(P >= 0 && P < 1);
	if (P == 0) return 0;
	else return dim * pow(1.0 - 2.0 / (9 * dim) +
		sqrt(2.0 / (9 * dim))*normalQuantile(P), 3);
}

// =========================================================================================================================
void ComputeObservedPvalues(CGene* G, int GeneNumber, int VoxelNumber, CArray2D<float> &VoxelSNP, CArray2D<float> &VoxelGeneObs)
{
	double sum = 0;

	for (int i = G->GetSnpPos(); i < G->GetSnpPos() + G->GetSnpLen(); i++)
	{
		const float PValue = VoxelSNP(VoxelNumber, i);
		assert((1.0f - PValue) >= 0 && (1.0f - PValue) < 1);

		// I hate to cast the chi2inv as a float, but we are trying to make this app
		// as fast as possible.

		const float ChiSquare = (float)chi2inv(1.0f - PValue, 1);
		sum += ChiSquare;
	}

	VoxelGeneObs(VoxelNumber, GeneNumber) = (float)sum;
	cout << "\tVoxel " << setw(3) << VoxelNumber  << " Gene " << setw(3) << GeneNumber;
	cout << " Snp-Pos " << setw(4) << G->GetSnpPos() << " Snp-Len " << setw(4) << G->GetSnpLen()  <<  " Test Statistic " << sum << endl;
}

void PerformSimulation(CGene *G, float VoxelGeneObservedPValue, int NumberOfIterations, CArray2D<float>& VoxelGeneSim, int v, int g)
{
	G->SetCount(0);

	for (int iter = 0; iter < NumberOfIterations; iter++)
	{
		G->ComputeRandomZ();
		G->TransformZ(VoxelGeneObservedPValue);
	}

	VoxelGeneSim(v, g) = (float)G->GetCount() / (float)NumberOfIterations;
}

void CCPUCmdInterpreter::DoFunctionVegasTest(CSocket* pCS, CMD_t *pCmd)
{
	DLABEL(__FUNCTION__);

	CBaseCmdInterpreter::DoFunctionVegasTest(pCS, pCmd);

	float* pVoxelGeneObserved			= pCmd->C.FunctionVegasTest.VoxelGeneObserved;
	float* pVoxelGeneSim				= pCmd->C.FunctionVegasTest.VoxelGeneSim;
	float* pVoxelSNP					= pCmd->C.FunctionVegasTest.VoxelSNP;
	int *pSNPPosLenPairs				= pCmd->C.FunctionVegasTest.SNPPosLenPairs;
	uint64_t *pLDMatrixList				= pCmd->C.FunctionVegasTest.LDMatrixList;
	const int NumberOfVoxels			= pCmd->C.FunctionVegasTest.NumberOfVoxels;
	const int NumberOfGenes				= pCmd->C.FunctionVegasTest.NumberOfGenes;
	const int NumberOfSNPs				= pCmd->C.FunctionVegasTest.NumberOfSNPs;
	const int NumberOfIterations		= pCmd->C.FunctionVegasTest.NumberOfIterations;
	const int NumberOfSNPPositionLength = pCmd->C.FunctionVegasTest.NumberOfSNPPositionLength;

	//
	// Since a bunch of the pointers refer to 1 and 2 dimensional arrays that
	// were allocated on the server, pass the pointers to some classes that
	// will make it easier to access them, and provide us with an index checking
	// mechanism.
	//
	CArray2D<float> VoxelGeneObserved(NumberOfVoxels, NumberOfGenes, pVoxelGeneObserved);
	CArray2D<float> VoxelGeneSim(NumberOfVoxels, NumberOfGenes, pVoxelGeneSim);
	CArray2D<float> VoxelSNP(NumberOfVoxels, NumberOfSNPs, pVoxelSNP);
	CArray1D<int> SNPPosLenPairs(NumberOfSNPPositionLength, pSNPPosLenPairs);
	CArray1D<uint64_t> LDMatrixList(NumberOfGenes * 2, pLDMatrixList);

	vector<CGene*> GeneList;

	for (int i = 0; i < NumberOfGenes; i++)
	{
		CGene* G = new CGene;

		G->AddSnpPos(SNPPosLenPairs(2 * i), SNPPosLenPairs(2 * i + 1));
		G->AddLDMatrix(LDMatrixList(2 * i), LDMatrixList(2 * i + 1));

		GeneList.push_back(G);
	}

	cout << "ComputeObservedPvalues " << endl;
	for (int g = 0; g < NumberOfGenes; g++)
	{
		for (int v = 0; v < NumberOfVoxels; v++)
		{
			ComputeObservedPvalues(GeneList[g], g, v, VoxelSNP, VoxelGeneObserved);
		}
	}

	// 
	// Perform simulation for all the voxels and genes. OpenMP helps the CPU code here.
	//
	cout << "PerformSimulation " << endl;

	for (int v = 0; v < NumberOfVoxels; v++)
	{
		mutex PrintMutex;
#pragma omp parallel for
		for (int g = 0; g < NumberOfGenes; g++)
		{
			PrintMutex.lock();
			cout << "\tSimulating voxel " << setw(4) << v << " gene " << setw(3) << g << " on TID " << omp_get_thread_num() << endl;
			PrintMutex.unlock();

			PerformSimulation(GeneList[g], VoxelGeneObserved(v, g), NumberOfIterations, VoxelGeneSim, v, g);
		}
	}

	while (!GeneList.empty())
	{
		CGene *G = GeneList.back();
		GeneList.pop_back();
		delete G;
	}

	DTRACE(1 == 1, __FUNCTION__);
	CMD_t outcmd;
	outcmd.errorcode = 0;
	pCS->SendTCPMessage(&outcmd, NULL);
}

