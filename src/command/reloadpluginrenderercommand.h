#ifndef RELOADPLUGINRENDERERCOMMAND_H
#define RELOADPLUGINRENDERERCOMMAND_H
#include <gitlabstractcommand.h>

class ReloadPluginRendererCommand : public GitlAbstractCommand
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit ReloadPluginRendererCommand(QObject *parent = 0);

    Q_INVOKABLE bool execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg);
signals:
    
public slots:
    
};

#endif // RELOADPLUGINRENDERERCOMMAND_H
