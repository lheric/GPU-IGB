#ifndef OPENTRKCOUNTFILECOMMAND_H
#define OPENTRKCOUNTFILECOMMAND_H

#include "gitlabstractcommand.h"
#include "gitlmodule.h"

class OpenTrkCountFileCommand : public GitlAbstractCommand
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit OpenTrkCountFileCommand(QObject *parent = 0);
    Q_INVOKABLE bool execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg);

signals:

public slots:
};

#endif // OPENTRKCOUNTFILECOMMAND_H
