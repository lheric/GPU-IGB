#ifndef MODELLOCATOR_H
#define MODELLOCATOR_H
/// vtk define & include
//#define vtkRenderingCore_AUTOINIT 4(vtkInteractionStyle,vtkRenderingFreeType,vtkRenderingFreeTypeOpenGL,vtkRenderingOpenGL)
//#define vtkRenderingVolume_AUTOINIT 1(vtkRenderingVolumeOpenGL)


#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <QPixmap>
#include <QVector>
#include <QMutex>
#include <QTabWidget>
#include <QSplitter>
#include "gitlmodule.h"
#include "preferences.h"
#include "graymatter.h"
#include <vtkPointPicker.h>
#include <vtkCellPicker.h>
#include <vtkPointPicker.h>
#include <vtkPointLocator.h>
#include <vtkPropPicker.h>
#include <vtkCellLocator.h>
#include <vtkCallbackCommand.h>
#include "fiber.h"
#include "subject.h"
#include "model/renderengine/pluginrendererloader.h"

/*!
 * \brief The ModelLocator class
 *  This class serves as the model, saving the state of the system
 *  Singleton Pattern
 */

class ModelLocator : public GitlModule
{
public:
    /// preference
    ADD_CLASS_FIELD_NOSETTER(Preferences, cPreferences, getPreferences)   ///< Setting for cache directory, decoder path, etc.

    /// subject
    ADD_CLASS_FIELD_NOSETTER(Subject, cSubject, getSubject)

    /// renderer
    ADD_CLASS_FIELD(vtkSmartPointer<vtkRenderer>, pcRenderer, getRenderer, setRender)

    /// interactors (move each object individually or move camera)
    ADD_CLASS_FIELD(vtkSmartPointer<vtkInteractorStyleTrackballActor>,  pcActorStyle,  getActorStyle,  setActorStyle)
    ADD_CLASS_FIELD(vtkSmartPointer<vtkInteractorStyleTrackballCamera>, pcCameraStyle, getCameraStyle, setCameraStyle)

    /// axes indicator widget
    ADD_CLASS_FIELD(vtkSmartPointer<vtkAxesActor>, pcAxesActor, getAxesActor, setAxesActor)
    ADD_CLASS_FIELD(vtkSmartPointer<vtkOrientationMarkerWidget>, pcOrientationWidget, getOrientationWidget, setOrientationWidget)


    /// render window
    ADD_CLASS_FIELD(vtkSmartPointer<vtkRenderWindow>, pcRenderWindow, getRenderWindow, setRenderWindow)

    /// plugin renderer property tabs
    ADD_CLASS_FIELD(QTabWidget*, pcPluginPropTabs, getPluginPropTabs, setPluginPropTabs)

    /// center widget splitter in mainwin
    ADD_CLASS_FIELD(QSplitter*, pcCenterWidgetSplitter, getCenterWidgetSplitter, setCenterWidgetSplitter)

    /// camera call back
    ADD_CLASS_FIELD(vtkSmartPointer<vtkCallbackCommand>, pcCamCallback, getCamCallback, setCamCallback)

    /// picker
    ADD_CLASS_FIELD(vtkSmartPointer<vtkCellPicker>, pcCellPicker, getCellPicker, setCellPicker)
    ADD_CLASS_FIELD(vtkSmartPointer<vtkPointLocator>, pcGMPointLocator, getGMPointLocator, setGMPointLocator)

    /// plugin renderer loader
    ADD_CLASS_FIELD_NOSETTER(PluginRendererLoader, cPluginLoader, getPluginLoader)

public:
    /**
      * SINGLETON ( design pattern )
      */
    SINGLETON_PATTERN_DECLARE(ModelLocator)
private:
    explicit ModelLocator();
public:
    ~ModelLocator();

};

#endif // MODELLOCATOR_H
