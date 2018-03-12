#include "parcellationlabels.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QDir>
#include <QFile>

ParcellationLabels::ParcellationLabels(QObject *parent) : QObject(parent)
{
    this->setValid(false);
}

bool ParcellationLabels::loadParcellation(QString strFilename)
{

    QFile cInFile(strFilename);
    if(cInFile.open(QIODevice::ReadOnly) == false)
    {
        qWarning() << "Open Parcellation Label Failed:" << strFilename;
        return false;
    }

    /// clear the old one
    this->getLabelNamesTable().clear();

    /// read the new one
    int iLabel = -1;
    QString strLabelName;

    QTextStream cInStream(&cInFile);
    QString strLine;
    int iLineNum = 0;
    while(cInStream.atEnd() == false)
    {
        ++iLineNum;
        strLine = cInStream.readLine();
        QStringList strList = strLine.split(",", QString::SkipEmptyParts);
        if(strList.size() != 2)
        {
            qWarning() << "Line" << iLineNum << "Has" << strList.size() << "Elements. Expecting 2 Elements Per Line.";
            this->getLabelNamesTable().clear();
            this->setValid(false);
            return false;
        }
        bool bOK;
        iLabel = strList[0].toInt(&bOK);
        strLabelName = strList[1];
        if(bOK == false)
        {
            qWarning() << "Expecting the Fisrt Element to Be a Number in Line" << iLineNum << ".";
            this->getLabelNamesTable().clear();
            this->setValid(false);
            return false;
        }

        /// add one key-value pair
        if(this->getLabelNamesTable().contains(iLabel) == true)
        {
            qWarning() << "Duplicate Label" << iLabel << "Found in Line" << iLineNum << ".";
            this->getLabelNamesTable().clear();
            this->setValid(false);
            return false;
        }
        this->getLabelNamesTable()[iLabel] = strLabelName;

    }
    this->setName(QFileInfo(strFilename).baseName());
    this->setValid(true);
    emit updated();
    return true;

}

bool ParcellationLabels::query(int iLabel, QString *pstrLabelName)
{
    if(this->getValid() == false)
        return false;

    if(this->getLabelNamesTable().contains(iLabel) == true)
    {
        *pstrLabelName = this->getLabelNamesTable()[iLabel];
        return true;
    }
    else
    {
        qWarning() << "Label" << iLabel << "Dose Not Exist.";
        return false;
    }

}

QList<int> ParcellationLabels::getKeys()
{
    return this->getLabelNamesTable().keys();
}

int ParcellationLabels::getNumofROIs()
{
    return this->getLabelNamesTable().size();
}
