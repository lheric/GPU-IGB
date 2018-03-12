#ifndef SUBJECT_H
#define SUBJECT_H
#include "gitldef.h"
#include "graymatter.h"
#include "fiber.h"

class Subject
{
public:
    Subject();
    ~Subject();


    /// Gray Matter
    ADD_CLASS_FIELD_NOSETTER(GrayMatter, cGrayMatter, getGrayMat)

    /// Fiber
    ADD_CLASS_FIELD_NOSETTER(Fiber, cFiber, getFiber)

};

#endif // SUBJECT_H
