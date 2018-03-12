#ifndef OPENFIBERFILECOMMAND_H
#define OPENFIBERFILECOMMAND_H

#include "gitlabstractcommand.h"
#include "gitlmodule.h"

class OpenFiberFileCommand : public GitlAbstractCommand
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit OpenFiberFileCommand(QObject *parent = 0);

    Q_INVOKABLE bool execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg);
signals:

public slots:

};

#endif // OPENFIBERFILECOMMAND_H
