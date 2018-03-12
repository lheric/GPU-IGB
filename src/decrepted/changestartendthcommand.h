#ifndef CHANGESTARTENDTHCOMMAND_H
#define CHANGESTARTENDTHCOMMAND_H
#include "gitlabstractcommand.h"
#include "gitlmodule.h"

class ChangeStartEndThCommand : public GitlAbstractCommand
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit ChangeStartEndThCommand(QObject *parent = 0);
    Q_INVOKABLE bool execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg);
signals:

public slots:

};

#endif // CHANGESTARTENDTHCOMMAND_H
