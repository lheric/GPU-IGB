#include "csvmatrixreader.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <limits>
#include <float.h>

CSVMatrixReader::CSVMatrixReader()
{

}

CSVMatrixReader::~CSVMatrixReader()
{

}

QVector<QVector<double> > CSVMatrixReader::readMatrix(QString strFilename, double* pdMin, double* pdMax)
{

    QVector<QVector<double> > cMatrix;

    QFile cFile(strFilename);
    bool bSuccess = cFile.open(QIODevice::ReadOnly);
    if( !bSuccess )
    {
        qCritical() <<  "Cannot Read File" << strFilename;
        return cMatrix;
    }


    if( pdMin != NULL )
        *pdMin = DBL_MAX;
    if( pdMax != NULL )
        *pdMax = DBL_MIN;

    QTextStream cStream;
    cStream.setDevice(&cFile);
    int line = 0;
    int iLineEleNum = -1;
    while(!cStream.atEnd())
    {
        /// every line in the data file is the data for one ROI
        line++;
        QStringList cList = cStream.readLine().trimmed().split(',');
        if(iLineEleNum == -1)
            iLineEleNum = cList.size();
        if(iLineEleNum != cList.size())
            qCritical() <<  QString("Reading %1 Failed. Element Number is Different For Line %2:\n Others:%3\n Line %2:%4")
                            .arg(strFilename).arg(line).arg(iLineEleNum).arg(cList.size());

        QVector<double> cMatrixRow;
        for(int i = 0; i < iLineEleNum; i++)
        {

            bool bOK;
            double dValue = cList[i].toDouble(&bOK);
            if(bOK == false)
                qCritical() << QString("Reading %1 Failed. Non-number Found In Line %2.").arg(strFilename).arg(line);

            if( pdMax != NULL && dValue > *pdMax)
                *pdMax = dValue;

            if( pdMin != NULL && dValue < *pdMin)
                *pdMin = dValue;

            cMatrixRow.push_back(dValue);

        }
        cMatrix.push_back(cMatrixRow);
    }
    cFile.close();

    qDebug() << cMatrix.size() << "Lines Read. Every Line Has" << iLineEleNum << "Elements.";
    qDebug() << "Min:" << *pdMin << ", Max:" << *pdMax;
    return cMatrix;
}


