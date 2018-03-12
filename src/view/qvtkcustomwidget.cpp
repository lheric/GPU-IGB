#include "qvtkcustomwidget.h"
#include <QDebug>
#include <QKeyEvent>
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkCamera.h"
#include "vtkRendererCollection.h"
QVTKCustomWidget::QVTKCustomWidget(QWidget *parent) :
    QVTKWidget(parent)
{

    ///
    //this->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->GetActiveCamera()->AddObserver(vtkCommand::ModifiedEvent, this, &QVTKCustomWidget::cameraChangedCallback);
}

QVTKCustomWidget::~QVTKCustomWidget()
{

}

void QVTKCustomWidget::keyPressEvent(QKeyEvent *event)
{

    if(event->type() == QEvent::KeyPress)
    {
        if( event->key() == Qt::Key_Escape )
        {
            emit exitFullscreen();
        }
    }
    QVTKWidget::keyPressEvent(event);
}

//void QVTKCustomWidget::cameraChangedCallback()
//{
////    qDebug() << "changed";
//}

