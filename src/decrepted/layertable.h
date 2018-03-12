#ifndef ROILAYERTABLE_H
#define ROILAYERTABLE_H

#include <QObject>
#include <QMap>
#include "model/matrix.h"

class Ray;

class LayerTable : public QObject
{
    Q_OBJECT
public:
    explicit LayerTable(QObject *parent = 0);
    ~LayerTable();

    void resize(int iNumLabel);
    void analyzeRay(Ray* pcRay);
    int getLayerForLabel(int iLabel, double* pdPercent = NULL);
//    void buildTable();
    void print();
    void hightlightLayer(int iLayer = -1);

    ADD_CLASS_FIELD_NOSETTER(Matrix<int>, cMatrix, getMatrix)
    ADD_CLASS_FIELD_NOSETTER(CONCATE(QMap<int, int>), cLabelToLayer, getLabelToLayer)
    ADD_CLASS_FIELD_NOSETTER(CONCATE(QVector<QSet<int>>), cLayerToLabel, getLayerToLabel)


signals:

public slots:
};

#endif // ROILAYERTABLE_H
