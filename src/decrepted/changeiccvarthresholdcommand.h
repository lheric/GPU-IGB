#ifndef CHANGEICCVARTHRESHOLDCOMMAND_H
#define CHANGEICCVARTHRESHOLDCOMMAND_H

#include "gitlabstractcommand.h"
#include "gitlmodule.h"

class ChangeICCVarThresholdCommand : public GitlAbstractCommand
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit ChangeICCVarThresholdCommand(QObject *parent = 0);
    Q_INVOKABLE bool execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg);

};

#endif // CHANGEICCVARTHRESHOLDCOMMAND_H
