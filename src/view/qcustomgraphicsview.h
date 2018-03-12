#ifndef QCUSTOMGRAPHICSVIEW_H
#define QCUSTOMGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include "gitlview.h"

namespace Ui {
class QCustomGraphicsView;
}

class QCustomGraphicsView : public QGraphicsView, public GitlView
{
    Q_OBJECT

public:
    explicit QCustomGraphicsView(QWidget *parent = 0);
    ~QCustomGraphicsView();

    void onReceivedRenderedImage(GitlUpdateUIEvt &rcEvt);
protected:
    virtual void resizeEvent(QResizeEvent * event);

private:
    Ui::QCustomGraphicsView *ui;
    ADD_CLASS_FIELD_NOSETTER(QGraphicsScene, cScene, getScene)
    ADD_CLASS_FIELD_NOSETTER(QGraphicsPixmapItem, cPixmapItem, getPixmapItem)
};

#endif // QCUSTOMGRAPHICSVIEW_H
