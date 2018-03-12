#include "colorschemedelegate.h"
#include <QColorDialog>
#include <QDebug>

ColorSchemeDelegate::ColorSchemeDelegate(QObject *parent) : QStyledItemDelegate(parent)
{

}

QWidget *ColorSchemeDelegate::createEditor(QWidget *parent,
                                    const QStyleOptionViewItem &option,
                                    const QModelIndex &index) const

{

    if (index.column() == 2) {
        QColorDialog* pcDialog = new QColorDialog(parent);
        connect(pcDialog, &QColorDialog::accepted,
                this, &ColorSchemeDelegate::commitAndCloseEditor);
        return pcDialog;
    } else {
        return QStyledItemDelegate::createEditor(parent, option, index);
    }
}

void ColorSchemeDelegate::setEditorData(QWidget *editor,
                                 const QModelIndex &index) const
{

    if (index.column() == 2) {
        ((QColorDialog*)(editor))->setCurrentColor(index.data().value<QColor>());
    } else {
        QStyledItemDelegate::setEditorData(editor, index);
    }
}

void ColorSchemeDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                const QModelIndex &index) const
{
    model->setData(index, QVariant::fromValue<QColor>(((QColorDialog*)(editor))->currentColor()), Qt::DisplayRole);
}

void ColorSchemeDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    /// override the default behavior
    Q_UNUSED(editor)
    Q_UNUSED(option)
    Q_UNUSED(index)
}

void ColorSchemeDelegate::commitAndCloseEditor()
{
    QColorDialog *editor = qobject_cast<QColorDialog*>(sender());
    emit commitData(editor);
    emit closeEditor(editor);
}
