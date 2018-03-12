#ifndef GRAYMATTERNETWORK_H
#define GRAYMATTERNETWORK_H
#include "gitldef.h"
#include <QObject>
#include <QVector>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkPoints.h>
#include <vtkCellArray.h>
#include <vtkUnsignedCharArray.h>
#include <vtkTubeFilter.h>
#include <QColor>
#include <QVector3D>
#include "matrix.h"


/**
 * @brief The GrayMatterNetwork class
 * the network across surface representing the connectivity between ROIs
 */
class GrayMatterNetwork: public QObject
{
    Q_OBJECT
public:
    explicit GrayMatterNetwork(QObject *parent = 0);
    ~GrayMatterNetwork();

    void init();
    void buildNetwork();
    void setVisibility(bool bVisible);
    void addActorsToRenderer(vtkRenderer* pcRenderer);
    void removeActorsFromRenderer(vtkRenderer* pcRenderer);


protected:
    void xAddROICenterPoint();
    QVector<QVector3D> xBezierPoints(const QVector<QVector3D> &racPoints, int iCurLevel = 0, int iMaxLevel = 5);

    ///
    ADD_CLASS_FIELD(Matrix<QVector<QVector3D>>, cCtrlPntsMatrix, getCtrlPntMatrix, setCtrlPntMatrix)
    ADD_CLASS_FIELD(Matrix<QColor>, cColorMatrix, getColorMatrix, setColorMatrix)

    /// use bezier curve over the brain to represent the connection between ROIs
    ADD_CLASS_FIELD(bool, bUseBezierCurve, getUseBezierCurve, setUseBezierCurve)

    ADD_CLASS_FIELD(vtkSmartPointer<vtkCellArray>, pcLines, getLines, setLines)
    ADD_CLASS_FIELD(vtkSmartPointer<vtkPoints>, pcPoints, getPoints, setPoints)
    ADD_CLASS_FIELD(vtkSmartPointer<vtkUnsignedCharArray>, pcLineColors, getLineColors, setLineColors)
    ADD_CLASS_FIELD(vtkSmartPointer<vtkPolyData>, pcPolyData, getPolyData, setPolyData)
    ADD_CLASS_FIELD(vtkSmartPointer<vtkTubeFilter>, pcTubeFilter, getTubeFilter, setTubeFilter)
    ADD_CLASS_FIELD(vtkSmartPointer<vtkPolyDataMapper>, pcMapper, getMapper, setMapper)
    ADD_CLASS_FIELD(vtkSmartPointer<vtkActor>, pcActor, getActor, setActor)


    ADD_CLASS_FIELD(vtkSmartPointer<vtkPolyData>, pcROICenterPolyData, getROICenterPolyData, setROICenterPolyData)
    ADD_CLASS_FIELD(vtkSmartPointer<vtkPolyDataMapper>, pcROICenterMapper, getROICenterMapper, setROICenterMapper)
    ADD_CLASS_FIELD(vtkSmartPointer<vtkActor>, pcROICenterActor, getROICenterActor, setROICenterActor)



};

#endif // GRAYMATTERNETWORK_H
