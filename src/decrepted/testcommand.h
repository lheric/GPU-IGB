#ifndef TESTCOMMAND_H
#define TESTCOMMAND_H

#include "gitlabstractcommand.h"

class TestCommand : public GitlAbstractCommand
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit TestCommand(QObject *parent = 0);

    Q_INVOKABLE bool execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg);
signals:

public slots:
};

#endif // TESTCOMMAND_H
