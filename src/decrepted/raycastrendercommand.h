#ifndef RAYCASTRENDERCOMMAND_H
#define RAYCASTRENDERCOMMAND_H

#include "gitlabstractcommand.h"

class RayCastRenderCommand : public GitlAbstractCommand
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit RayCastRenderCommand(QObject *parent = 0);

    Q_INVOKABLE bool execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg);
signals:

public slots:
};

#endif // RAYCASTRENDERCOMMAND_H
