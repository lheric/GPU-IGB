#ifndef PARCELLATIONLABELS_H
#define PARCELLATIONLABELS_H
#include <QMap>
#include <QObject>
#include "gitldef.h"

class ParcellationLabels : public QObject
{
    Q_OBJECT
public:
    explicit ParcellationLabels(QObject *parent = 0);
    bool loadParcellation(QString strFilename);
    bool query(int iLabel, QString* pstrLabelName);
    QList<int> getKeys();
    int getNumofROIs();

    ADD_CLASS_FIELD(bool, bValid, getValid, setValid)
    ADD_CLASS_FIELD(QString, strName, getName, setName)
    ADD_CLASS_FIELD_NOSETTER( CONCATE(QMap<int, QString>), cLabelNamesTable, getLabelNamesTable)
signals:
    void updated();

public slots:
};

#endif // PARCELLATIONLABELS_H
