#include "trklengthfilter.h"

#include <qmath.h>
TrkLengthFilter::TrkLengthFilter()
{
}

bool TrkLengthFilter::isShorterThan(vector<float> &rcTrk, double dMinLength)
{
    size_t iTotalPnt = rcTrk.size()/3;
    double dTrkLength = 0.0;
    for( size_t i = 1; i < iTotalPnt; i++ )
    {
        dTrkLength += qAbs(rcTrk[3*i]-rcTrk[3*i-3]) + qAbs(rcTrk[3*i+1]-rcTrk[3*i-2]) + qAbs(rcTrk[3*i+2]-rcTrk[3*i-1]);
        if( dMinLength < dTrkLength )
            return false;
    }

    return true;

}
