#include "switchplugincommand.h"
#include "model/modellocator.h"
#include "gitlivkcmdevt.h"
#include <QDebug>
SwitchPluginCommand::SwitchPluginCommand(QObject *parent) :
    GitlAbstractCommand(parent)
{
    this->setInWorkerThread(false);
}


bool SwitchPluginCommand::execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg)
{
    ModelLocator* pModel = ModelLocator::getInstance();
    PluginRendererLoader* pcPluginLoader = &pModel->getPluginLoader();
    QString strFiltername = rcInputArg.getParameter("filter_name").toString();
    bool bEnable = rcInputArg.getParameter("filter_enable").toBool();
    AbstractPluginRenderer* pcPlugin = pcPluginLoader->getPluginByName(strFiltername);
    if( pcPlugin == NULL )
    {
        qCritical() << QString("Cannot find filter %1").arg(strFiltername);
        return false;
    }
    pcPlugin->setEnable(bEnable);

    // reset camera
    GitlIvkCmdEvt cResetCam("reset_camera");
    cResetCam.dispatch();

    /// refresh
    GitlIvkCmdEvt cRerenderEvt("rerender");
    cRerenderEvt.dispatch();

    ///
    rcOutputArg.setParameter("filter_names",  QVariant::fromValue(pcPluginLoader->getFilterNames())  );
    rcOutputArg.setParameter("filter_status", QVariant::fromValue(pcPluginLoader->getEnableStatus()) );
    return true;
}
