#include "changeroitexturecommand.h"
#include "model/modellocator.h"
#include "gitlivkcmdevt.h"
#include "gitlupdateuievt.h"

ChangeROITextureCommand::ChangeROITextureCommand(QObject *parent) :
    GitlAbstractCommand(parent)
{
    setInWorkerThread(false);
}

bool ChangeROITextureCommand::execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg)
{
    QString strTextureType = rcInputArg.getParameter("type").toString();
    TextureType eType = TEXTURE_NONE;

    if(strTextureType == "none")
        eType = TEXTURE_NONE;
    else if(strTextureType == "color")
        eType = TEXTURE_SCOLOR;
    else if(strTextureType == "strip")
        eType = TEXTURE_STRIPE;
    else if(strTextureType == "circle")
        eType = TEXTURE_CIRCLE;
    else if(strTextureType == "contour")
        eType = TEXTURE_CONTOUR;
    else
    {
        qWarning() << "Unkonw Texture Type" << strTextureType;
        return false;
    }

    ModelLocator* pcModel = ModelLocator::getInstance();
    pcModel->getSubject().getFMRITextureBuilder().setTextureType(eType);

    pcModel->getRenderWindow()->Render();

    qDebug() << "Texture Changed To" << strTextureType;

    return true;
}
