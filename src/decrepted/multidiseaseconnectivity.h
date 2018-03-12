#ifndef MULTIDISEASECONNECTIVITY_H
#define MULTIDISEASECONNECTIVITY_H

#include "matrix.h"
#include "gitldef.h"
#include <QVector>
#include <QObject>

/**
 * @brief The MultiDiseaseConnectivity class
 *
 */
class MultiDiseaseConnectivity: public QObject
{
    Q_OBJECT
public:
    MultiDiseaseConnectivity(QObject *parent = 0);
    ~MultiDiseaseConnectivity();

    /**
     * @brief openMultiDiseaseConnectivity
     * For n disease and m ROIs, it should be a (m*n) X m matrix
     * -- the concatenating of the connectivity matrices for n diseases.
     * @param strFilename
     * @return
     */
    bool readFromCSV(const QString& strFilename);


    /// store it in a m X m matrix, every element is an array of n float
    ADD_CLASS_FIELD_NOSETTER( Matrix< QVector<double> >, cMatrix, getMatrix )

signals:
    void updated();
};

#endif // MULTIDISEASECONNECTIVITY_H
