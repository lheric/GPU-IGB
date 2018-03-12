#include "qcustomgraphicsview.h"
#include "ui_qcustomgraphicsview.h"
#include <QTransform>
#include <QDebug>
QCustomGraphicsView::QCustomGraphicsView(QWidget *parent) :
    QGraphicsView(parent),
    ui(new Ui::QCustomGraphicsView)
{
    setModuleName("QCustomGraphicsView");
    ui->setupUi(this);
    this->setScene(&m_cScene);
    m_cScene.setBackgroundBrush(Qt::black);
    m_cPixmapItem.setFlag(QGraphicsItem::ItemIsMovable, true);
    m_cScene.addItem(&m_cPixmapItem);


    this->listenToParams("raycasting_opencl_rendered", MAKE_CALLBACK(QCustomGraphicsView::onReceivedRenderedImage));
}

QCustomGraphicsView::~QCustomGraphicsView()
{
    delete ui;
}

void QCustomGraphicsView::onReceivedRenderedImage(GitlUpdateUIEvt &rcEvt)
{
    QImage cRendered = rcEvt.getParameter("raycasting_opencl_rendered").value<QImage>();
    m_cPixmapItem.setPixmap(QPixmap::fromImage(cRendered));
    this->fitInView(&m_cPixmapItem, Qt::KeepAspectRatio);
    //    pItem->setTransformOriginPoint();
}

void QCustomGraphicsView::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)

    this->fitInView(&m_cPixmapItem, Qt::KeepAspectRatio);
}
