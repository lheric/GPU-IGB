#ifndef COLORSCHEMEDELEGATE_H
#define COLORSCHEMEDELEGATE_H

#include <QObject>
#include <QStyledItemDelegate>

class ColorSchemeDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit ColorSchemeDelegate(QObject *parent = 0);
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const override;

    void updateEditorGeometry(QWidget *editor,
                              const QStyleOptionViewItem &option,
                              const QModelIndex &index) const;
    void commitAndCloseEditor();
signals:

public slots:
};

#endif // COLORSCHEMEDELEGATE_H
