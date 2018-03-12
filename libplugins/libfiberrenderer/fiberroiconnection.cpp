#include "fiberroiconnection.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <limits>

FiberROIConnection::FiberROIConnection()
{
    init();
}

FiberROIConnection::~FiberROIConnection()
{

}

void FiberROIConnection::init()
{
    m_cConnectedROIs.clear();
}

