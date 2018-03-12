#include "fiber.h"
#include <QTime>
#include <QDir>
#include <QDebug>
#include <QMatrix4x4>
#include "vtkPolyLine.h"
#include "trkfileio.h"

Fiber::Fiber(QObject *parent) : QObject(parent)
{
    init();
}

Fiber::~Fiber()
{

}

bool Fiber::init()
{
    ///
    this->setConnectionInfo(NULL);
    this->setFiberClassification(NULL);

    /// create
    this->setReader(vtkTrackVisTrkReader::New());    
    this->setFiberPolyData(vtkPolyData::New());    

    return true;
}

bool Fiber::openTrkFile(QString strFilepath)
{
    /// check if file exists
    QFileInfo cFileInfo(strFilepath);
    if( !cFileInfo.exists() )
    {
        qWarning() << QString("File Not Found: %1").arg(strFilepath);
        return false;
    }

    /// pass *.trk file path to reader
    this->getReader()->SetFileName(strFilepath.toStdString().c_str());
    this->getReader()->Update();
    this->getFiberPolyData()->ShallowCopy(this->getReader()->GetOutput());

    emit updated();

    return true;
}







