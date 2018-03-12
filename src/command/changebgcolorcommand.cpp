#include "changebgcolorcommand.h"
#include "model/modellocator.h"
#include <QColor>

ChangeBgColorCommand::ChangeBgColorCommand(QObject *parent) :
    GitlAbstractCommand(parent)
{
    setInWorkerThread(false);
}


bool ChangeBgColorCommand::execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg)
{
    Q_UNUSED(rcOutputArg)

    ModelLocator* pcModel = ModelLocator::getInstance();

    QString strColor = rcInputArg.getParameter("color").toString();
    QColor cColor(strColor);
    if( !cColor.isValid() )
        return false;

    pcModel->getRenderer()->SetBackground(cColor.redF(), cColor.greenF(),cColor.blueF());

    return true;
}
