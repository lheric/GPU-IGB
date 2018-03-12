#ifndef CHANGEBGCOLORCOMMAND_H
#define CHANGEBGCOLORCOMMAND_H
#include "gitlabstractcommand.h"

class ChangeBgColorCommand : public GitlAbstractCommand
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit ChangeBgColorCommand(QObject *parent = 0);

    Q_INVOKABLE bool execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg);
signals:

public slots:
};

#endif // CHANGEBGCOLORCOMMAND_H
