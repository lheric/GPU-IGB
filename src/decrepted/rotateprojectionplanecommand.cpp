#include "rotateprojectionplanecommand.h"
#include "model/modellocator.h"
#include <vtkCamera.h>
#include <QVector3D>

RotateProjectionPlaneCommand::RotateProjectionPlaneCommand(QObject *parent) :
    GitlAbstractCommand(parent)
{
    setInWorkerThread(false);
}

bool RotateProjectionPlaneCommand::execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg)
{
    /// TODO
//    ModelLocator* pcModel = ModelLocator::getInstance();
//    ProjectionPlane* pcProjPlane = &pcModel->getSubject().getProjectionPlane();
//    vtkCamera* pcCamera = pcModel->getRenderer()->GetActiveCamera();
//    ProbabilityVolume* pcProbVol = &pcModel->getSubject().getGrayMat().getProbVol();
//    LabelSurface* pcSurface = &pcModel->getSubject().getGrayMat().getLabelSurface();

//    /// if no gray matter is loaded
//    if(pcProbVol->getFilepath().isEmpty())
//        return true;

//    int* aiDimension = pcProbVol->getVolume()->GetDimensions();
//    int iPlaneSize = *std::max_element(aiDimension, aiDimension+3)*1.3;
//    int iPlaneDistFromGMCenter = iPlaneSize/1.3;

//    double* adGMCenter = pcProbVol->getVolume()->GetCenter();
//    double* adUp  = pcCamera->GetViewUp();
//    double* adPos = pcCamera->GetPosition();
////    double* adFocus = pcCamera->GetFocalPoint();


//    QVector3D cPoint1(adUp[0],adUp[1],adUp[2]);
//    QVector3D cPos(adPos[0],adPos[1],adPos[2]);
//    QVector3D cGMCenter(adGMCenter[0],adGMCenter[1],adGMCenter[2]);
//    QVector3D cNormal = cPos - cGMCenter;
//    QVector3D cOrigin = cGMCenter + cNormal.normalized()*iPlaneDistFromGMCenter;

//    QVector3D cPoint2 = QVector3D::crossProduct(cPoint1,cNormal);


//    cPoint1 = cPoint1.normalized()*iPlaneSize;
//    cPoint2 = cPoint2.normalized()*iPlaneSize;


//    cOrigin = cOrigin - 0.5*cPoint1 - 0.5*cPoint2;

//    cPoint1 = cOrigin+cPoint1;
//    cPoint2 = cOrigin+cPoint2;


//    pcProjPlane->getPlaneSource()->SetOrigin(cOrigin[0], cOrigin[1], cOrigin[2]);
//    pcProjPlane->getPlaneSource()->SetPoint1(cPoint2[0], cPoint2[1], cPoint2[2]);
//    pcProjPlane->getPlaneSource()->SetPoint2(cPoint1[0], cPoint1[1], cPoint1[2]);

//    pcProjPlane->projectGrayMatter(pcSurface);



//    pcModel->getRenderWindow()->Render();

    return true;
}
