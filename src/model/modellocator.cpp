#include "model/modellocator.h"
#include <QDebug>
SINGLETON_PATTERN_IMPLIMENT(ModelLocator)


ModelLocator::ModelLocator()
{
    this->setModuleName("model");

}

ModelLocator::~ModelLocator()
{
}





