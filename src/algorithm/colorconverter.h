#ifndef COLORCONVERTER_H
#define COLORCONVERTER_H

#include <QObject>
#include <QColor>
#include <QVector3D>
class ColorConverter : public QObject
{
    Q_OBJECT
public:
    explicit ColorConverter(QObject *parent = 0);
    ~ColorConverter();

    static QColor ryb2rgb(float inR, float inY, float inB);
signals:

public slots:
};

#endif // COLORCONVERTER_H
