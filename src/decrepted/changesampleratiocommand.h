#ifndef CHANGESAMPLERATIOCOMMAND_H
#define CHANGESAMPLERATIOCOMMAND_H
#include "gitlabstractcommand.h"

class ChangeSampleRatioCommand : public GitlAbstractCommand
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit ChangeSampleRatioCommand(QObject *parent = 0);
    Q_INVOKABLE bool execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg);
signals:

public slots:

};

#endif // CHANGESAMPLERATIOCOMMAND_H
