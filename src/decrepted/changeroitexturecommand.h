#ifndef CHANGEROITEXTURECOMMAND_H
#define CHANGEROITEXTURECOMMAND_H

#include "gitlabstractcommand.h"
#include <QObject>
class ChangeROITextureCommand : public GitlAbstractCommand
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit ChangeROITextureCommand(QObject *parent = 0);
    Q_INVOKABLE bool execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg);
signals:

public slots:

};
#endif // CHANGEROITEXTURECOMMAND_H
