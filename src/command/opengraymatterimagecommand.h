#ifndef OPENGRAYMATTERIMAGECOMMAND_H
#define OPENGRAYMATTERIMAGECOMMAND_H
#include "gitlabstractcommand.h"

class OpenGrayMatterImageCommand : public GitlAbstractCommand
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit OpenGrayMatterImageCommand(QObject *parent = 0);
    Q_INVOKABLE virtual bool execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg);
signals:

public slots:

};

#endif // OPENGRAYMATTERIMAGECOMMAND_H
