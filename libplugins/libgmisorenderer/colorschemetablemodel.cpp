#include "colorschemetablemodel.h"
#include <QColor>

ColorSchemeTableModel::ColorSchemeTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    setColorMap(nullptr);
    setLabelNames(nullptr);
}

QVariant ColorSchemeTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    // FIXME: Implement me!
    static QVector<QString> cColHeaders {"ROI ID", "ROI Name", "Color"};
    if(Qt::DisplayRole == role)
    {
        if(Qt::Horizontal == orientation)
            return cColHeaders[section];
        else
            return QVariant();
    }
    return QVariant();
}


int ColorSchemeTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    if(this->getLabelNames() == nullptr)
        return 0;
    return this->getLabelNames()->size();
}

int ColorSchemeTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 3;
}

QVariant ColorSchemeTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if(Qt::DisplayRole == role)
    {
        if(index.column() == 0)
            return index.row();
        else if( this->getLabelNames()!= nullptr && index.column() == 1)
        {
            /// label name
            if(index.row() < this->getLabelNames()->size() )
                return this->getLabelNames()->value(index.row());
        }
        else if(index.column() == 2)
        {
            /// label color
            if(index.row() < this->getColorMap()->size() )
                return this->getColorMap()->value(index.row());
        }
    }
    else if(Qt::DecorationRole == role)
    {
        if(index.column() == 2)
        {
            /// label color
            if(index.row() < this->getColorMap()->size() )
                return this->getColorMap()->value(index.row());
        }
    }
    else if(Qt::TextAlignmentRole == role)
    {
        if(index.column() == 0)
            return int(Qt::AlignHCenter|Qt::AlignVCenter);
        else
            return int(Qt::AlignLeft|Qt::AlignVCenter);
    }
    return QVariant();
}

bool ColorSchemeTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (data(index, role) != value) {
        (*this->getColorMap())[index.row()] = value.value<QColor>();
        emit dataChanged(index, index, QVector<int>() << role);
        return true;
    }
    return true;
}

Qt::ItemFlags ColorSchemeTableModel::flags(const QModelIndex &index) const
{
    /// only the color column is editable
    if (index.column() == 2)
        return Qt::ItemIsEditable|Qt::ItemIsSelectable|Qt::ItemIsEnabled;
    return Qt::ItemIsSelectable|Qt::ItemIsEnabled;
}
