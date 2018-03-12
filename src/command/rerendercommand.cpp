#include "rerendercommand.h"
#include "model/modellocator.h"
#include <qthread.h>
RerenderCommand::RerenderCommand(QObject *parent) :
    GitlAbstractCommand(parent)
{
    setInWorkerThread(false);
}
bool RerenderCommand::execute( GitlCommandParameter& rcInputArg, GitlCommandParameter& rcOutputArg )
{
    Q_UNUSED(rcInputArg)
    Q_UNUSED(rcOutputArg)

    ModelLocator* pcModel = ModelLocator::getInstance();

    pcModel->getRenderWindow()->Render();
    bool bDepthPeeling = ModelLocator::getInstance()->getRenderer()->GetLastRenderingUsedDepthPeeling();

    if( !bDepthPeeling )
    {
        qDebug() <<  "Depth Peeling Failed";
        //qDebug() << "Depth Peeling is NOT working on this platform. Translucent geomerty may be incorrectly rendered.";
    }
    return true;
}
