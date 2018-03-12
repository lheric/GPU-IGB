#ifndef CHANGERAYCASTPARAMETERCOMMAND_H
#define CHANGERAYCASTPARAMETERCOMMAND_H

#include "gitlabstractcommand.h"

class ChangeRayCastParameterCommand : public GitlAbstractCommand
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit ChangeRayCastParameterCommand(QObject *parent = 0);
    Q_INVOKABLE virtual bool execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg);
signals:

public slots:
};

#endif // CHANGERAYCASTPARAMETERCOMMAND_H
