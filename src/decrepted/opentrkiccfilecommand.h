#ifndef OPENTRKICCCOMMAND_H
#define OPENTRKICCCOMMAND_H

#include "gitlabstractcommand.h"
#include "gitlmodule.h"

class OpenTrkICCFileCommand : public GitlAbstractCommand
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit OpenTrkICCFileCommand(QObject *parent = 0);

    Q_INVOKABLE bool execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg);
signals:

public slots:

};

#endif // OPENTRKICCCOMMAND_H
