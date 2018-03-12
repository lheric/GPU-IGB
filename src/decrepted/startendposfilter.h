#ifndef STARTENDPOSFILTER_H
#define STARTENDPOSFILTER_H
#include <vector>
using namespace std;
class StartEndPosFilter
{
public:
    StartEndPosFilter();
    bool isShorterThan(vector<float>& rcTrk, double dMinLength);
};

#endif // STARTENDPOSFILTER_H
