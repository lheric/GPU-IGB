#ifndef SWITCHPLUGINCOMMAND_H
#define SWITCHPLUGINCOMMAND_H
#include "gitlabstractcommand.h"

class SwitchPluginCommand : public GitlAbstractCommand
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit SwitchPluginCommand(QObject *parent = 0);

    Q_INVOKABLE bool execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg);
signals:
    
public slots:
    
};

#endif // SWITCHPLUGINCOMMAND_H
