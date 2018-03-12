#include "testcommand.h"
#include "model/modellocator.h"
#include <QImage>
#include <QPainter>
#include <QColor>
#include "model/modellocator.h"
#include <QDir>
#include "algorithm/imageutils.h"

TestCommand::TestCommand(QObject *parent) :
    GitlAbstractCommand(parent)
{
    setInWorkerThread(false);
}

bool TestCommand::execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg)
{
//    ModelLocator* pcModel = ModelLocator::getInstance();
//    Subject *pcSubject = &pcModel->getSubject();
//    FunctionalConnectivityData* pcFCData = &pcSubject->getFCData();
//    if(pcFCData->getMatrix().isEmpty())
//    {
//        qWarning() << "Functional Connectivity Data Not Loaded..";
//        return false;
//    }

//    pcModel->getRayCastRenderer().renderNetwork(pcFCData, true); ///TODO debug mode



//    QImage result;
//    pcModel->getRayCastRenderer().blendingTextureAndLighting(QImage("texture.png"),QImage("lightmap.png"),result);
//    result.save("blend.png");

    ModelLocator* pcModel = ModelLocator::getInstance();
    Subject *pcSubject = &pcModel->getSubject();

    QString strClass = rcInputArg.getParameter("class").toString();
    QString strLightmapFolder = rcInputArg.getParameter("lightmap_folder").toString();
    QString strTextureFolder = rcInputArg.getParameter("texture_folder").toString();

    QDir cLightmapDir(strLightmapFolder);
    QDir cTextureDir(strTextureFolder);

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



    pcModel->getRayCastRenderer().renderNetworkTile(pcFCData, true); ///TODO debug mode

    return true;
}

