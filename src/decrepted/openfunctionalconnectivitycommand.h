#ifndef OPENFUNCTIONALCONNECTIVITYCOMMAND_H
#define OPENFUNCTIONALCONNECTIVITYCOMMAND_H

#include "gitlabstractcommand.h"
#include "gitlmodule.h"

class OpenFunctionalConnectivityCommand : public GitlAbstractCommand
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit OpenFunctionalConnectivityCommand(QObject *parent = 0);

    Q_INVOKABLE bool execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg);
signals:

public slots:

};

#endif // OPENFUNCTIONALCONNECTIVITYCOMMAND_H
