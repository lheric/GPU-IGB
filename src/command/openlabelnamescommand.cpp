#include "openlabelnamescommand.h"
#include "model/modellocator.h"
#include "gitlivkcmdevt.h"
#include "gitlupdateuievt.h"
#include <vtkCellLocator.h>
#include <vtkPointData.h>
#include "model/rom.h"
OpenLabelNamesCommand::OpenLabelNamesCommand(QObject *parent) : GitlAbstractCommand(parent)
{
    setInWorkerThread(false);
}

bool OpenLabelNamesCommand::execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg)
{    
    /// read parcellation image
    QString strFilename = rcInputArg.getParameter("filename").toString();
    ModelLocator* pcModel = ModelLocator::getInstance();
    Subject *pcSubject = &pcModel->getSubject();

    pcSubject->getGrayMat().getLabelNames().loadParcellation(strFilename);
    pcModel->getRenderWindow()->Render();
    pcModel->getRenderer()->ResetCamera();

    /// update the filename and ROI list on GUI
    rcOutputArg.setParameter("ROI_names_file", strFilename);
    rcOutputArg.setParameter("ROI_names", QVariant::fromValue<void*>(&pcSubject->getGrayMat().getLabelNames().getLabelNamesTable()));
    rcOutputArg.setParameter("ROI_color_scheme", QVariant::fromValue<void*>(&pcSubject->getGrayMat().getColorScheme().getColorMap()));
//    qDebug() << pcSubject->getGrayMat().getColorScheme().getColorMap().size();
    return true;
}

