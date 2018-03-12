#ifndef IMAGEUTILS_H
#define IMAGEUTILS_H

#include <QImage>
#include <QVector4D>
#include "model/matrix.h"
#include <vtkImageData.h>
class ImageUtils
{
public:
    ImageUtils();
    ~ImageUtils();

    static int erode(const QImage &cSrc, QImage &cDst, int iMaskSize, QRgb cColor, QPointF *pcCenter = NULL);
    static int dilate(const QImage &cSrc, QImage &cDst, int iMaskSize, QRgb cColor);
    static int close(const QImage &cSrc, QImage &cDst, int iMaskSize, QRgb cColor);
    static void overlayImage(QImage &cBaseImage, const QImage &cOverlayImage, double dOpacity = 1.0);
    static void blur(QImage& rcImage);
    static QImage hammerAitoffProjection(QImage &rcInputImg, QColor cBackgroundColor);
    static bool hammerAitoffTransform(QPointF &rcInput, QPointF &rcOutput, int iWidth, int iHeight);
    static Matrix<int> hammerAitoffProjection(Matrix<int> &rcLabelImg);
    static void clipVector4D(QVector4D& rcColor, double dMax);
    static QRgb bilinearSampling(QImage &rcInputImg, double x, double y);
    static QImage generateLatiLogiTexture(int iWidth, int iHeight);
    static void calculateROIGradient(vtkImageData *pcIntensity, vtkImageData *pcLabel, vtkImageData *pcGradient);
    static QImage maskImage(QImage &rcInput, QImage &rcMask);
    static bool blendingTextureAndLighting(QImage &rcTexture, QImage &rcLighting, QImage& rcResult);
};

#endif // IMAGEUTILS_H
