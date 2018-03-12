#ifndef CONNECTPIPELINECOMMAND_H
#define CONNECTPIPELINECOMMAND_H
#include "gitlabstractcommand.h"
#include "gitlmodule.h"
#include <QObject>

class ConnectPipelineCommand : public GitlAbstractCommand
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit ConnectPipelineCommand(QObject *parent = 0);

    Q_INVOKABLE bool execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg);
signals:

public slots:

};

#endif // CONNECTPIPELINECOMMAND_H
