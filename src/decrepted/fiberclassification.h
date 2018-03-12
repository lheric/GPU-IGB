#ifndef FIBERCLASSIFICATION_H
#define FIBERCLASSIFICATION_H

#include <QObject>
#include "gitldef.h"
#include "matrix.h"

enum ConnectionType
{
    CONNECT_UNKNOWN = 1,
    CONNECT_REPRODUCIBLE,
    CONNECT_BACKBONE,
    CONNECT_NOISE,
};


class FiberClassification : public QObject
{
    Q_OBJECT
public:
    explicit FiberClassification(QObject *parent = 0);
    ~FiberClassification();

    void init();
    bool openICCFile(QString strFilename);
    bool openVarFile(QString strFilename);
    bool openCountFile(QString strFilename);
    void setICCThreshold(double dICCThreshold);
    void setVarThreshold(double dVarThreshold);
    void setCountThreshold(int iCountThreshold);
    bool queryConnection(int iROILabelStart, int iROILabelEnd);
    void update();
    bool isReady();


    ADD_CLASS_FIELD_NOSETTER(QVector<QVector<double> >, cICC, getICC)
    ADD_CLASS_FIELD(double, dMinICC, getMinICC, setMinICC)
    ADD_CLASS_FIELD(double, dMaxICC, getMaxICC, setMaxICC)

    ADD_CLASS_FIELD_NOSETTER(QVector<QVector<double> >, cVar, getVar)
    ADD_CLASS_FIELD(double, dMinVar, getMinVar, setMinVar)
    ADD_CLASS_FIELD(double, dMaxVar, getMaxVar, setMaxVar)

    ADD_CLASS_FIELD_NOSETTER(QVector<QVector<int> >,    cCount, getCount)
    ADD_CLASS_FIELD(int, iMinCount, getMinCount, setMinCount)
    ADD_CLASS_FIELD(int, iMaxCount, getMaxCount, setMaxCount)

    ADD_CLASS_FIELD_NOSETTER(QVector<QVector<ConnectionType> >, cConnectType, getConnectType)
    ADD_CLASS_FIELD_NOSETTER(double, dICCThreshold, getICCThreshold)
    ADD_CLASS_FIELD_NOSETTER(double, dVarThreshold, getVarThreshold)
    ADD_CLASS_FIELD_NOSETTER(int,    iCountThreshold, getCountThreshold)

    ADD_CLASS_FIELD(bool, bShowReproducible, getShowReproducible, setShowReproducible)
    ADD_CLASS_FIELD(bool, bShowBackbone, getShowBackbone, setShowBackbone)
    ADD_CLASS_FIELD(bool, bShowNoise, getShowNoise, setShowNoise)
    ADD_CLASS_FIELD(bool, bShowUnknown, getShowUnknown, setShowUnknown)

signals:

public slots:
};

#endif // FIBERCLASSIFICATION_H
