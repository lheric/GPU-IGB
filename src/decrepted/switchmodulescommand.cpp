#include "switchmodulescommand.h"
#include "model/modellocator.h"

SwitchModulesCommand::SwitchModulesCommand(QObject *parent) :
    GitlAbstractCommand(parent)
{
    setInWorkerThread(false);
}

bool SwitchModulesCommand::execute( GitlCommandParameter& rcInputArg, GitlCommandParameter& rcOutputArg )
{
    QString strModuleName = rcInputArg.getParameter("module").toString();
    ModelLocator* pcModel = ModelLocator::getInstance();
    bool bStatus = rcInputArg.getParameter("status").toBool();
    vtkRenderer* pcRenderer = pcModel->getRenderer();
//    qDebug() << strModuleName << bStatus;

    if(strModuleName == "fiber")
    {
//          TODO refactor
//        if(bStatus == true)
//            pcModel->getSubject().getFiber().addToRenderer(pcRenderer);
//        else
//            pcModel->getSubject().getFiber().removeFromRenderer(pcRenderer);
    }
    else if(strModuleName == "gray_matter")
    {
//        if(bStatus == true)
//            pcModel->getSubject().getGrayMat().addToRenderer(pcRenderer);
//        else
//            pcModel->getSubject().getGrayMat().removeFromRenderer(pcRenderer);
    }
    else if(strModuleName == "fiber_classification")
    {
//        TODO refactor
//        pcModel->getSubject().getFiber().setEnableMask(bStatus);
    }
    else if(strModuleName == "fMRI")
    {
        pcModel->getSubject().getFMRITextureBuilder().setEnable(bStatus);
        pcModel->getSubject().getFCNetwork().setEnable(bStatus);
    }
    else if(strModuleName == "multi_disease")
    {
        pcModel->getSubject().getMDTextureBuilder().setEnable(bStatus);
        pcModel->getSubject().getMDNetwork().setEnable(bStatus);
    }
    else if(strModuleName == "projection_plane")
    {
//        if(bStatus == true)
//        {
//            /// TODO setEnable method

//            pcModel->getSubject().getProjectionPlane().projectGrayMatter(&pcModel->getSubject().getGrayMat().getLabelSurface());
//            pcModel->getSubject().getProjectionPlane().addActorsToRenderer(pcRenderer);
//        }
//        else
//        {
//            pcModel->getSubject().getProjectionPlane().removeFromRenderer(pcRenderer);
//        }
    }
    else
    {
        qWarning() << "Unsupported Module Name" << strModuleName;
    }

    pcModel->getRenderWindow()->Render();



    return true;
}

