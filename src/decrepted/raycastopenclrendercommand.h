#ifndef RAYCASTOPENCLRENDERCOMMAND_H
#define RAYCASTOPENCLRENDERCOMMAND_H

#include "gitlabstractcommand.h"

class RayCastOpenCLRenderCommand : public GitlAbstractCommand
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit RayCastOpenCLRenderCommand(QObject *parent = 0);
    Q_INVOKABLE virtual bool execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg);
};

#endif // RAYCASTOPENCLRENDERCOMMAND_H
