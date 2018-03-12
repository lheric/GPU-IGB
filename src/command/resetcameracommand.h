#ifndef RESETCAMERACOMMAND_H
#define RESETCAMERACOMMAND_H
#include "gitlabstractcommand.h"

class ResetCameraCommand : public GitlAbstractCommand
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit ResetCameraCommand(QObject *parent = 0);
    Q_INVOKABLE virtual bool execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg);

signals:

public slots:
};

#endif // RESETCAMERACOMMAND_H
