#ifndef PARCELLATIONCOLORSCHEME_H
#define PARCELLATIONCOLORSCHEME_H

#include "gitldef.h"
#include <QObject>
#include <QMap>
#include <QColor>
#include <QString>

class ParcellationColorScheme : public QObject
{
    Q_OBJECT
public:
    explicit ParcellationColorScheme(QObject *parent = 0);
    void autoFill(int iLabelMax);
    bool loadColorScheme(QString strFilename);
    bool query(int iLabel, QColor* pcLabelColor);

    ADD_CLASS_FIELD(bool, bValid, getValid, setValid)
    ADD_CLASS_FIELD(QString, strName, getName, setName)
    /// Color Scheme <label> to <color> map
    ADD_CLASS_FIELD(CONCATE(QMap<int, QColor>), cColorMap, getColorMap, setColorMap)

signals:
    void updated(ParcellationColorScheme* pcNewScheme);

public slots:
};

#endif // PARCELLATIONCOLORSCHEME_H
