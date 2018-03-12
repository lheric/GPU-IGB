#ifndef CHANGEFCNETWORKTHRESHOLDCOMMAND_H
#define CHANGEFCNETWORKTHRESHOLDCOMMAND_H

#include "gitlabstractcommand.h"
#include "gitlmodule.h"

class ChangeFCNetworkThresholdCommand : public GitlAbstractCommand
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit ChangeFCNetworkThresholdCommand(QObject *parent = 0);

    Q_INVOKABLE bool execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg);
signals:

public slots:
};

#endif // CHANGEFCNETWORKTHRESHOLDCOMMAND_H
