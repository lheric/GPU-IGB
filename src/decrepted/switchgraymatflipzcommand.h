#ifndef SWITCHGRAYMATFLIPZCOMMAND_H
#define SWITCHGRAYMATFLIPZCOMMAND_H
#include "gitlabstractcommand.h"
#include "gitlmodule.h"
#include <QObject>

class SwitchGrayMatFlipZCommand : public GitlAbstractCommand
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit SwitchGrayMatFlipZCommand(QObject *parent = 0);

    Q_INVOKABLE bool execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg);
signals:

public slots:
};

#endif // SWITCHGRAYMATFLIPZCOMMAND_H
