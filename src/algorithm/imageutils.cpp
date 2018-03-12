#include "imageutils.h"
#include <QPainter>
#include <QVector3D>
#include <QColor>
#include <qmath.h>
#include <algorithm>
#include <vtkNIFTIImageWriter.h>
#include <vtkSmartPointer.h>
#include "model/matrix.h"
#include "vtkutils.h"

using namespace std;

#define GAUSSIAN_MATRIX_SIZE 21
static double s_adGauBlurMatrix[GAUSSIAN_MATRIX_SIZE][GAUSSIAN_MATRIX_SIZE] =
{

    { 0, 0, 0, 0, 0, 0, 0, 0, 0.000100, 0.000100, 0.000100, 0.000100, 0.000100, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0.000100, 0.000100, 0.000200, 0.000200, 0.000200, 0.000200, 0.000200, 0.000100, 0.000100, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0.000100, 0.000100, 0.000200, 0.000300, 0.000400, 0.000500, 0.000500, 0.000500, 0.000400, 0.000300, 0.000200, 0.000100, 0.000100, 0, 0, 0, 0 },
    { 0, 0, 0, 0.000100, 0.000200, 0.000300, 0.000500, 0.000700, 0.000900, 0.00110, 0.00120, 0.00110, 0.000900, 0.000700, 0.000500, 0.000300, 0.000200, 0.000100, 0, 0, 0 },
    { 0, 0, 0.000100, 0.000200, 0.000300, 0.000600, 0.00100, 0.00150, 0.00190, 0.00230, 0.00240, 0.00230, 0.00190, 0.00150, 0.00100, 0.000600, 0.000300, 0.000200, 0.000100, 0, 0 },
    { 0, 0, 0.000100, 0.000300, 0.000600, 0.00110, 0.00180, 0.00270, 0.00350, 0.00420, 0.00440, 0.00420, 0.00350, 0.00270, 0.00180, 0.00110, 0.000600, 0.000300, 0.000100, 0, 0 },
    { 0, 0.000100, 0.000200, 0.000500, 0.00100, 0.00180, 0.00300, 0.00440, 0.00580, 0.00690, 0.00730, 0.00690, 0.00580, 0.00440, 0.00300, 0.00180, 0.00100, 0.000500, 0.000200, 0.000100, 0 },
    { 0, 0.000100, 0.000300, 0.000700, 0.00150, 0.00270, 0.00440, 0.00650, 0.00860, 0.0102, 0.0107, 0.0102, 0.00860, 0.00650, 0.00440, 0.00270, 0.00150, 0.000700, 0.000300, 0.000100, 0 },
    { 0.000100, 0.000200, 0.000400, 0.000900, 0.00190, 0.00350, 0.00580, 0.00860, 0.0113, 0.0134, 0.0142, 0.0134, 0.0113, 0.00860, 0.00580, 0.00350, 0.00190, 0.000900, 0.000400, 0.000200, 0.000100 },
    { 0.000100, 0.000200, 0.000500, 0.00110, 0.00230, 0.00420, 0.00690, 0.0102, 0.0134, 0.0158, 0.0167, 0.0158, 0.0134, 0.0102, 0.00690, 0.00420, 0.00230, 0.00110, 0.000500, 0.000200, 0.000100 },
    { 0.000100, 0.000200, 0.000500, 0.00120, 0.00240, 0.00440, 0.00730, 0.0107, 0.0142, 0.0167, 0.0177, 0.0167, 0.0142, 0.0107, 0.00730, 0.00440, 0.00240, 0.00120, 0.000500, 0.000200, 0.000100 },
    { 0.000100, 0.000200, 0.000500, 0.00110, 0.00230, 0.00420, 0.00690, 0.0102, 0.0134, 0.0158, 0.0167, 0.0158, 0.0134, 0.0102, 0.00690, 0.00420, 0.00230, 0.00110, 0.000500, 0.000200, 0.000100 },
    { 0.000100, 0.000200, 0.000400, 0.000900, 0.00190, 0.00350, 0.00580, 0.00860, 0.0113, 0.0134, 0.0142, 0.0134, 0.0113, 0.00860, 0.00580, 0.00350, 0.00190, 0.000900, 0.000400, 0.000200, 0.000100 },
    { 0, 0.000100, 0.000300, 0.000700, 0.00150, 0.00270, 0.00440, 0.00650, 0.00860, 0.0102, 0.0107, 0.0102, 0.00860, 0.00650, 0.00440, 0.00270, 0.00150, 0.000700, 0.000300, 0.000100, 0 },
    { 0, 0.000100, 0.000200, 0.000500, 0.00100, 0.00180, 0.00300, 0.00440, 0.00580, 0.00690, 0.00730, 0.00690, 0.00580, 0.00440, 0.00300, 0.00180, 0.00100, 0.000500, 0.000200, 0.000100, 0 },
    { 0, 0, 0.000100, 0.000300, 0.000600, 0.00110, 0.00180, 0.00270, 0.00350, 0.00420, 0.00440, 0.00420, 0.00350, 0.00270, 0.00180, 0.00110, 0.000600, 0.000300, 0.000100, 0, 0 },
    { 0, 0, 0.000100, 0.000200, 0.000300, 0.000600, 0.00100, 0.00150, 0.00190, 0.00230, 0.00240, 0.00230, 0.00190, 0.00150, 0.00100, 0.000600, 0.000300, 0.000200, 0.000100, 0, 0 },
    { 0, 0, 0, 0.000100, 0.000200, 0.000300, 0.000500, 0.000700, 0.000900, 0.00110, 0.00120, 0.00110, 0.000900, 0.000700, 0.000500, 0.000300, 0.000200, 0.000100, 0, 0, 0 },
    { 0, 0, 0, 0, 0.000100, 0.000100, 0.000200, 0.000300, 0.000400, 0.000500, 0.000500, 0.000500, 0.000400, 0.000300, 0.000200, 0.000100, 0.000100, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0.000100, 0.000100, 0.000200, 0.000200, 0.000200, 0.000200, 0.000200, 0.000100, 0.000100, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0.000100, 0.000100, 0.000100, 0.000100, 0.000100, 0, 0, 0, 0, 0, 0, 0, 0 },

};


ImageUtils::ImageUtils()
{

}

ImageUtils::~ImageUtils()
{

}


int ImageUtils::erode(const QImage &cSrc, QImage &cDst, int iMaskSize, QRgb cColor, QPointF* pcCenter)
{
    int iWidth = cDst.width();
    int iHeight = cDst.height();
    int iMaskRadius = (iMaskSize-1)/2;

    /// init bg with transparent color
    cDst.fill(qRgba(255, 255, 255, 0));

    /// image loop
    bool bKeep = true;
    bool bAbort = false;
    int iArea = 0;
    QPointF cEstCenter; bool bUserEstCenter = true;
    for(int outer_i = 0; outer_i < iWidth; outer_i++)
    {
        for(int outer_j = 0; outer_j < iHeight; outer_j++)
        {
            /// mask loop

            /// init
            bKeep = true;
            bAbort = false;

            /// erode the dark pixel
            if(qAlpha(cSrc.pixel(outer_i, outer_j)) != 0)
            {
                cEstCenter.setX(outer_i);
                cEstCenter.setY(outer_j);
                for(int inner_i = outer_i-iMaskRadius; inner_i <= outer_i+iMaskRadius && !bAbort; inner_i++ )
                {
                    for(int inner_j = outer_j-iMaskRadius; inner_j <= outer_j+iMaskRadius && !bAbort; inner_j++)
                    {
                        if( inner_i < 0 || inner_j < 0 || inner_i >= iWidth || inner_j >= iHeight ||
                            qAlpha(cSrc.pixel(inner_i, inner_j)) == 0 )
                        {
                            bKeep = false;
                            bAbort = true;
                            break;
                        }

                    }

                }
                /// mask loop end
            }
            else
            {
               bKeep = false;
            }

            if( bKeep )
            {
                cDst.setPixel(outer_i,outer_j, cColor);
                if(pcCenter != NULL)
                {
                    pcCenter->setX(outer_i);
                    pcCenter->setY(outer_j);
                }
                bUserEstCenter = false;
                iArea++;
            }
        }
    }


    if(bUserEstCenter && pcCenter != NULL)
        *pcCenter = cEstCenter;

    return iArea;
}

int ImageUtils::dilate(const QImage &cSrc, QImage &cDst, int iMaskSize, QRgb cColor)
{
    int iWidth = cDst.width();
    int iHeight = cDst.height();
    int iMaskRadius = (iMaskSize-1)/2;

    /// init bg with transparent color
    cDst.fill(qRgba(255, 255, 255, 0));

    /// image loop
    bool bKeep = true;
    bool bAbort = false;
    int iArea = 0;
    for(int outer_i = 0; outer_i < iWidth; outer_i++)
    {
        for(int outer_j = 0; outer_j < iHeight; outer_j++)
        {
            /// mask loop

            /// init
            bKeep = false;
            bAbort = false;

            /// erode the dark pixel
            if(qAlpha(cSrc.pixel(outer_i, outer_j)) == 0)
            {
                for(int inner_i = outer_i-iMaskRadius; inner_i <= outer_i+iMaskRadius && !bAbort; inner_i++ )
                {
                    for(int inner_j = outer_j-iMaskRadius; inner_j <= outer_j+iMaskRadius && !bAbort; inner_j++)
                    {
                        if( inner_i < 0 || inner_j < 0 || inner_i >= iWidth || inner_j >= iHeight ||
                            qAlpha(cSrc.pixel(inner_i, inner_j)) != 0 )
                        {
                            bKeep = true;
                            bAbort = true;
                            break;
                        }

                    }

                }
                /// mask loop end
            }
            else
            {
               bKeep = true;
            }

            if( bKeep )
            {
                cDst.setPixel(outer_i,outer_j, cColor);
                iArea++;
            }
        }
    }
    return iArea;
}

int ImageUtils::close(const QImage &cSrc, QImage &cDst, int iMaskSize, QRgb cColor)
{
    QImage cTemp = cDst;
    dilate(cSrc, cTemp, iMaskSize, cColor);
    return erode(cTemp, cDst, iMaskSize, cColor);
}

void ImageUtils::overlayImage(QImage& cBaseImage, const QImage& cOverlayImage, double dOpacity)
{
    QPainter painter(&cBaseImage);
    painter.setOpacity(dOpacity);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter.drawImage(0, 0, cOverlayImage);

    painter.end();
}

void ImageUtils::blur(QImage &rcImage)
{
    int iMaskRadius = (GAUSSIAN_MATRIX_SIZE-1)/2;

    int iWidth = rcImage.width();
    int iHeight = rcImage.height();
    QImage cCopy = rcImage;
    double dWeightSum = 0;
    double dPixWeight;
    QVector3D cSum;
    int iXOffset = 0;
    int iYOffset = 0;
    for(int outer_i = 0; outer_i < iWidth; outer_i++)
    {
        for(int outer_j = 0; outer_j < iHeight; outer_j++)
        {

            if( qAlpha(cCopy.pixel(outer_i, outer_j)) == 0 )
                continue;


            cSum.setX(0);
            cSum.setY(0);
            cSum.setZ(0);
            dWeightSum = 0;

            iXOffset = -1;
            for(int inner_i = outer_i-iMaskRadius; inner_i <= outer_i+iMaskRadius; inner_i++ )
            {
                iXOffset++;
                iYOffset = -1;
                for(int inner_j = outer_j-iMaskRadius; inner_j <= outer_j+iMaskRadius; inner_j++)
                {
                    iYOffset++;
                    if( inner_i < 0 || inner_j < 0 || inner_i >= iWidth || inner_j >= iHeight )
                        continue;

                    QRgb cPixel = cCopy.pixel(inner_i, inner_j);

                    if(qAlpha(cPixel) == 0)
                        continue;


                    dPixWeight = s_adGauBlurMatrix[iXOffset][iYOffset];
                    //qDebug() << iXOffset << iYOffset << dFactor;
                    cSum += dPixWeight * QVector3D(qRed(cPixel), qGreen(cPixel), qBlue(cPixel));
                    dWeightSum += dPixWeight;
                }
            }
            cSum /= dWeightSum;
            QRgb cBlurred = qRgb(cSum.x(), cSum.y(), cSum.z());
            rcImage.setPixel(outer_i, outer_j, cBlurred);

        }
    }
}

QImage ImageUtils::hammerAitoffProjection(QImage &rcInputImg, QColor cBackgroundColor)
{
    QImage cResult(rcInputImg.size(), QImage::Format_RGBA8888);

    cResult.fill(cBackgroundColor);

    static const double sqrt2 = qSqrt(2.0);
    int iWidth = rcInputImg.width();
    int iHeight = rcInputImg.height();

    for(int i = 0; i < iWidth; i++)
    {
        double x = 2*(i*1.0/iWidth-0.5)+0.001;
        for(int j = 0; j < iHeight; j++)
        {
            double y = 2*(j*1.0/iHeight-0.5)+0.001;

            double z2 = 1 - x*x/2 - y*y/2;
            if(z2 < 0)
                continue;

            double z = sqrt(z2);
            double longitude = 2 * qAtan(sqrt2 * x * z / (2 * z2 - 1));
            if(x*longitude < 0)
                continue;
            double latitude = qAsin(sqrt2 * y * z);

            double dXMapped = (longitude+M_PI)/(M_PI*2)*iWidth;
            double dYMapped = (latitude+M_PI_2)/M_PI*iHeight;
            cResult.setPixel(i,j,bilinearSampling(rcInputImg, dXMapped, dYMapped ));

        }
    }
    return cResult;
}

Matrix<int> ImageUtils::hammerAitoffProjection(Matrix<int> &rcLabelImg)
{
    Matrix<int> cResult;
    cResult.allocate(rcLabelImg.getNumRow(), rcLabelImg.getNumCol());
    for(int i = 0; i < cResult.getNumRow(); i++)
        for(int j = 0; j < cResult.getNumCol(); j++)
            cResult[i][j] = 0;

    static const double sqrt2 = qSqrt(2.0);
    int iWidth = rcLabelImg.getNumRow();
    int iHeight = rcLabelImg.getNumCol();

    for(int i = 0; i < iWidth; i++)
    {
        double x = 2*(i*1.0/iWidth-0.5)+0.001;
        for(int j = 0; j < iHeight; j++)
        {
            double y = 2*(j*1.0/iHeight-0.5)+0.001;

            double z2 = 1 - x*x/2 - y*y/2;
            if(z2 < 0)
                continue;

            double z = sqrt(z2);
            double longitude = 2 * qAtan(sqrt2 * x * z / (2 * z2 - 1));
            if(x*longitude < 0)
                continue;
            double latitude = qAsin(sqrt2 * y * z);

            int iXMapped = int((longitude+M_PI)/(M_PI*2)*iWidth);
            int iYMapped = int((latitude+M_PI_2)/M_PI*iHeight);
            cResult[i][j] = rcLabelImg[iXMapped][iYMapped];

        }
    }
    return cResult;
}

void ImageUtils::clipVector4D(QVector4D &rcColor, double dMax)
{
    if(rcColor.x() > dMax)
        rcColor.setX(dMax);
    if(rcColor.y() > dMax)
        rcColor.setY(dMax);
    if(rcColor.z() > dMax)
        rcColor.setZ(dMax);
    if(rcColor.w() > dMax)
        rcColor.setW(dMax);
}

QRgb ImageUtils::bilinearSampling(QImage &rcInputImg, double x, double y)
{
    int iWidth = rcInputImg.width();
    int iHeight = rcInputImg.height();

    int x0 = floor(x); int x1 = min(x0+1, iWidth-1);
    int y0 = floor(y); int y1 = min(y0+1, iHeight-1);

    double x_d = x - x0;
    double y_d = y - y0;


    QRgb c00 = rcInputImg.pixel(x0,y0); double w00 = (1-x_d) * (1-y_d);
    QRgb c01 = rcInputImg.pixel(x0,y1); double w01 = (1-x_d) * (y_d);
    QRgb c10 = rcInputImg.pixel(x1,y0); double w10 = (x_d) * (1-y_d);
    QRgb c11 = rcInputImg.pixel(x1,y1); double w11 = (x_d) * (y_d);

    int red   = qRed(c00)*w00 + qRed(c01)*w01 + qRed(c10)*w10 + qRed(c11)*w11;
    int green = qGreen(c00)*w00 + qGreen(c01)*w01 + qGreen(c10)*w10 + qGreen(c11)*w11;
    int blue  = qBlue(c00)*w00 + qBlue(c01)*w01 + qBlue(c10)*w10 + qBlue(c11)*w11;
    int alpha = qAlpha(c00)*w00 + qAlpha(c01)*w01 + qAlpha(c10)*w10 + qAlpha(c11)*w11;

    return qRgba(red, green, blue, alpha);
}

void ImageUtils::calculateROIGradient(vtkImageData *pcIntensity, vtkImageData *pcLabel, vtkImageData *pcGradient)
{
    int* aiDimIntensity = pcIntensity->GetDimensions();
    int* aiDimLabel = pcLabel->GetDimensions();
    double* adSpacingIntensity = pcIntensity->GetSpacing();
    double* adSpaceingLabel = pcLabel->GetSpacing();

    if(VTKUtils::compareIntVector3(aiDimIntensity, aiDimLabel) != true)
    {
        qDebug() << "Dimension not the same!";
        return;
    }

    if(VTKUtils::compareDoubleVector3(adSpacingIntensity, adSpaceingLabel) != true)
    {
        qDebug() << "Spacing not the same";
        return;
    }



    int x0, x1, y0, y1, z0, z1;
    int iCenterLabel = 0;
    double intervalX = -0.5/adSpacingIntensity[0];
    double intervalY = -0.5/adSpacingIntensity[1];
    double intervalZ = -0.5/adSpacingIntensity[2];
    double d = 0;

    for(int z = 0; z < aiDimLabel[2]; z++)
    {
        z0 = (z-1<0) ? 0: (z-1);
        z1 = (z+1 > aiDimLabel[2]-1 ) ? (aiDimLabel[2]-1) : (z+1);
        for(int y = 0; y < aiDimLabel[1]; y++)
        {
            y0 = (y-1<0) ? 0: (y-1);
            y1 = (y+1 > aiDimLabel[1]-1 ) ? (aiDimLabel[1]-1) : (y+1);
            for(int x = 0; x < aiDimLabel[0]; x++)
            {
                x0 = (x-1<0) ? 0: (x-1);
                x1 = (x+1 > aiDimLabel[0]-1 ) ? (aiDimLabel[0]-1) : (x+1);

                iCenterLabel = qRound(pcLabel->GetScalarComponentAsDouble(x,y,z,0));

                /// x
                if(qRound(pcLabel->GetScalarComponentAsFloat(x0,y,z,0)) == iCenterLabel)
                    d = pcIntensity->GetScalarComponentAsDouble(x0,y,z,0);
                else
                    d = pcIntensity->GetScalarComponentAsDouble(x0,y,z,0)/3;

                if(qRound(pcLabel->GetScalarComponentAsFloat(x1,y,z,0)) == iCenterLabel)
                    d -= pcIntensity->GetScalarComponentAsDouble(x1,y,z,0);
                else
                    d -= pcIntensity->GetScalarComponentAsDouble(x1,y,z,0)/3;

                d *= intervalX;
                pcGradient->SetScalarComponentFromDouble(x,y,z,0,d);


                /// y
                if(qRound(pcLabel->GetScalarComponentAsFloat(x,y0,z,0)) == iCenterLabel)
                    d = pcIntensity->GetScalarComponentAsDouble(x,y0,z,0);
                else
                    d = pcIntensity->GetScalarComponentAsDouble(x,y0,z,0)/3;

                if(qRound(pcLabel->GetScalarComponentAsFloat(x,y1,z,0)) == iCenterLabel)
                    d -= pcIntensity->GetScalarComponentAsDouble(x,y1,z,0);
                else
                    d -= pcIntensity->GetScalarComponentAsDouble(x,y1,z,0)/3;

                d *= intervalY;
                pcGradient->SetScalarComponentFromDouble(x,y,z,1,d);

                /// z
                if(qRound(pcLabel->GetScalarComponentAsFloat(x,y,z0,0)) == iCenterLabel)
                    d = pcIntensity->GetScalarComponentAsDouble(x,y,z0,0);
                else
                    d = pcIntensity->GetScalarComponentAsDouble(x,y,z0,0)/3;

                if(qRound(pcLabel->GetScalarComponentAsFloat(x,y,z1,0)) == iCenterLabel)
                    d -= pcIntensity->GetScalarComponentAsDouble(x,y,z1,0);
                else
                    d -= pcIntensity->GetScalarComponentAsDouble(x,y,z1,0)/3;

                d *= intervalZ;
                pcGradient->SetScalarComponentFromDouble(x,y,z,2,d);


            }
        }
    }

//    vtkSmartPointer<vtkNIFTIImageWriter> pcWriter = vtkSmartPointer<vtkNIFTIImageWriter>::New();
//    pcWriter->SetFileName("gradient.nii");
//    pcWriter->SetInputData(pcGradient);
//    pcWriter->Update();

}

QImage ImageUtils::maskImage(QImage &rcInput, QImage &rcMask)
{
    if(rcInput.size().isEmpty() || rcInput.size() != rcMask.size())
    {
        qWarning() << "Invalid Input Image and Mask Image.";
        qWarning() << "Input Image:" << rcInput.size() << "Mask:" << rcMask.size();
        return QImage();
    }
    QImage rcResult = rcInput.convertToFormat(QImage::Format_ARGB32);

    QRgb cInputPix; QRgb cMaskPix;

    for(int i = 0; i < rcInput.width(); i++)
    {
        for(int j = 0; j < rcInput.height(); j++)
        {
            cInputPix = rcInput.pixel(i, j);
            cMaskPix  = rcMask.pixel(i, j);

            if(qAlpha(cMaskPix) == 0)
                rcResult.setPixel(i, j, qRgba(0,0,0,0));
        }
    }

    return rcResult;
}


bool ImageUtils::hammerAitoffTransform(QPointF& rcInput, QPointF& rcOutput, int iWidth, int iHeight)
{
    double latitude = (rcInput.y()/iHeight-0.5)*M_PI;
    double longitude = 2*(rcInput.x()/iWidth-0.5)*M_PI;


    double z2 = 1 + qCos(latitude) * qCos(longitude/2);
    double z = qSqrt(z2);
    double x = qCos(latitude) * qSin(longitude/2) / z;
    double y = qSin(latitude) / z;

    rcOutput.setX(int((x/2+0.5)*iWidth));
    rcOutput.setY(int((y/2+0.5)*iHeight));

    return true;
}

bool ImageUtils::blendingTextureAndLighting(QImage &rcTexture, QImage &rcLighting, QImage& rcResult)
{
    if(rcTexture.size().isEmpty() || rcTexture.size() != rcLighting.size())
    {
        qWarning() << "Invalid Texture and Lighting Map.";
        qWarning() << "Texture:" << rcTexture.size() << "Lighting:" << rcLighting.size();
        return false;
    }
    rcResult = QImage(rcTexture.size(), QImage::Format_ARGB32);

    QRgb cLightPix; QRgb cTexturePix;
    int iR, iG, iB, iA;

#define CLIP_255(component) \
    ((component) > 255 ? 255 : component);

    for(int i = 0; i < rcTexture.width(); i++)
    {
        for(int j = 0; j < rcTexture.height(); j++)
        {
            cLightPix = rcLighting.pixel(i, j);
            cTexturePix = rcTexture.pixel(i, j);
            iR = (qRed(cTexturePix)*qRed(cLightPix))/255;
            iG = (qGreen(cTexturePix)*qGreen(cLightPix))/255;
            iB = (qBlue(cTexturePix)*qBlue(cLightPix))/255;
            iA = (qAlpha(cTexturePix)*qAlpha(cLightPix))/255;

            iR = CLIP_255(iR);
            iG = CLIP_255(iG);
            iB = CLIP_255(iB);
            iA = CLIP_255(iA);

            if(iA == 0)
                rcResult.setPixel(i, j, qRgba(0, 0, 0, 255));
            else
                rcResult.setPixel(i, j, qRgba(iR, iG, iB, iA));
        }
    }

#undef CLIP_255

    return true;
}
