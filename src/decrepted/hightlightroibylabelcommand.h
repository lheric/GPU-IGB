#ifndef HIGHTLIGHTROIBYLABELCOMMAND_H
#define HIGHTLIGHTROIBYLABELCOMMAND_H

#include "gitlabstractcommand.h"

class HightLightROIByLabelCommand : public GitlAbstractCommand
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit HightLightROIByLabelCommand(QObject *parent = 0);

    Q_INVOKABLE bool execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg);

signals:

public slots:
};

#endif // HIGHTLIGHTROIBYLABELCOMMAND_H
