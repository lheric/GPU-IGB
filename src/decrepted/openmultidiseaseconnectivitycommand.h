#ifndef OPENMULTIDISEASECONNECTIVITYCOMMAND_H
#define OPENMULTIDISEASECONNECTIVITYCOMMAND_H

#include "gitlabstractcommand.h"

class OpenMultiDiseaseConnectivityCommand : public GitlAbstractCommand
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit OpenMultiDiseaseConnectivityCommand(QObject *parent = 0);
    Q_INVOKABLE virtual bool execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg);

};

#endif // OPENMULTIDISEASECONNECTIVITYCOMMAND_H
