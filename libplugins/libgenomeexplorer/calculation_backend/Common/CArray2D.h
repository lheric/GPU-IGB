// ========================================================================================
//	CArray2D.h - template for the CArray2D and CArray2D.
//
// A 2D array class with 0 based indexing for each dimension.
// The data is this class is stored in row major order.
// ========================================================================================

#pragma once

//
// CArray2D Class
//
template <class T>
class CArray2D
{
public:
	CArray2D(unsigned int y, unsigned int x, bool initflag = true, T init = 0);
	CArray2D();
	CArray2D(unsigned int y, unsigned int x, T* p);
	virtual ~CArray2D();

	void Create(unsigned int y, unsigned int x, bool initflag = true, T init = 0);
	T* GetHostDataPtr(void) const { return m_pHostData; }

	T& operator()(unsigned int y, unsigned int x);

	unsigned int GetHeight(void) const { return m_Height; }
	unsigned int GetWidth(void) const { return m_Width; }
	unsigned int NumberOfElements(void);
	unsigned int SizeOfMemory(void);

protected:
	unsigned int m_Width;
	unsigned int m_Height;

	T*  m_pHostData;
	bool m_Destruct;			// Indicates whether the destructor should free RAM
};

// ========================================================================================
// Constructor
// ========================================================================================
template <class T>
CArray2D<T>::CArray2D(unsigned int y, unsigned int x, bool initflag, T init) :
	m_Width(x),
	m_Height(y),
	m_Destruct(true)
{
	unsigned int NumEle = NumberOfElements();

	m_pHostData = new T[NumEle];

	if (initflag)
		for (unsigned int i = 0; i < NumEle; i++)
			m_pHostData[i] = init;
}

template <class T>
CArray2D<T>::CArray2D() :
	m_Width(0),
	m_Height(0),
	m_pHostData(0),
	m_Destruct(true)
{
}

template <class T>
CArray2D<T>::CArray2D(unsigned int y, unsigned int x, T* p) :
	m_Width(x),
	m_Height(y),
	m_pHostData(p),
	m_Destruct(false)
{
}


// ========================================================================================
// Destructor
// ========================================================================================
template <class T>
CArray2D<T>::~CArray2D()
{
	if (m_Destruct)
		delete[] m_pHostData;
}

// ========================================================================================
// Create method
// ========================================================================================
template <class T>
void CArray2D<T>::Create(unsigned int y, unsigned int x, bool initflag, T init)
{
	if (m_Destruct == false)
	{
		char Msg[256];
		strcpy_s(Msg, sizeof(Msg), "CArray2D<T>::Create called on object with Destruct == False");
		throw(Msg);
	}

	delete[] m_pHostData;

	m_Height = y;
	m_Width = x;

	unsigned int NumEle = NumberOfElements();

	m_pHostData = new T[NumEle];

	if (initflag)
		for (unsigned int i = 0; i < NumEle; i++)
			m_pHostData[i] = init;
}


// ========================================================================================
// Indexing method
//
// Note : The debug build of the app will throw an exception if the index
//        bounds are violated.
// ========================================================================================

template <class T>
T& CArray2D<T>::operator()(unsigned int y, unsigned int x)
{
#ifdef DEBUG_INDEXING
	char Msg[256];

	if (y < 0 || y >= m_Height)
	{
		sprintf_s(Msg, sizeof(Msg), "CArray2D Y indexing error, y=%d, y max=%d", y, m_Height);
		throw(Msg);
	}

	if (x < 0 || x >= m_Width)
	{
		sprintf_s(Msg, sizeof(Msg), "CArray2D X indexing error, x=%d, x max=%d", x, m_Width);
		throw(Msg);
	}
#endif

	return m_pHostData[(x)+m_Width*(y)];
}


//
// Helper methods
//
template <class T>
unsigned int CArray2D<T>::SizeOfMemory()
{
	unsigned int Q = NumberOfElements() * sizeof(T);

	if (Q > (1U << 31) - 1U)
		throw "CArray2D<T>::SizeOfMemory() maximum memory size exceeded.";

	return Q;
}

template <class T>
unsigned int CArray2D<T>::NumberOfElements()
{
	unsigned int v = m_Width*m_Height;

#ifdef DEBUG_INDEXING
	char Msg[256];

	if (v < 0)
	{
		sprintf_s(Msg, sizeof(Msg), "CArray2D::NumberOfElements is < 0, Height=%d, Width=%d", m_Height, m_Width);
		throw(Msg);
	}
#endif
	return v;
}


