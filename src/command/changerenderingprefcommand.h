#ifndef CHANGERENDERINGPREFCOMMAND_H
#define CHANGERENDERINGPREFCOMMAND_H

#include "gitlabstractcommand.h"

class ChangeRenderingPrefCommand : public GitlAbstractCommand
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit ChangeRenderingPrefCommand(QObject *parent = 0);
    Q_INVOKABLE virtual bool execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg);



signals:

public slots:
};

#endif // CHANGERENDERINGPREFCOMMAND_H
