#include "ray.h"

uint qHash(const InterpComponent& rcComp)
{
    return rcComp.getLabel();
}
