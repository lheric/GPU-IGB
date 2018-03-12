#include "graymatternetwork.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QColor>
#include <vtkPolyLine.h>
#include <vtkUnsignedCharArray.h>
#include <vtkProperty.h>
#include <vtkTextProperty.h>
#include <vtkSphereSource.h>
#include <vtkAppendPolyData.h>
#include "model/labelsurface.h"
#include "algorithm/colorconverter.h"
#include "algorithm/csvmatrixreader.h"


GrayMatterNetwork::GrayMatterNetwork(QObject *parent) : QObject(parent)
{
    init();
}

GrayMatterNetwork::~GrayMatterNetwork()
{

}

void GrayMatterNetwork::init()
{

    m_bUseBezierCurve = false;
    m_pcPolyData = vtkPolyData::New();
    m_pcLines = vtkCellArray::New();
    m_pcPoints = vtkPoints::New();
    m_pcLineColors = vtkUnsignedCharArray::New();
    m_pcTubeFilter = vtkTubeFilter::New();
    m_pcMapper = vtkPolyDataMapper::New();
    m_pcActor = vtkActor::New();


    this->getTubeFilter()->SetInputData(m_pcPolyData);
    this->getMapper()->SetInputConnection(this->getTubeFilter()->GetOutputPort());
    this->getActor()->SetMapper(this->getMapper());
    this->getActor()->GetProperty()->SetOpacity(0.5);


    this->getTubeFilter()->SetNumberOfSides(10);
    this->getTubeFilter()->SetRadius(0.5);
    this->getMapper()->ScalarVisibilityOn();


    m_pcROICenterPolyData = vtkPolyData::New();
    m_pcROICenterMapper = vtkPolyDataMapper::New();
    m_pcROICenterActor = vtkActor::New();
    this->getROICenterMapper()->SetInputData(m_pcROICenterPolyData);
    this->getROICenterActor()->SetMapper(this->getROICenterMapper());
}

void GrayMatterNetwork::buildNetwork()
{


    /// clear
    m_pcPolyData->Initialize();
    m_pcLines->Initialize();
    m_pcPoints->Initialize();
    m_pcLineColors->Initialize();
    m_pcLineColors->SetNumberOfComponents(4);
    m_pcLineColors->SetName("Colors");

    /// check
    if(getColorMatrix().getSize() != getCtrlPntMatrix().getSize())
    {
        qWarning() << "Color matrix and ctrl point matrix sizes do not match";
        qWarning() << "Color Matrix" << getColorMatrix().getSize();
        qWarning() << "Ctrl point matrix" << getCtrlPntMatrix().getSize();
        return;
    }

    if(getCtrlPntMatrix().isEmpty())
    {
        qWarning() << "Ctrl point matrix is empty";
        return;
    }

    if(!getCtrlPntMatrix().isSquare())
    {
        qWarning() << "Ctrl point matrix is non-square";
        return;
    }


    int iNumROI = getCtrlPntMatrix().getNumCol();

    qDebug() << "Building Network for" << iNumROI << "ROIs";

    xAddROICenterPoint();   /// add shpere for each ROI center

    QVector<QVector3D> cCtrlPoints;
    for(int iStartIdx = 0; iStartIdx < iNumROI-1; iStartIdx++)
    {

        for(int iEndIdx = iStartIdx+1; iEndIdx < iNumROI; iEndIdx++)
        {
            cCtrlPoints.clear();
            cCtrlPoints = getCtrlPntMatrix()[iStartIdx][iEndIdx];

            if(cCtrlPoints.empty())
                continue;

//            qDebug() << cCtrlPoints.size();
            QVector<QVector3D> cBezierCurve;
            if( getUseBezierCurve() )
                cBezierCurve = xBezierPoints(cCtrlPoints);
            else
            {
                cBezierCurve.push_back(cCtrlPoints.at(0));
                cBezierCurve.push_back(cCtrlPoints.at(cCtrlPoints.size()-1));
            }
            vtkSmartPointer<vtkPolyLine> pcPolyLine = vtkPolyLine::New();
            pcPolyLine->GetPointIds()->SetNumberOfIds(cBezierCurve.size());

            for(int iPntIdx = 0; iPntIdx < cBezierCurve.size(); iPntIdx++) {
                vtkIdType iInsertedIdx = m_pcPoints->InsertNextPoint(cBezierCurve[iPntIdx].x(),
                                                                     cBezierCurve[iPntIdx].y(),
                                                                     cBezierCurve[iPntIdx].z());
                pcPolyLine->GetPointIds()->SetId(iPntIdx, iInsertedIdx);
            }
            m_pcLines->InsertNextCell(pcPolyLine);
            pcPolyLine->Delete();

            /// set color for this curve
            static unsigned char auhCurveColor[4];
            QColor cColor = this->getColorMatrix()[iStartIdx][iEndIdx];
            auhCurveColor[0] = cColor.red();
            auhCurveColor[1] = cColor.green();
            auhCurveColor[2] = cColor.blue();
            auhCurveColor[3] = 255;
            m_pcLineColors->InsertNextTupleValue(auhCurveColor);

        }

    }

    // Add the points to the dataset
    m_pcPolyData->SetPoints(m_pcPoints);
    // Add the lines to the dataset
    m_pcPolyData->SetLines(m_pcLines);
    // Add the colors
    m_pcPolyData->GetCellData()->SetScalars(m_pcLineColors);

    m_pcPolyData->Modified();
    m_pcTubeFilter->Update();

    /// apply color array
    this->getMapper()->SetScalarModeToUseCellFieldData();
    this->getMapper()->SelectColorArray("Colors");
    this->getMapper()->ScalarVisibilityOn();
    this->getMapper()->Modified();
    this->getMapper()->Update();
}

void GrayMatterNetwork::setVisibility(bool bVisible)
{
    this->getActor()->SetVisibility(bVisible);
    this->getROICenterActor()->SetVisibility(bVisible);
}

void GrayMatterNetwork::addActorsToRenderer(vtkRenderer *pcRenderer)
{
    pcRenderer->AddActor(this->getActor());
    pcRenderer->AddActor(this->getROICenterActor());
}

void GrayMatterNetwork::removeActorsFromRenderer(vtkRenderer *pcRenderer)
{
    pcRenderer->RemoveActor(this->getActor());
    pcRenderer->RemoveActor(this->getROICenterActor());
}

void GrayMatterNetwork::xAddROICenterPoint()
{
    this->getROICenterPolyData()->Initialize();
    vtkSmartPointer<vtkSphereSource> pcSource = vtkSmartPointer<vtkSphereSource>::New();
    vtkSmartPointer<vtkAppendPolyData> pcAppendFilter = vtkSmartPointer<vtkAppendPolyData>::New();
    pcAppendFilter->AddInputData(this->getROICenterPolyData());
    pcAppendFilter->AddInputData(pcSource->GetOutput());

    int iNumROI = this->getColorMatrix().getNumCol();
    for(int i = 0; i < iNumROI; i++)
    {
        if(this->getCtrlPntMatrix()[i][i].empty())
        {
            qWarning() << "ROI #" << i << "center point is missing. Skipped.";
            continue;
        }
        QVector3D& cCenter = this->getCtrlPntMatrix()[i][i][0];
        pcSource->SetCenter(cCenter[0], cCenter[1], cCenter[2]);
        pcSource->SetRadius(3.0);
        pcSource->Update();

        vtkSmartPointer<vtkUnsignedCharArray> auhColor = vtkSmartPointer<vtkUnsignedCharArray>::New();
        auhColor->SetNumberOfComponents(3);
        auhColor->SetName("Colors");
        QColor& cColor = this->getColorMatrix()[i][i];
        for(int k = 0; k < pcSource->GetOutput()->GetNumberOfCells(); k++)
            auhColor->InsertNextTuple3(cColor.red(), cColor.green(), cColor.blue());
        pcSource->GetOutput()->GetCellData()->SetScalars(auhColor);

        pcAppendFilter->Update();
        this->getROICenterPolyData()->DeepCopy(pcAppendFilter->GetOutput());
    }



    this->getROICenterPolyData()->Modified();
    this->getROICenterMapper()->Update();
}


QVector<QVector3D> GrayMatterNetwork::xBezierPoints(const QVector<QVector3D> &racPoints, int iCurLevel, int iMaxLevel)
{
    if( iCurLevel >= iMaxLevel )
        return racPoints;

    QVector<QVector3D> acLeft, acRight;
    QVector<QVector3D> acResult;
    QVector<QVector3D> acPre = racPoints;
    while(acPre.size() > 0)
    {
        acResult.clear();
        for( int i = 1; i < acPre.size(); i++ )
        {
            acResult.push_back( (acPre[i-1]+acPre[i])/2 );
        }
        acLeft.push_back(acPre[0]);
        acRight.push_front(acPre[acPre.size()-1]);
        acPre = acResult;
    }

    QVector<QVector3D> cLeftCurve = xBezierPoints(acLeft, iCurLevel+1, iMaxLevel);
    cLeftCurve.pop_back();  /// remove the duplicated shared point of left and right part

    return  cLeftCurve + xBezierPoints(acRight, iCurLevel+1, iMaxLevel);
}
