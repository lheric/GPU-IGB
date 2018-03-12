#include "changeraycastparametercommand.h"
#include "model/modellocator.h"
#include <time.h>
ChangeRayCastParameterCommand::ChangeRayCastParameterCommand(QObject *parent) :
    GitlAbstractCommand(parent)
{
    setInWorkerThread(false);
}


bool ChangeRayCastParameterCommand::execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg)
{
    ModelLocator* pcModel = ModelLocator::getInstance();
    RaycastParameters* pParameters = &pcModel->getRayCastOpenCLRenderer().getOpenCLContext().parameters;
    if( rcInputArg.hasParameter("transparency") )
    {
        pParameters->transparency = rcInputArg.getParameter("transparency").toDouble();
    }

    if( rcInputArg.hasParameter("ambient") )
    {
        pParameters->ambient = rcInputArg.getParameter("ambient").toDouble();
    }

    if( rcInputArg.hasParameter("diffuse") )
    {
        pParameters->diffuse = rcInputArg.getParameter("diffuse").toDouble();
    }

    if( rcInputArg.hasParameter("specular") )
    {
        pParameters->specular = rcInputArg.getParameter("specular").toDouble();
    }

    if( rcInputArg.hasParameter("f") )
    {
        pParameters->f = rcInputArg.getParameter("f").toDouble();
    }

    if( rcInputArg.hasParameter("change_center") )
    {
        bool bEnablePntWidget = rcInputArg.getParameter("change_center").toBool();
        pcModel->getRayCastOpenCLRenderer().getPointWidget()->SetEnabled(bEnablePntWidget);
    }

    if( rcInputArg.hasParameter("new_center") )
    {
        const QVector3D& rcCenter = rcInputArg.getParameter("new_center").value<QVector3D>();
        pParameters->center.x = rcCenter.x();
        pParameters->center.y = rcCenter.y();
        pParameters->center.z = rcCenter.z();
    }

    if( rcInputArg.hasParameter("change_kernel") )
    {
        QString strKernelName = rcInputArg.getParameter("change_kernel").toString();
        pcModel->getRayCastOpenCLRenderer().getOpenCLContext().kernal_name = strKernelName;
    }


    clock_t lStartTime = clock();
    pcModel->getRayCastOpenCLRenderer().render();
    clock_t lFinishTime = clock();
    qDebug() << (lFinishTime*1.0-lStartTime)/CLOCKS_PER_SEC;

    QImage cRenderedImg = pcModel->getRayCastOpenCLRenderer().getRenderedImage();
    rcOutputArg.setParameter("raycasting_opencl_rendered", cRenderedImg);

    return true;
}

