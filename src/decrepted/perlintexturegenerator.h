#ifndef PERLINTEXTUREGENERATOR_H
#define PERLINTEXTUREGENERATOR_H
#include <gitldef.h>
#include <QImage>
#include <QString>
#include <QVector3D>
#include <QImage>
#include <QList>

class PerlinTextureGenerator
{
public:
    PerlinTextureGenerator();
    ~PerlinTextureGenerator();
    static QString generateContourTexture(const QString& strFilepath, const QVector<double>& adTimeSeriesData);
    void convertToGrayScale(const QImage &cSrc, QImage& cDst);
    static void erode(const QImage &cSrc, QImage& cDst, int iMaskSize, QRgb cColor);
    static void overlayImage(QImage &cBaseImage, const QImage& cOverlayImage);
    QImage generatePerlineTexture(const QVector<double>& cRYBWeights,
                                double dDensity,
                                int iROILabel,
                                QString& strTexturePath, int iWidth = 256, int iHeight = 256);

protected:
    void getBlendingResult(QImage& cImg, double dDensity);
    void generateSmoothNoise();
    double smoothNoiseInterpolation(double x, double y);
    //double turbulence(double x, double y, double size);
    void generateRandomNoise();
    QColor ryb2rgb(float inR, float inY, float inB);


    ADD_CLASS_FIELD_PRIVATE(int,  iWidth)
    ADD_CLASS_FIELD_PRIVATE(int , iHeight)
    ADD_CLASS_FIELD_PRIVATE(QImage, cRandomNoise)
    ADD_CLASS_FIELD_PRIVATE(QImage, cSmoothed)
    ADD_CLASS_FIELD_PRIVATE(QList<QImage>, acAllWeights)
    ADD_CLASS_FIELD_PRIVATE(QImage, cResult)

    QVector3D cColor1 = QVector3D(1.0, 0.0, 0.0); //r
    QVector3D cColor2 = QVector3D(0.0, 1.0, 0.0); //y
    QVector3D cColor3 = QVector3D(0.0, 0.0, 1.0); //b
    QVector3D cColor4 = QVector3D(0.3, 0.5, 0.0); //1
    QVector3D cColor5 = QVector3D(0.5, 0.3, 0.1); //2
};

#endif // PERLINTEXTUREGENERATOR_H
