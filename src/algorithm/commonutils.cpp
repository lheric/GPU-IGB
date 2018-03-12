#include "commonutils.h"
#include <QFileDialog>
#include <QDebug>
#include "model/rom.h"
CommonUtils::CommonUtils()
{


}

bool CommonUtils::openFileDialog( QString strCaption,
                                  QString strFilter,
                                  QString strSaveKey,
                                  QString &rstrPath )
{
    QSettings cAppSetting(QSettings::IniFormat, QSettings::UserScope, ORGANIZATION_NAME, APPLICATION_NAME);
    QString strLastPath = cAppSetting.value(strSaveKey,".").toString();
    rstrPath=QFileDialog::getOpenFileName(NULL,
                                          strCaption,
                                          strLastPath,
                                          strFilter);

    if(!rstrPath.isEmpty())
        cAppSetting.setValue(strSaveKey,rstrPath);

    if(rstrPath.isEmpty() || !QFileInfo(rstrPath).exists() )
    {
        qWarning() << "File not found.";
        return false;
    }
    return true;
}
