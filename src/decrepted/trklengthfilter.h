#ifndef TRKLENGTHFILTER_H
#define TRKLENGTHFILTER_H
#include <vector>
using namespace std;
class TrkLengthFilter
{
public:
    TrkLengthFilter();
    bool isShorterThan(vector<float>& rcTrk, double dMinLength);
};

#endif // TRKLENGTHFILTER_H
