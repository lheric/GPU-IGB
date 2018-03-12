#ifndef QVTKCUSTOMWIDGET_H
#define QVTKCUSTOMWIDGET_H
#include <QVTKWidget.h>

class QVTKCustomWidget : public QVTKWidget
{
    Q_OBJECT
public:
    QVTKCustomWidget(QWidget *parent);
    ~QVTKCustomWidget();
//    void cameraChangedCallback();

protected:
    void keyPressEvent ( QKeyEvent * event );

signals:
    void exitFullscreen();
};

#endif // QVTKCUSTOMWIDGET_H
