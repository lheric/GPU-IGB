#ifndef FUNCTIONALCONNECTIVITYDATA_H
#define FUNCTIONALCONNECTIVITYDATA_H

#include <QObject>
#include "gitldef.h"
#include "model/matrix.h"
#include "model/graymatternetwork.h"


class FunctionalConnectivityData: public QObject
{
    Q_OBJECT
public:
    FunctionalConnectivityData(QObject *parent = 0);
    ~FunctionalConnectivityData();

    /**
     * @brief init
     */
    void init()
    {
        m_cMatrix.init();
    }

    /**
     * @brief readFromCSV
     * @param strFilename
     * @return
     */
    bool readFromCSV(const QString &strFilename)
    {
        if(m_cMatrix.readFromCSV(strFilename, [](const QString& strRead){return strRead.toDouble();}) == false)
            return false;
        emit updated();
        return true;
    }

signals:
    void updated();


    ADD_CLASS_FIELD_NOSETTER(Matrix<double>, cMatrix, getMatrix)

};

#endif // FUNCTIONALCONNECTIVITYDATA_H
