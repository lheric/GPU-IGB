#include "directionalcolor.h"
#include <qmath.h>
DirectionalColor::DirectionalColor()
{
}

void DirectionalColor::convertToColor(double *adStartPos, double *adEndPos, unsigned char *auhColor)
{

    double dSum = 0.0;
    static double adColor[3];
    for(int i = 0; i < 3; i++)
    {
        adColor[i] = qAbs(adStartPos[i]-adEndPos[i]);
        dSum += adColor[i];
    }

    for(int i = 0; i < 3; i++)
    {
        auhColor[i] = qRound((adColor[i]/dSum)*255.0);
    }
    auhColor[3] = 100;    //alpha
}
