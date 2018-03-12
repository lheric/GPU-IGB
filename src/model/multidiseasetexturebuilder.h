#ifndef MULTIDISEASETEXTUREBUILDER_H
#define MULTIDISEASETEXTUREBUILDER_H

#include <QObject>
#include "gitldef.h"

class Subject;
class LabelSurface;

class MultiDiseaseTextureBuilder : public QObject
{
    Q_OBJECT
public:
    explicit MultiDiseaseTextureBuilder(QObject *parent = 0);
    ~MultiDiseaseTextureBuilder();
    void init();

    void setLabelSurface(LabelSurface* pcLabelSurface);

    void setEnable(bool bEnabled);

signals:

public slots:
    void buildTexture();

    ADD_CLASS_FIELD_NOSETTER(bool, bEnable, getEnable)
    ADD_CLASS_FIELD_NOSETTER(LabelSurface*, pcLabelSurface, getLabelSurface)
    ADD_CLASS_FIELD(Subject*, pcSubject, getSubject, setSubject)
};

#endif // MULTIDISEASETEXTUREBUILDER_H
