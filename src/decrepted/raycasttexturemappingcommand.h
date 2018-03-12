#ifndef RAYCASTTEXTUREMAPPINGCOMMAND_H
#define RAYCASTTEXTUREMAPPINGCOMMAND_H
#include "gitlabstractcommand.h"

class RaycastTextureMappingCommand : public GitlAbstractCommand
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit RaycastTextureMappingCommand(QObject *parent = 0);
    Q_INVOKABLE virtual bool execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg);

};

#endif // RAYCASTTEXTUREMAPPINGCOMMAND_H
