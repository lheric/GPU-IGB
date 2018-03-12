#ifndef CIRCLETEXTUREGENERATOR_H
#define CIRCLETEXTUREGENERATOR_H
#include <QString>
#include "model/roi.h"
#include "abstracttexturegenerator.h"

class CircleTextureGenerator : public AbstractTextureGenerator
{
public:
    CircleTextureGenerator();
    ~CircleTextureGenerator();

    virtual QString generateTexture(ROI *pcROI, QVector<double>& adTimeSeriesData);
};

#endif // CIRCLETEXTUREGENERATOR_H
