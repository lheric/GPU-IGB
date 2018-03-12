// ========================================================================================
//	CArray1D.h - template for the CArray1D and CArray1D classes
//
// A 1D array class with 0 based indexing for each dimension.
// ========================================================================================

#pragma once

template <class T>
class CArray1D
{
public:
	CArray1D(unsigned int x, bool initflag = true, T init = 0);
	CArray1D();
	CArray1D(unsigned int x, T* p);
	virtual ~CArray1D();

	virtual void Create(unsigned int x, bool initflag = true, T init = 0);
	virtual void Initialize(T init);
	virtual T& operator()(unsigned int x);
	virtual unsigned int GetNumElements(void) const { return m_xdim; }
	unsigned int SizeOfMemory(void);
	T* GetCPUDataPtr(void) const { return m_pHostData; }

protected:
	unsigned int	NumberOfElements(void);
	unsigned int	m_xdim;
	T*				m_pHostData;
	bool			m_Destruct;			// Indicates whether the destructor should free RAM
};

// ========================================================================================
// Constructor
// ========================================================================================
template <class T>
CArray1D<T>::CArray1D(unsigned int x, bool initflag, T init) :
	m_xdim(x),
	m_Destruct(true)
{
	int NumEle = NumberOfElements();

	m_pHostData = new T[NumEle];

	if (initflag)
		for (int i = 0; i < NumEle; i++)
			m_pHostData[i] = init;
}

template <class T>
CArray1D<T>::CArray1D() :
	m_xdim(0),
	m_pHostData(0),
	m_Destruct(true)
{
}

template <class T>
CArray1D<T>::CArray1D(unsigned int x, T* p) :
	m_xdim(x),
	m_pHostData(p),
	m_Destruct(false)
{
}

// ========================================================================================
// Destructor
// ========================================================================================
template <class T>
CArray1D<T>::~CArray1D()
{
	if (m_Destruct)
		delete[] m_pHostData;
}

// ========================================================================================
// Create method
// ========================================================================================
template <class T>
void CArray1D<T>::Create(unsigned int x, bool initflag, T init)
{
	delete[] m_pHostData;

	m_xdim = x;

	unsigned int NumEle = NumberOfElements();

	m_pHostData = new T[NumEle];

	if (initflag)
		for (unsigned int i = 0; i < NumEle; i++)
			m_pHostData[i] = init;
}


// ========================================================================================
// Indexing method
//
// Note : The debug build of the app will throw an exception if the indexing
//        bounding are violated.
// ========================================================================================

template <class T>
T& CArray1D<T>::operator()(unsigned int x)
{
#ifdef DEBUG_INDEXING
	char Msg[256];

	if (x < 0 || x >= m_xdim)
	{
		sprintf_s(Msg, sizeof(Msg), "CArray1D indexing error x=%d, xdim=%d", x, m_xdim);
		throw(Msg);
	}
#endif
	return m_pHostData[x];
}

template <class T>
void CArray1D<T>::Initialize(T init)
{
	unsigned int NumEle = NumberOfElements();

	T *p = m_pHostData;

	for (unsigned int i = 0; i < NumEle; i++)
		*p++ = init;
}

//
// Helper methods
//
template <class T>
unsigned int CArray1D<T>::SizeOfMemory()
{
	unsigned int Q = NumberOfElements() * sizeof(T);

	if (Q > (1U << 31) - 1U)
		throw "CArray1D<T>::SizeOfMemory() maximum memory size exceeded.";

	return Q;
}

template <class T>
unsigned int CArray1D<T>::NumberOfElements()
{
	return  m_xdim;
}

