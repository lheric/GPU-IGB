#ifndef SWITCHPARALLELPROJECTIONCOMMAND_H
#define SWITCHPARALLELPROJECTIONCOMMAND_H

#include "gitlabstractcommand.h"
#include "gitlmodule.h"

class SwitchParallelProjectionCommand : public GitlAbstractCommand
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit SwitchParallelProjectionCommand(QObject *parent = 0);

    Q_INVOKABLE bool execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg);
signals:

public slots:
};

#endif // SWITCHPARALLELPROJECTIONCOMMAND_H
