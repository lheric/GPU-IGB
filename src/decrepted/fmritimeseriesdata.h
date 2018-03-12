#ifndef FMRITIMESERIESDATA_H
#define FMRITIMESERIESDATA_H
#include "gitldef.h"
#include "model/matrix.h"
#include <QString>
#include <QObject>

class FMRITimeSeriesData: public QObject
{
    Q_OBJECT
public:
    FMRITimeSeriesData(QObject *parent = 0);
    ~FMRITimeSeriesData();


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


    ADD_CLASS_FIELD_NOSETTER(Matrix<double>, cMatrix, getMatrix)


signals:
    void updated();
};

#endif // FMRITIMESERIESDATA_H
