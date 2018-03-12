#ifndef OPENTRKCONNECTIONINFOCOMMAND_H
#define OPENTRKCONNECTIONINFOCOMMAND_H

#include "gitlabstractcommand.h"
#include "gitlmodule.h"

class OpenTrkConnectionInfoCommand : public GitlAbstractCommand
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit OpenTrkConnectionInfoCommand(QObject *parent = 0);
    Q_INVOKABLE bool execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg);

signals:

public slots:
};

#endif // OPENTRKCONNECTIONINFOCOMMAND_H
