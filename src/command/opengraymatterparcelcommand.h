#ifndef OPENGRAYMATTERPARCELCOMMAND_H
#define OPENGRAYMATTERPARCELCOMMAND_H

#include "gitlabstractcommand.h"

class OpenGrayMatterParcelCommand : public GitlAbstractCommand
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit OpenGrayMatterParcelCommand(QObject *parent = 0);
    Q_INVOKABLE virtual bool execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg);

signals:

public slots:
};

#endif // OPENGRAYMATTERPARCELCOMMAND_H
