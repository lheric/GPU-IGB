#ifndef ROTATEPROJECTIONPLANECOMMAND_H
#define ROTATEPROJECTIONPLANECOMMAND_H

#include "gitlabstractcommand.h"
#include "gitlmodule.h"

class RotateProjectionPlaneCommand : public GitlAbstractCommand
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit RotateProjectionPlaneCommand(QObject *parent = 0);

    Q_INVOKABLE bool execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg);

signals:

public slots:
};

#endif // ROTATEPROJECTIONPLANECOMMAND_H
