#include "opengraymatterparcelcommand.h"
#include "model/modellocator.h"
#include "gitlivkcmdevt.h"
#include "gitlupdateuievt.h"
#include <vtkCellLocator.h>
#include <vtkPointData.h>
#include "model/rom.h"
OpenGrayMatterParcelCommand::OpenGrayMatterParcelCommand(QObject *parent) : GitlAbstractCommand(parent)
{
    setInWorkerThread(false);
}

bool OpenGrayMatterParcelCommand::execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg)
{    
    /// read parcellation image
    QString strFilename = rcInputArg.getParameter("filename").toString();
    ModelLocator* pcModel = ModelLocator::getInstance();
    Subject *pcSubject = &pcModel->getSubject();
    double dLabelMin, dLabelMax;
    pcSubject->getGrayMat().openLabelVolume(strFilename, dLabelMin, dLabelMax);
    pcSubject->getGrayMat().getColorScheme().autoFill(dLabelMax);   /// auto fill the color scheme table
    pcModel->getRenderWindow()->Render();
    pcModel->getRenderer()->ResetCamera();

    /// update the filename and ROI list on GUI
    rcOutputArg.setParameter("GM_parcellation_file", strFilename);
    return true;
}

