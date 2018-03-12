#include "fiberconnectionanalyzer.h"
#include "model/fiber.h"
#include "model/labelvolume.h"
#include "vtkutils.h"

FiberConnectionAnalyzer::FiberConnectionAnalyzer()
{

}

int FiberConnectionAnalyzer::AnalyzeConnection(Fiber *pcFiber, LabelVolume *pcLabelVolume)
{
    if(pcFiber == NULL || pcLabelVolume == NULL)
    {
        qDebug() << "Null Parameters. FiberConnectionAnalyzer Will Quit.";
        return -1;
    }

    vtkPolyData* pcTracks =  pcFiber->getReader()->GetOutput();
    vtkImageData* pcLabels = pcLabelVolume->getFlipFilter()->GetOutput();
    QVector<ConnectedROIs>& rcConnInfo = pcFiber->getConnectionInfo()->getFiberConnectionInfo();

    if( pcLabels == NULL || VTKUtils::isEmptyDimension(pcLabels->GetDimensions()))
    {
        qDebug() << "Empty Image. FiberConnectionAnalyzer Will Quit.";
        return -1;
    }
    if( pcTracks == NULL || pcTracks->GetNumberOfLines() == 0)
    {
        qDebug() << "Empty Tracks. FiberConnectionAnalyzer Will Quit.";
        return -1;
    }
    //pcLabels->GetScalarComponentAsFloat()

}
