#ifndef STRIPETEXTUREGENERATOR_H
#define STRIPETEXTUREGENERATOR_H
#include "abstracttexturegenerator.h"

class StripeTextureGenerator : public AbstractTextureGenerator
{
public:
    StripeTextureGenerator();
    ~StripeTextureGenerator();

    QString generateTexture(ROI *pcROI, QVector<double>& adTimeSeriesData);
};

#endif // STRIPETEXTUREGENERATOR_H
