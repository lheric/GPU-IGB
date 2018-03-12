#include "colorconverter.h"

ColorConverter::ColorConverter(QObject *parent) : QObject(parent)
{

}

ColorConverter::~ColorConverter()
{

}


QColor ColorConverter::ryb2rgb(float inR, float inY, float inB)
{

    const static QVector3D f000(1.0f,     1.0f,        1.0f);
    const static QVector3D f001(0.163f,   0.373f,      0.6f);
    const static QVector3D f010(1.0f,     1.0f,        0.0f);
    const static QVector3D f011(0.0f,     0.66f,       0.2f);
    const static QVector3D f100(1.0f,     0.0f,        0.0f);
    const static QVector3D f101(0.5f,     0.5f,        0.0f);
    const static QVector3D f110(1.0f,     0.5f,        0.0f);
    const static QVector3D f111(0.2f,     0.094f,      0.0f);
    float oneMinusR = 1-inR;
    float oneMinusY = 1-inY;
    float oneMinusB = 1-inB;
    static QVector3D rgb;
    rgb = f000*oneMinusR*oneMinusY*oneMinusB + f001*oneMinusR*oneMinusY*inB
            + f010*oneMinusR*inY*oneMinusB + f100*inR*oneMinusY*oneMinusB +
            f011*oneMinusR*inY*inB + f101*inR*oneMinusY*inB +
            f110*inR*inY*oneMinusB + f111*inR*inY*inB;

    rgb *= 255;

    float outR = qMax(0, qMin(int(rgb.x()), 255));
    float outG = qMax(0, qMin(int(rgb.y()), 255));
    float outB = qMax(0, qMin(int(rgb.z()), 255));
    return QColor(outR, outG, outB);
}
