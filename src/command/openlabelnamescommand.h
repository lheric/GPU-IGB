#ifndef OPENLABELNAMESCOMMAND_H
#define OPENLABELNAMESCOMMAND_H

#include "gitlabstractcommand.h"

class OpenLabelNamesCommand : public GitlAbstractCommand
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit OpenLabelNamesCommand(QObject *parent = 0);
    Q_INVOKABLE virtual bool execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg);

signals:

public slots:
};

#endif // OPENLABELNAMESCOMMAND_H
