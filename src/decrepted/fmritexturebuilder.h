#ifndef FMRITEXTUREBUILDER_H
#define FMRITEXTUREBUILDER_H

#include <QObject>
#include "gitldef.h"
#include "algorithm/contourtexturegenerator.h"
#include "algorithm/stripetexturegenerator.h"

class LabelSurface;
class Subject;

/**
 * @brief The ROITextureType enum texture types
 * TEXTURE_NONE - no texture
 * TEXTURE_SCOLOR - single color
 * TEXTURE_STRIPE - stripes
 * TEXTURE_CIRCLE - concentric circles
 * TEXTURE_CONTOUR - contours
 */
enum TextureType {TEXTURE_NONE = 0, TEXTURE_SCOLOR, TEXTURE_STRIPE, TEXTURE_CIRCLE, TEXTURE_CONTOUR};

class FMRITextureBuilder: public QObject
{
    Q_OBJECT
public:
    explicit FMRITextureBuilder(QObject *parent = 0);
    ~FMRITextureBuilder();
    void init();

    void setLabelSurface(LabelSurface* pcLabelSurface);

    void setTextureType(TextureType eType);
    void setEnable(bool bEnabled);

public slots:
    void buildTexture();

    ADD_CLASS_FIELD_NOSETTER(bool, bEnable, getEnable)
    ADD_CLASS_FIELD_NOSETTER(LabelSurface*, pcLabelSurface, getLabelSurface)
    ADD_CLASS_FIELD(Subject*, pcSubject, getSubject, setSubject)

    ADD_CLASS_FIELD(AbstractTextureGenerator*, pcTextureGenerator, getTextureGenerator, setTextureGenerator)
    ADD_CLASS_FIELD_NOSETTER(TextureType, eTextureType, getTextureType)
};

#endif // FMRITEXTUREBUILDER_H
