#ifndef MULTIDISEASEMEASUREMENT_H
#define MULTIDISEASEMEASUREMENT_H

#include <QObject>
#include <QVector>
#include <vtkSmartPointer.h>
#include "matrix.h"
#include "gitldef.h"

/**
 * @brief The MultiDiseaseMeasurement class Every row is a ROI. Every col is a disease.
 */

class MultiDiseaseMeasurement : public QObject
{
    Q_OBJECT
public:
    explicit MultiDiseaseMeasurement(QObject *parent = 0);

    void init();

    bool openMutiDiseasMeasurement(const QString& strFilename);

    QVector<double> getMutiDiseaseDataForROI(int iROILabel);

    int getNumROI() { return m_cMatrix.getNumRow(); }


    int getNumDisease() { return m_cMatrix.getNumCol(); }

    ADD_CLASS_FIELD_NOSETTER( Matrix<double>, cMatrix, getMatrix)

signals:
    void updated();

public slots:
};

#endif // MULTIDISEASEMEASUREMENT_H
