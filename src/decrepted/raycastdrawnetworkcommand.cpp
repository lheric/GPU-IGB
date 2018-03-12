#include "raycastdrawnetworkcommand.h"
#include "model/modellocator.h"
#include <QDir>
#include "algorithm/imageutils.h"

RaycastDrawNetworkCommand::RaycastDrawNetworkCommand(QObject *parent) : GitlAbstractCommand(parent)
{
    setInWorkerThread(false);
}

bool RaycastDrawNetworkCommand::execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg)
{
    ModelLocator* pcModel = ModelLocator::getInstance();
    Subject *pcSubject = &pcModel->getSubject();

    QString strClass = rcInputArg.getParameter("class").toString();
    QString strLightmapFolder = rcInputArg.getParameter("lightmap_folder").toString();
    QString strTextureFolder = rcInputArg.getParameter("texture_folder").toString();

    QDir cLightmapDir(strLightmapFolder);
    QDir cTextureDir(strTextureFolder);

    if(strClass == "fMRI")
    {

        FunctionalConnectivityData* pcFCData = &pcSubject->getFCData();
        if(pcFCData->getMatrix().isEmpty())
        {
            qWarning() << "Functional Connectivity Data Not Loaded..";
            return false;
        }


        QStringList cFilters;
        cFilters << "*.png";
        QFileInfoList cFileInfoList = cLightmapDir.entryInfoList(cFilters, QDir::Files|QDir::NoDotAndDotDot);
        foreach(QFileInfo cLightmapFile, cFileInfoList)
        {
            QString strLightmapName = cLightmapFile.fileName();
            if(!cTextureDir.exists(strLightmapName))
            {
                qWarning() << "Missing" << strLightmapName << "in texture folder" << strTextureFolder;
                return false;
            }
            qDebug() << strLightmapName << "FOUND";
            QString strTexturePath = cTextureDir.absoluteFilePath(strLightmapName);
            QImage cLightmapImg(cLightmapFile.absoluteFilePath());
            QImage cTextureImg(strTexturePath);
            QImage cResult;
            ImageUtils::blendingTextureAndLighting(cTextureImg, cLightmapImg, cResult);
            cResult.save(strLightmapName);
        }
        pcModel->getRayCastRenderer().renderNetwork(pcFCData, true); ///TODO debug mode

    }
    else
    {
        MultiDiseaseConnectivity* pcMDData = &pcSubject->getMDConnectivity();
        if(pcMDData->getMatrix().isEmpty())
        {
            qWarning() << "Multi-disease Connectivity Data Not Loaded..";
            return false;
        }


        QStringList cFilters;
        cFilters << "*.png";
        QFileInfoList cFileInfoList = cLightmapDir.entryInfoList(cFilters, QDir::Files|QDir::NoDotAndDotDot);
        foreach(QFileInfo cLightmapFile, cFileInfoList)
        {
            QString strLightmapName = cLightmapFile.fileName();
            if(!cTextureDir.exists(strLightmapName))
            {
                qWarning() << "Missing" << strLightmapName << "in texture folder" << strTextureFolder;
                return false;
            }
            qDebug() << strLightmapName << "FOUND";
            QString strTexturePath = cTextureDir.absoluteFilePath(strLightmapName);
            QImage cLightmapImg(cLightmapFile.absoluteFilePath());
            QImage cTextureImg(strTexturePath);
            QImage cResult;
            ImageUtils::blendingTextureAndLighting(cTextureImg, cLightmapImg, cResult);
            cResult.save(strLightmapName);
        }
        pcModel->getRayCastRenderer().renderNetworkTile(pcMDData, true); ///TODO debug mode
    }
    return true;
}

