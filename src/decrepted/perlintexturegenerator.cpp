#include "perlintexturegenerator.h"
#include <QPixmap>
#include <QPainter>
#include <QDebug>
#include <QTime>

PerlinTextureGenerator::PerlinTextureGenerator()
{

}

PerlinTextureGenerator::~PerlinTextureGenerator()
{

}

QString PerlinTextureGenerator::generateContourTexture(const QString &strFilepath, const QVector<double> &adTimeSeriesData)
{
    QImage cOriImg;
    if( cOriImg.load(strFilepath) == false )
        qCritical() << "Texture Generator Cannot Load" << strFilepath;

    QImage cGrayScaleImg = cOriImg;



    int iMaskSize = 3;

    QPainter cPainter;
    cPainter.begin(&cOriImg);

    cPainter.end();

    return QString();
}

void PerlinTextureGenerator::convertToGrayScale(const QImage& cSrc, QImage &cDst)
{
//    if( cSrc.size() != cDst.size() )
//    {
//        qCritical() << "src and dst size differs..";
//        return;
//    }
//    int iWidth = cSrc.width();
//    int iHeight = cSrc.height();
//    int iGray = 0;
//    for(int i = 0; i < iWidth; i++)
//    {
//        for(int j = 0; j < iHeight; j++)
//        {
//            iGray = qGray(cSrc.pixel(i,j);
//            cDst.setPixel(i,j,  e));
//        }
//    }
}

void PerlinTextureGenerator::erode(const QImage &cSrc, QImage &cDst, int iMaskSize, QRgb cColor)
{
    int iWidth = cDst.width();
    int iHeight = cDst.height();
    int iMaskRadius = (iMaskSize-1)/2;

    /// init bg with transparent color
    cDst.fill(qRgba(255, 255, 255, 0));

    /// image loop
    bool bKeep = true;
    bool bAbort = false;
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
                cDst.setPixel(outer_i,outer_j, cColor);
        }
    }
}

void PerlinTextureGenerator::overlayImage(QImage& cBaseImage, const QImage& cOverlayImage)
{
    QPainter painter(&cBaseImage);

    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter.drawImage(0, 0, cOverlayImage);

    painter.end();
}

QImage PerlinTextureGenerator::generatePerlineTexture(const QVector<double>& cRYBWeights,
                                                      double dDensity,
                                                      int iROILabel,
                                                      QString& strTexturePath,
                                                      int iWidth,
                                                      int iHeight)
{
    if(cRYBWeights.size() != 3)
    {
        qWarning() << "Three Weights are Needed for Perlin Noise Texture.. Current Array Size is" << cRYBWeights.size();
        return QImage();
    }

    /// init
    qsrand(QTime::currentTime().msec());
    m_iHeight = iHeight;
    m_iWidth = iWidth;
    m_cResult = QImage(m_iWidth, m_iHeight, QImage::Format_ARGB32);
    m_cRandomNoise = QImage(m_iWidth, m_iHeight, QImage::Format_RGB32);
    m_cSmoothed = QImage(m_iWidth, m_iHeight, QImage::Format_RGB32);


    cColor1 = QVector3D(1.0, 0.0, 0.0); //r
    cColor2 = QVector3D(0.0, 1.0, 0.0); //y
    cColor3 = QVector3D(0.0, 0.0, 1.0); //b
    cColor4 = QVector3D(0.3, 0.5, 0.0); //1
    cColor5 = QVector3D(0.5, 0.3, 0.1); //2

    QImage cPerlinNoise(m_iWidth, m_iHeight, QImage::Format_RGB32);

    ///
    double dPro1 = cRYBWeights[0];
    double dPro2 = cRYBWeights[1];
    double dPro3 = cRYBWeights[2];

    m_acAllWeights.clear();
    // first
    this->generateRandomNoise();
    this->generateSmoothNoise();
    this->getBlendingResult(cPerlinNoise, dDensity);
    m_acAllWeights.push_back(cPerlinNoise);

    // second
    this->generateRandomNoise();
    this->generateSmoothNoise();
    this->getBlendingResult(cPerlinNoise, dDensity);
    m_acAllWeights.push_back(cPerlinNoise);

    // third
    this->generateRandomNoise();
    this->generateSmoothNoise();
    this->getBlendingResult(cPerlinNoise, dDensity);
    m_acAllWeights.push_back(cPerlinNoise);

    // result
    for(int x = 0; x < m_iWidth; x++)
    {
        for(int y = 0; y < m_iHeight; y++)
        {
            double dWeight1 = qGray(m_acAllWeights[0].pixel(x,y))*dPro1/255.0;
            double dWeight2 = qGray(m_acAllWeights[1].pixel(x,y))*dPro2/255.0;
            double dWeight3 = qGray(m_acAllWeights[2].pixel(x,y))*dPro3/255.0;
            m_cResult.setPixel(x, y, ryb2rgb(dWeight1, dWeight2, dWeight3).rgb());
        }
    }

//        m_cResult.save(QString("#%1_%2_%3_%4.png").arg(iLineNum)
//                       .arg(dPro1, 0, 'f', 1)
//                       .arg(dPro2, 0, 'f', 1)
//                       .arg(dPro3, 0, 'f', 1));

    strTexturePath = QString("#%1.png").arg(iROILabel);
    m_cResult.save(strTexturePath);


    return m_cResult;
}

///////
void PerlinTextureGenerator::getBlendingResult(QImage& cImg, double dDensity)
{
    for (int x = 0; x < m_iWidth; x++)
    {
        for (int y = 0; y < m_iHeight; y++)
        {
            double size = dDensity;
            double value = 0.0;
            double initialSize = size;

            while(size >= 1)
            {
                value += smoothNoiseInterpolation(x / size, y / size) * size;
                size /= 2.0;
            }

            value = value / (initialSize*2);
            double percent = value/255;
            double oneMinusPercent = 1-percent;
            cImg.setPixel(x, y, qRgb(255*percent,255*percent,255*percent));
        }
    }
}

void PerlinTextureGenerator::generateSmoothNoise()
{
    for (int x = 0; x < m_iWidth; x++)
    {
        for (int y = 0; y < m_iHeight; y++)
        {

            int iGray = smoothNoiseInterpolation(x/8.0,y/8.0);
            m_cSmoothed.setPixel(x,y, qRgb(iGray,iGray,iGray));
        }
    }
}

double PerlinTextureGenerator::smoothNoiseInterpolation(double x, double y)
{
   //get fractional part of x and y
   double fractX = x - int(x);
   double fractY = y - int(y);

   //wrap around
   int x1 = (int(x) + m_iWidth)  % m_iWidth;
   int y1 = (int(y) + m_iHeight) % m_iHeight;

   //neighbor values
   int x2 = (x1 + m_iWidth - 1) % m_iWidth;
   int y2 = (y1 + m_iHeight - 1) % m_iHeight;

   //smooth the noise with bilinear interpolation
   double value = 0.0;
   value += fractX       * fractY       * qGray(m_cRandomNoise.pixel(x1,y1));
   value += fractX       * (1 - fractY) * qGray(m_cRandomNoise.pixel(x1,y2));
   value += (1 - fractX) * fractY       * qGray(m_cRandomNoise.pixel(x2,y1));
   value += (1 - fractX) * (1 - fractY) * qGray(m_cRandomNoise.pixel(x2,y2));

   return value;

}

QColor PerlinTextureGenerator::ryb2rgb(float inR, float inY, float inB)
{

    const static QVector3D f000(1.0,     1.0,        1.0);
    const static QVector3D f001(0.163,   0.373,      0.6);
    const static QVector3D f010(1.0,     1.0,        0.0);
    const static QVector3D f011(0.0,     0.66,       0.2);
    const static QVector3D f100(1.0,     0.0,        0.0);
    const static QVector3D f101(0.5,     0.5,        0.0);
    const static QVector3D f110(1.0,     0.5,        0.0);
    const static QVector3D f111(0.2,     0.094,      0.0);
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

void PerlinTextureGenerator::generateRandomNoise()
{
    for (int x = 0; x < m_iWidth; x++)
    {
        for (int y = 0; y < m_iHeight; y++)
        {
            int iRand = qrand() % 255;
            m_cRandomNoise.setPixel(x,y, qRgb(iRand, iRand, iRand));
        }
    }
}
