#include "connectpipelinecommand.h"
#include "model/modellocator.h"
#include "gitlupdateuievt.h"
#include "gitlivkcmdevt.h"
#include "vtkLight.h"
#include "vtkLightCollection.h"
#include "vtkCamera.h"
#include <QTabWidget>
#include <QSplitter>
#include <QDebug>

static void CameraModifiedCallback(vtkObject* caller,
                                   long unsigned int vtkNotUsed(eventId),
                                   void* vtkNotUsed(clientData),
                                   void* vtkNotUsed(callData) )
{
    static ModelLocator* pcModel = ModelLocator::getInstance();
//    static OpenCLContext* pcContext = &(pcModel->getRayCastOpenCLRenderer().getOpenCLContext());
//    std::cout << caller->GetClassName() << " modified" << std::endl;


//    vtkCamera* camera = static_cast<vtkCamera*>(caller);
    // print the interesting stuff
//    qDebug()  << "\tPosition: "
//              << camera->GetPosition()[0] << ", "
//              << camera->GetPosition()[1] << ", "
//              << camera->GetPosition()[2] ;
//    qDebug()  << "\tFocal point: "
//              << camera->GetFocalPoint()[0] << ", "
//              << camera->GetFocalPoint()[1] << ", "
//              << camera->GetFocalPoint()[2] ;

//    qDebug()  << "\tView Up: "
//              << camera->GetViewUp()[0] << ", "
//              << camera->GetViewUp()[1] << ", "
//              << camera->GetViewUp()[2] ;

}

ConnectPipelineCommand::ConnectPipelineCommand(QObject *parent) :
    GitlAbstractCommand(parent)
{
    setInWorkerThread(false);
}

bool ConnectPipelineCommand::execute( GitlCommandParameter& rcInputArg, GitlCommandParameter& rcOutputArg )
{
    Q_UNUSED(rcOutputArg)

    ModelLocator* pcModel = ModelLocator::getInstance();


    /// render window (created by Qt GUI. Saving the pointer into model)
    vtkRenderWindow* pcRenderWin = (vtkRenderWindow*)(rcInputArg.getParameter("render_win").value<void*>());
    pcModel->setRenderWindow(pcRenderWin);

    /// plugin renderer property tabs (created by Qt GUI. Saving the pointer into model)
    /// Plugin loader will insert the plugin config panel into the tabs when it init the plugin.
    QTabWidget* pcPluginPropTabs = (QTabWidget*)(rcInputArg.getParameter("plugin_prop_tabs").value<void*>());
    pcModel->setPluginPropTabs(pcPluginPropTabs);

    /// center widget in mainwin.
    /// Plugins may add some more widgets to it
    QSplitter* pcCenterWidgetSplitter = (QSplitter*)(rcInputArg.getParameter("center_widget_splitter").value<void*>());
    pcModel->setCenterWidgetSplitter(pcCenterWidgetSplitter);

    /// renderer && config for hardware depth sorting
    pcRenderWin->SetAlphaBitPlanes(1);
    pcRenderWin->SetMultiSamples(0);
    pcModel->setRender(vtkRenderer::New());
    pcRenderWin->StereoCapableWindowOn();
    pcRenderWin->SetStereoTypeToCheckerboard();
    pcRenderWin->StereoRenderOff();                 // default off
    pcModel->getRenderer()->TwoSidedLightingOff();
    pcModel->getRenderer()->SetUseDepthPeeling(1);
    pcModel->getRenderer()->SetMaximumNumberOfPeels(20);
    pcModel->getRenderer()->SetOcclusionRatio(0.2);
    pcModel->getRenderWindow()->AddRenderer(pcModel->getRenderer());

    /// backgound color
    pcModel->getRenderer()->SetBackground(0, 0, 0);

    /// interactor
    pcModel->setActorStyle(vtkInteractorStyleTrackballActor::New());
    pcModel->setCameraStyle(vtkInteractorStyleTrackballCamera::New());

    /// camera callback
    pcModel->setCamCallback(vtkSmartPointer<vtkCallbackCommand>::New());
    pcModel->getCamCallback()->SetCallback(CameraModifiedCallback);
    pcModel->getRenderer()->GetActiveCamera()->AddObserver(vtkCommand::ModifiedEvent,
                                                           pcModel->getCamCallback());

    /// axes actor
    pcModel->setAxesActor(vtkAxesActor::New());
    pcModel->setOrientationWidget(vtkOrientationMarkerWidget::New());
    pcModel->getOrientationWidget()->SetOrientationMarker(pcModel->getAxesActor());
    pcModel->getOrientationWidget()->SetInteractor( pcModel->getRenderWindow()->GetInteractor() );
    pcModel->getOrientationWidget()->SetViewport( 0.0, 0.0, 0.2, 0.2 );
    pcModel->getOrientationWidget()->SetEnabled(true);
    pcModel->getOrientationWidget()->InteractiveOff();

    /// picker
    pcModel->setCellPicker(vtkCellPicker::New());
    pcModel->setGMPointLocator(vtkPointLocator::New());

    return true;
}
