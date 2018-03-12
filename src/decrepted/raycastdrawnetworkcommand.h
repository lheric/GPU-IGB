#ifndef RAYCASTDRAWNETWORKCOMMAND_H
#define RAYCASTDRAWNETWORKCOMMAND_H

#include "gitlabstractcommand.h"

class RaycastDrawNetworkCommand : public GitlAbstractCommand
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit RaycastDrawNetworkCommand(QObject *parent = 0);
    Q_INVOKABLE virtual bool execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg);

};

#endif // RAYCASTDRAWNETWORKCOMMAND_H
