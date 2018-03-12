#ifndef OPENTIMESERIESDATACOMMAND_H
#define OPENTIMESERIESDATACOMMAND_H
#include "gitlabstractcommand.h"

class OpenTimeSeriesDataCommand : public GitlAbstractCommand
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit OpenTimeSeriesDataCommand(QObject *parent = 0);
    Q_INVOKABLE virtual bool execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg);


};

#endif // OPENTIMESERIESDATACOMMAND_H
