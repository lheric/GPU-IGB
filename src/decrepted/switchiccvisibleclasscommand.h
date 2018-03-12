#ifndef SWITCHICCVISIBLECLASSCOMMAND_H
#define SWITCHICCVISIBLECLASSCOMMAND_H

#include "gitlabstractcommand.h"
#include "gitlmodule.h"

class SwitchICCVisibleClassCommand : public GitlAbstractCommand
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit SwitchICCVisibleClassCommand(QObject *parent = 0);
    Q_INVOKABLE bool execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg);

signals:

public slots:
};

#endif // SWITCHICCVISIBLECLASSCOMMAND_H
