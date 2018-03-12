#ifndef SWITCHICCVARANALYSISCOMMAND_H
#define SWITCHICCVARANALYSISCOMMAND_H

#include "gitlabstractcommand.h"

class SwitchICCVarAnalysisCommand : public GitlAbstractCommand
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit SwitchICCVarAnalysisCommand(QObject *parent = 0);
    Q_INVOKABLE virtual bool execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg);

};

#endif // SWITCHICCVARANALYSISCOMMAND_H
