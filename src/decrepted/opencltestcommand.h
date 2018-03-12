#ifndef OPENCLTESTCOMMAND_H
#define OPENCLTESTCOMMAND_H

#include "gitlabstractcommand.h"

class OpenCLTestCommand : public GitlAbstractCommand
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit OpenCLTestCommand(QObject *parent = 0);

    Q_INVOKABLE bool execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg);
signals:

public slots:
};

#endif // OPENCLTESTCOMMAND_H
