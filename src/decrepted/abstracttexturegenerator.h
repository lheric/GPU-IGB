#ifndef ABSTRACTTEXTUREGENERATOR_H
#define ABSTRACTTEXTUREGENERATOR_H
#include "model/roi.h"

class AbstractTextureGenerator
{
public:
    AbstractTextureGenerator();
    ~AbstractTextureGenerator();

    virtual QString generateTexture(ROI *pcROI, QVector<double>& adTimeSeriesData) = 0;
};

#endif // ABSTRACTTEXTUREGENERATOR_H
