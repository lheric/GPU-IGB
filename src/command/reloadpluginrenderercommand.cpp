#include "reloadpluginrenderercommand.h"
#include "model/modellocator.h"
#include "gitlivkcmdevt.h"
#include <QApplication>
ReloadPluginRendererCommand::ReloadPluginRendererCommand(QObject *parent) :
    GitlAbstractCommand(parent)
{
    setInWorkerThread(false);
}

bool ReloadPluginRendererCommand::execute( GitlCommandParameter& rcInputArg, GitlCommandParameter& rcOutputArg )
{
    Q_UNUSED(rcInputArg)

    ModelLocator* pModel = ModelLocator::getInstance();
    PluginRendererLoader* pPluginRendererLoader = &pModel->getPluginLoader();
    pPluginRendererLoader->reloadAllFilters();
    /// refresh screen
    GitlIvkCmdEvt cRefresh("rerender");
    cRefresh.dispatch();
    ///
    rcOutputArg.setParameter("filter_names",  QVariant::fromValue(pPluginRendererLoader->getFilterNames())  );
    rcOutputArg.setParameter("filter_status", QVariant::fromValue(pPluginRendererLoader->getEnableStatus()) );

    return true;
}
