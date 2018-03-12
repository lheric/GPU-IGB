#-------------------------------------------------
#
# Project created by QtCreator 2014-10-11T21:05:09
#
#-------------------------------------------------
#-------------------------------------------------
QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BECA
TEMPLATE = app

CONFIG += c++11



SOURCES += main.cpp\
    algorithm/colorconverter.cpp \
    algorithm/commonutils.cpp \
    algorithm/csvmatrixreader.cpp \
    algorithm/imageutils.cpp \
    algorithm/vtkutils.cpp \
    command/appfrontcontroller.cpp \
    command/busydialogcommand.cpp \
    command/changebgcolorcommand.cpp \
    command/changerenderingprefcommand.cpp \
    command/connectpipelinecommand.cpp \
    command/openfiberfilecommand.cpp \
    command/opengraymatterimagecommand.cpp \
    command/opengraymatterparcelcommand.cpp \
    command/openlabelnamescommand.cpp \
    command/pickcoordinatecommand.cpp \
    command/reloadpluginrenderercommand.cpp \
    command/rerendercommand.cpp \
    command/resetcameracommand.cpp \
    command/switchplugincommand.cpp \
    model/fiber.cpp \
    model/graymatter.cpp \
    model/intensityvolume.cpp \
    model/labelvolume.cpp \
    model/modellocator.cpp \
    model/parcellationlabels.cpp \
    model/preferences.cpp \
    model/renderengine/pluginrendererloader.cpp \
    model/roi.cpp \
    model/rom.cpp \
    model/subject.cpp \
    view/aboutdialog.cpp \
    view/affinematrixform.cpp \
    view/busydialog.cpp \
    view/controlpanel.cpp \
    view/interactorstylewidget.cpp \
    view/messageviewer.cpp \
    view/pluginfilteritem.cpp \
    view/pluginfilterlist.cpp \
    view/pluginpropertytabs.cpp \
    view/qcustomgraphicsview.cpp \
    view/qvtkcirclegraphview.cpp \
    view/qvtkcustomwidget.cpp \
    view/mainwindow.cpp \
    algorithm/vtktrackvistrkreader.cpp \
    command/changeinteractorstylecommand.cpp \
    model/parcellationcolorscheme.cpp

HEADERS  +=     algorithm/colorconverter.h \
    algorithm/commonutils.h \
    algorithm/csvmatrixreader.h \
    algorithm/imageutils.h \
    algorithm/vtkutils.h \
    command/appfrontcontroller.h \
    command/busydialogcommand.h \
    command/changebgcolorcommand.h \
    command/changeinteractorstylecommand.h \
    command/changerenderingprefcommand.h \
    command/connectpipelinecommand.h \
    command/openfiberfilecommand.h \
    command/opengraymatterimagecommand.h \
    command/opengraymatterparcelcommand.h \
    command/openlabelnamescommand.h \
    command/pickcoordinatecommand.h \
    command/reloadpluginrenderercommand.h \
    command/rerendercommand.h \
    command/resetcameracommand.h \
    command/switchplugincommand.h \
    model/fiber.h \
    model/graymatter.h \
    model/intensityvolume.h \
    model/labelvolume.h \
    model/matrix.h \
    model/modellocator.h \
    model/parcellationlabels.h \
    model/preferences.h \
    model/renderengine/abstractpluginrenderer.h \
    model/renderengine/pluginrendererloader.h \
    model/roi.h \
    model/rom.h \
    model/subject.h \
    view/aboutdialog.h \
    view/affinematrixform.h \
    view/busydialog.h \
    view/controlpanel.h \
    view/interactorstylewidget.h \
    view/mainwindow.h \
    view/messageviewer.h \
    view/pluginfilteritem.h \
    view/pluginfilterlist.h \
    view/pluginpropertytabs.h \
    view/qcustomgraphicsview.h \
    view/qvtkcirclegraphview.h \
    view/qvtkcustomwidget.h \
    algorithm/vtktrackvistrkreader.h \
    model/parcellationcolorscheme.h

FORMS    += view/mainwindow.ui \
    view/messageviewer.ui \
    view/aboutdialog.ui \
    view/busydialog.ui \
    view/controlpanel.ui \
    view/interactorstylewidget.ui \
    view/affinematrixform.ui \
    view/qcustomgraphicsview.ui \
    view/pluginfilteritem.ui


#include & libs
INCLUDEPATH += .\
               ../libgitlmvc/libgitlevtbus/src \
               ../libgitlmvc/src \
               ../libtrkfileio

LIBS += -L$${OUT_PWD}/../libgitlmvc/libgitlevtbus -L$${OUT_PWD}/../libgitlmvc -L$${OUT_PWD}/../libtrkfileio

CONFIG(debug, debug|release){
    LIBS += -lGitlMVCd -lGitlEvtBusd -lTrkFileIOd
}
CONFIG(release, debug|release){
    LIBS += -lGitlMVC  -lGitlEvtBus -lTrkFileIO
}

# vtk
include(../vtk.pri)


# OpenCL
DISABLE_OPENCL {
    message("OpenCL Disabled\n");

} else {
    macx{
        LIBS += -framework OpenCL
    } else {
        INCLUDEPATH += "C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v8.0\include"
        LIBS += -L"C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v8.0\lib\x64"
        LIBS += -lOpenCL
    }
    message("OpenCL Enabled\n");
}


#icon
RC_FILE = resources/logo/BECA_LOGO.rc




RESOURCES += \
    assets/assets.qrc
