greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TEMPLATE        = lib
CONFIG         += plugin
CONFIG         += c++11

TARGET          = $$qtLibraryTarget(libfiberrenderer)
DESTDIR         = $${OUT_PWD}/../../plugins

HEADERS         = fiberrenderer.h \
    vtkfiberconnectionfilter.h \
    fiberroiconnection.h \
    vtklinecolorfilter.h \
    fiberconfigpanel.h
SOURCES         = fiberrenderer.cpp \
    vtkfiberconnectionfilter.cpp \
    fiberroiconnection.cpp \
    vtklinecolorfilter.cpp \
    fiberconfigpanel.cpp


FORMS += \
    fiberconfigpanel.ui


include(../libpluginscommon.pri)
