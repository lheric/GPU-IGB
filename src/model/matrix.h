#ifndef MATRIX_H
#define MATRIX_H
#include <QVector>
#include <QString>
#include <QFile>
#include <QDebug>
#include <QSize>
#include <float.h>
#include "gitldef.h"

template <typename T>
class Matrix
{
public:
    Matrix();
    ~Matrix();

    void init();

    bool readFromCSV(QString strFilename, T (*pf)(const QString& strElement));
    bool writeToCSV(QString strFilename);
    QSize getSize();
    bool isSquare();
    bool isEmpty();
    void allocate(int iNumRow, int iNumCol);

    QVector<T>& operator[] (int i);

    ADD_CLASS_FIELD_NOSETTER(QVector<QVector<T> >, cMatrix, getMatrix)
    ADD_CLASS_FIELD_NOSETTER(T, dMax, getMax)
    ADD_CLASS_FIELD_NOSETTER(T, dMin, getMin)
    ADD_CLASS_FIELD_NOSETTER(int, iNumRow, getNumRow)
    ADD_CLASS_FIELD_NOSETTER(int, iNumCol, getNumCol)
};

template <typename T>
Matrix<T>::Matrix()
{
    init();
}

template <typename T>
Matrix<T>::~Matrix()
{

}

template <typename T>
void Matrix<T>::init()
{
    m_iNumCol = m_iNumRow = 0;
    m_cMatrix.clear();
}

template <typename T>
bool Matrix<T>::readFromCSV(QString strFilename, T (*pf)(const QString& strElement))
{

    init();

    QFile cFile(strFilename);
    bool bSuccess = cFile.open(QIODevice::ReadOnly);
    if( !bSuccess )
    {
        qCritical() <<  "Cannot Read File" << strFilename;
        return false;
    }

    QTextStream cStream;
    cStream.setDevice(&cFile);
    int line = 0;
    int iLineEleNum = 0;
    while(!cStream.atEnd())
    {
        QStringList cList = cStream.readLine().trimmed().split(',');
        if(iLineEleNum == 0)
            iLineEleNum = cList.size();
        if(iLineEleNum != cList.size())
            qCritical() <<  QString("Reading %1 Failed. Element Number is Different For Line %2:\n Others:%3\n Line %2:%4")
                            .arg(strFilename).arg(line).arg(iLineEleNum).arg(cList.size());

        QVector<T> cMatrixRow;
        for(int i = 0; i < iLineEleNum; i++)
        {

            T dValue = pf(cList[i]);

            if( i == 0 && line == 0 )
            {
                m_dMax = m_dMin = dValue;
            }
            else
            {
                if( dValue > m_dMax)
                    m_dMax = dValue;

                if( dValue < m_dMin)
                    m_dMin = dValue;
            }

            cMatrixRow.push_back(dValue);

        }
        m_cMatrix.push_back(cMatrixRow);

        line++;
    }
    cFile.close();

    m_iNumRow = m_cMatrix.size();
    m_iNumCol = iLineEleNum;

    qDebug() << m_iNumRow << "X" << m_iNumCol << "Matrix Read.";
    qDebug() << "Min:" << m_dMin << ", Max:" << m_dMax;
    return true;
}

template <typename T>
bool Matrix<T>::writeToCSV(QString strFilename)
{
    QFile cFile(strFilename);
    bool bSuccess = cFile.open(QIODevice::WriteOnly);
    if( !bSuccess )
    {
        qCritical() <<  "Cannot Write File" << strFilename;
        return false;
    }

    QTextStream cStream;
    cStream.setDevice(&cFile);
    for(int i = 0; i < m_iNumRow; i++)
    {
        for(int j = 0; j < m_iNumCol; j++)
        {
            cStream << m_cMatrix[i][j];

            if( j != m_iNumCol-1)
                cStream << ",";
        }
        cStream << endl;
    }
    cFile.close();
    return true;
}

template <typename T>
QSize Matrix<T>::getSize()
{
    return QSize(getNumRow(), getNumCol());
}

template <typename T>
bool Matrix<T>::isSquare()
{
    return getNumRow() == getNumCol();
}


template <typename T>
bool Matrix<T>::isEmpty()
{
    return getNumRow() == 0 || getNumCol() == 0;
}

template <typename T>
void Matrix<T>::allocate(int iNumRow, int iNumCol)
{
    this->getMatrix().clear();
    QVector<T> cRow;
    cRow.resize(iNumCol);
    for(int i = 0; i < iNumRow; i++)
    {
        this->getMatrix().push_back(cRow);
    }
    this->m_iNumRow = iNumRow;
    this->m_iNumCol = iNumCol;
}



template <typename T>
QVector<T>& Matrix<T>::operator[](int i)
{
    return this->m_cMatrix[i];
}

#endif // MATRIX_H
