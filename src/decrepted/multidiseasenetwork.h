#ifndef MULTIDISEASENETWORK_H
#define MULTIDISEASENETWORK_H

#include <QObject>
#include "gitldef.h"
#include "graymatternetwork.h"

class LabelSurface;
class Subject;
class MultiDiseaseConnectivity;

class MultiDiseaseNetwork : public QObject
{
    Q_OBJECT
public:
    explicit MultiDiseaseNetwork(QObject *parent = 0);
    ~MultiDiseaseNetwork();
    void init();

    void setLabelSurface(LabelSurface* pcLabelSurface);
    void setMDConnectivity(MultiDiseaseConnectivity* pcMDConnectivity);
    void setThreshold(double dThreshold);

    void setVisibility(bool bVisible);
    void addActorsToRenderer(vtkRenderer* pcRenderer);
    void removeActorsFromRenderer(vtkRenderer* pcRenderer);

    void setEnable(bool bEnabled);

protected:
    void xGetConnectionColor(QVector<double> &adValues, unsigned char *auhColor);
    bool xBiasCheck(QVector<double> &adValues);

signals:
    void updated();

public slots:
    void buildNetwork();

    ADD_CLASS_FIELD_NOSETTER(LabelSurface*, pcLabelSurface, getLabelSurface)
    ADD_CLASS_FIELD_NOSETTER(MultiDiseaseConnectivity*, pcMDConnectivity, getMDConnectivity)
    ADD_CLASS_FIELD_NOSETTER(GrayMatterNetwork, cNetwork, getNetwork)

    ADD_CLASS_FIELD_NOSETTER(bool, bEnable, getEnable)

    ADD_CLASS_FIELD(int, iCurveHeight, getCurveHeight, setCurveHeight)
    ADD_CLASS_FIELD_NOSETTER(double, dThreshold, getThreshold)

};

#endif // MULTIDISEASENETWORK_H
