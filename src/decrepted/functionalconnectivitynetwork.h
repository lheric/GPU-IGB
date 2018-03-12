#ifndef FUNCTIONALCONNECTIVITYNETWORK_H
#define FUNCTIONALCONNECTIVITYNETWORK_H

#include <QObject>
#include <vtkScalarBarActor.h>
#include <vtkLookupTable.h>
#include "gitldef.h"
#include "graymatternetwork.h"

class LabelSurface;
class Subject;
class FunctionalConnectivityData;

class FunctionalConnectivityNetwork : public QObject
{
    Q_OBJECT
public:
    explicit FunctionalConnectivityNetwork(QObject *parent = 0);
    ~FunctionalConnectivityNetwork();
    void init();

    void setLabelSurface(LabelSurface* pcLabelSurface);
    void setFCData(FunctionalConnectivityData* pcFCData);
    void setThreshold(double dThreshold);

    void setVisibility(bool bVisible);
    void addActorsToRenderer(vtkRenderer* pcRenderer);
    void removeActorsFromRenderer(vtkRenderer* pcRenderer);

    void setEnable(bool bEnabled);

protected:
    void xGetConnectionColor(double dValue, unsigned char *auhColor);

signals:
    void updated();

public slots:
    void buildNetwork();

    ADD_CLASS_FIELD_NOSETTER(LabelSurface*, pcLabelSurface, getLabelSurface)
    ADD_CLASS_FIELD_NOSETTER(FunctionalConnectivityData*, pcFCData, getFCData)
    ADD_CLASS_FIELD_NOSETTER(GrayMatterNetwork, cNetwork, getNetwork)


    ADD_CLASS_FIELD_NOSETTER(bool, bEnable, getEnable)

    ADD_CLASS_FIELD(int, iCurveHeight, getCurveHeight, setCurveHeight)
    ADD_CLASS_FIELD_NOSETTER(double, dThreshold, getThreshold)

    /// LUT for positive correlation (from white to red)
    ADD_CLASS_FIELD_NOSETTER(vtkSmartPointer<vtkLookupTable>, pcPosValLUT, getPosValLUT)
    /// LUT for negative correlation (from white to blue)
    ADD_CLASS_FIELD_NOSETTER(vtkSmartPointer<vtkLookupTable>, pcNegValLUT, getNegValLUT)
    /// LUT for ROI center sphere
    ADD_CLASS_FIELD_NOSETTER(vtkSmartPointer<vtkLookupTable>, pcROICentroidLUT, getROICentroidLUT)

    /// LUT scalar bars
    ADD_CLASS_FIELD(vtkSmartPointer<vtkScalarBarActor>, pcPosBarActor, getPosBarActor, setPosBarActor)
    ADD_CLASS_FIELD(vtkSmartPointer<vtkScalarBarActor>, pcNegBarActor, getNegBarActor, setNegBarActor)
};

#endif // FUNCTIONALCONNECTIVITYNETWORK_H
