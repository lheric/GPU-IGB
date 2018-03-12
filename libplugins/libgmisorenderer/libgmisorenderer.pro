greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TEMPLATE        = lib
CONFIG         += plugin
CONFIG         += c++11

TARGET          = $$qtLibraryTarget(libgmisorenderer)
DESTDIR         = $${OUT_PWD}/../../plugins

HEADERS         = gmisorenderer.h \
    gmisoconfigpanel.h \
    ../../src/model/roi.h \
    ../../src/model/rom.h \
    ../../src/algorithm/vtkutils.h \
    ../../src/algorithm/imageutils.h \
    ../../src/model/parcellationlabels.h \
    ../../src/model/parcellationcolorscheme.h \
    colorschemetablemodel.h \
    colorschemedelegate.h
SOURCES         = gmisorenderer.cpp \
    gmisoconfigpanel.cpp \
    ../../src/model/roi.cpp \
    ../../src/model/rom.cpp \
    ../../src/algorithm/vtkutils.cpp \
    ../../src/algorithm/imageutils.cpp \
    ../../src/model/parcellationlabels.cpp \
    ../../src/model/parcellationcolorscheme.cpp \
    colorschemetablemodel.cpp \
    colorschemedelegate.cpp


FORMS += \
    gmisoconfigpanel.ui

include(../libpluginscommon.pri)
