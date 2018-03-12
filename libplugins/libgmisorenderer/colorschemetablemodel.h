#ifndef COLORSCHEMETABLEMODEL_H
#define COLORSCHEMETABLEMODEL_H

#include <QAbstractTableModel>
#include "gitldef.h"

class ColorSchemeTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit ColorSchemeTableModel(QObject *parent = 0);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Editable:
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    ADD_CLASS_FIELD(CONCATE(QMap<int,QColor>*), pcColorMap, getColorMap, setColorMap)
    ADD_CLASS_FIELD(CONCATE(QMap<int,QString>*), pcLabelNames, getLabelNames, setLabelNames)

private:
};

#endif // COLORSCHEMETABLEMODEL_H
