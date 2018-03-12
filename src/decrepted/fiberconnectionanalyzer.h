#ifndef FIBERCONNECTIONANALYZER_H
#define FIBERCONNECTIONANALYZER_H

class Fiber;
class LabelVolume;

class FiberConnectionAnalyzer
{
public:
    FiberConnectionAnalyzer();
    int AnalyzeConnection(Fiber* pcFiber, LabelVolume* pcLabelVolume);
};

#endif // FIBERCONNECTIONANALYZER_H
