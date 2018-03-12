#ifndef CHANGEINTERACTORSTYLECOMMAND_H
#define CHANGEINTERACTORSTYLECOMMAND_H
#include "gitlabstractcommand.h"
#include <QObject>

class ChangeInteractorStyleCommand : public GitlAbstractCommand
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit ChangeInteractorStyleCommand(QObject *parent = 0);
    Q_INVOKABLE bool execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg);
signals:

public slots:
};

#endif // CHANGEINTERACTORSTYLECOMMAND_H
