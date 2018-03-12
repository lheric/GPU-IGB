#ifndef SWITCHMODULESCOMMAND_H
#define SWITCHMODULESCOMMAND_H

#include <QObject>
#include "gitlabstractcommand.h"

class SwitchModulesCommand : public GitlAbstractCommand
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit SwitchModulesCommand(QObject *parent = 0);

    Q_INVOKABLE bool execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg);
signals:

public slots:
};

#endif // SWITCHMODULESCOMMAND_H
