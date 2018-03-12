#ifndef RERENDERCOMMAND_H
#define RERENDERCOMMAND_H
#include "gitlabstractcommand.h"

class RerenderCommand : public GitlAbstractCommand
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit RerenderCommand(QObject *parent = 0);
    Q_INVOKABLE bool execute( GitlCommandParameter& rcInputArg, GitlCommandParameter& rcOutputArg );
signals:

public slots:

};

#endif // RERENDERCOMMAND_H
