#ifndef PICKCOORDINATECOMMAND_H
#define PICKCOORDINATECOMMAND_H

#include "gitlabstractcommand.h"

class PickCoordinateCommand : public GitlAbstractCommand
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit PickCoordinateCommand(QObject *parent = 0);

    Q_INVOKABLE bool execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg);
};

#endif // PICKCOORDINATECOMMAND_H
