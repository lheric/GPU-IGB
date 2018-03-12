#ifndef COMMONUTILS_H
#define COMMONUTILS_H
#include <QString>
#include <functional>
#include <QVector>
#include <QFile>
#include <QDebug>

class CommonUtils
{
public:
    CommonUtils();
    static bool openFileDialog( QString strCaption,
                                QString strFilter,
                                QString strSaveKey,
                                QString &rstrPath);

    template <class T>
    static bool readCSV(QString strFilename,
                        QVector<QVector<T>>* pcData,
                        std::function< bool (const QString&, T*) > pFuncConverter,
                        T* pMax = nullptr,
                        T* pMin = nullptr)
    {
        QFile cFile(strFilename);
        bool bSuccess = cFile.open(QIODevice::ReadOnly);
        if( !bSuccess )
        {
            qCritical() <<  "Cannot Read File" << strFilename;
            return false;
        }

        pcData->clear();
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

            QVector<T> cMatrixRow;
            T cValue;
            for(int i = 0; i < iLineEleNum; i++)
            {

                bool bOK = pFuncConverter(cList[i], &cValue);
                if(bOK == false)
                {
                    qCritical() << QString("Reading %1 Failed. Non-number Found In Line %2.").arg(strFilename).arg(line);
                    pcData->clear();
                    return false;
                }

                if( pMax != NULL && cValue > *pMax)
                    *pMax = cValue;

                if( pMin != NULL && cValue < *pMin)
                    *pMin = cValue;

                cMatrixRow.append(cValue);

            }
            pcData->append(cMatrixRow);
        }
        cFile.close();

        qDebug() << pcData->size() << "Lines Read. Every Line Has" << iLineEleNum << "Elements.";
        if( pMax != NULL && pMin != NULL)
            qDebug() << "Min:" << *pMin << ", Max:" << *pMax;
        return true;
    }


};

#endif // COMMONUTILS_H
