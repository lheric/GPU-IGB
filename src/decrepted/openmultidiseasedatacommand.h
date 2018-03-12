#ifndef OPENMULTIDISEASEMEASUREMENTCOMMAND_H
#define OPENMULTIDISEASEMEASUREMENTCOMMAND_H

#include "gitlabstractcommand.h"

class OpenMultiDiseaseMeasurementCommand : public GitlAbstractCommand
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit OpenMultiDiseaseMeasurementCommand(QObject *parent = 0);
    Q_INVOKABLE virtual bool execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg);

};

#endif // OPENMULTIDISEASEMEASUREMENTCOMMAND_H
