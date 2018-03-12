#ifndef OPENTRKVARFILECOMMAND_H
#define OPENTRKVARFILECOMMAND_H

#include "gitlabstractcommand.h"
#include "gitlmodule.h"

class OpenTrkVarFileCommand : public GitlAbstractCommand
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit OpenTrkVarFileCommand(QObject *parent = 0);

    Q_INVOKABLE bool execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg);
signals:

public slots:

};

#endif // OPENTRKVARFILECOMMAND_H
