#ifndef CONTOURTEXTUREGENERATOR_H
#define CONTOURTEXTUREGENERATOR_H
#include "abstracttexturegenerator.h"
#include <QColor>
#include <QVector>
class ContourTextureGenerator : public AbstractTextureGenerator
{
public:
    ContourTextureGenerator();
    ~ContourTextureGenerator();

    virtual QString generateTexture(ROI *pcROI, QVector<double>& adTimeSeriesData);
    bool generateTexture(QString strFilePath, QVector<double>& adTimeSeriesData, ROI *pcROI, double dMin, double dMax);
    bool generateTexture(QImage cInputImg, QVector<double>& adTimeSeriesData, ROI *pcROI, double dMin, double dMax, QPointF *pcCenter);


};

#endif // CONTOURTEXTUREGENERATOR_H
