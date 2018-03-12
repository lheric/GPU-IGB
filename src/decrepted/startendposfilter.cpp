#include "startendposfilter.h"
#include <qmath.h>
StartEndPosFilter::StartEndPosFilter()
{
}

bool StartEndPosFilter::isShorterThan(vector<float> &rcTrk, double dMinLength)
{
    float* pdStartPos = &rcTrk[0];
    float* pdEndPos = &rcTrk[rcTrk.size()-3];
    double dDist = qAbs(pdStartPos[0]-pdEndPos[0]) + qAbs(pdStartPos[1]-pdEndPos[1]) + qAbs(pdStartPos[2]-pdEndPos[2]);
    return dDist < dMinLength;
}
