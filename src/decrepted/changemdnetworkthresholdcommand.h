#ifndef CHANGEMDNETWORKTHRESHOLDCOMMAND_H
#define CHANGEMDNETWORKTHRESHOLDCOMMAND_H

#include "gitlabstractcommand.h"
#include "gitlmodule.h"

class ChangeMDNetworkThresholdCommand : public GitlAbstractCommand
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit ChangeMDNetworkThresholdCommand(QObject *parent = 0);

    Q_INVOKABLE bool execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg);

signals:

public slots:
};

#endif // CHANGEMDNETWORKTHRESHOLDCOMMAND_H
