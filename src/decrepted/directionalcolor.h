#ifndef DIRECTIONALCOLOR_H
#define DIRECTIONALCOLOR_H

class DirectionalColor
{
public:
    DirectionalColor();
    void convertToColor(double *adStartPos, double *adEndPos,  unsigned char *auhColor);
};

#endif // DIRECTIONALCOLOR_H
