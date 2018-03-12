#ifndef CHANGELENGTHTHCOMMAND_H
#define CHANGELENGTHTHCOMMAND_H
#include "gitlabstractcommand.h"

class ChangeLengthThCommand : public GitlAbstractCommand
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit ChangeLengthThCommand(QObject *parent = 0);
    Q_INVOKABLE bool execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg);
signals:

public slots:

};

#endif // CHANGELENGTHTHCOMMAND_H
