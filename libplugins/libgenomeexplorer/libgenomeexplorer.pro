greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
QT += charts
TEMPLATE        = lib
CONFIG         += plugin
CONFIG         += c++11

TARGET          = $$qtLibraryTarget(libgenomeexplorer)
DESTDIR         = $${OUT_PWD}/../../plugins

HEADERS         = genomeexplorer.h \
    genomeexplorermainwin.h \
    genomechart.h \
    heatmap.h \
    genomeconfigpanel.h \
    calculation_backend/Common/CArray1D.h \
    calculation_backend/Common/CArray1DTCP.h \
    calculation_backend/Common/CArray2D.h \
    calculation_backend/Common/CArray2DTCP.h \
    calculation_backend/Common/Sockets.h \
    calculation_backend/Common/Timer.h \
    calculation_backend/AnovaTestClient/clientdriver.h \
    heatmapgraphicsview.h \
    genomegraphicsview.h \
    brainexpvolrenderwidget.h \
    genomeexplorermodel.h \
    heatmapwidget.h \
    labelgraphicsview.h \
    brainexplorerwidget.h \
    heatmapelementrectitem.h \
    thumbnailgraphicsview.h

SOURCES         = genomeexplorer.cpp \
    genomeexplorermainwin.cpp \
    genomechart.cpp \
    heatmap.cpp \
    genomeconfigpanel.cpp \
    calculation_backend/Common/Sockets.cpp \
    calculation_backend/AnovaTestClient/clientdriver.cpp \
    heatmapgraphicsview.cpp \
    genomegraphicsview.cpp \
    brainexpvolrenderwidget.cpp \
    genomeexplorermodel.cpp \
    heatmapwidget.cpp \
    labelgraphicsview.cpp \
    brainexplorerwidget.cpp \
    heatmapelementrectitem.cpp \
    thumbnailgraphicsview.cpp

include(../libpluginscommon.pri)

FORMS += \
    genomeexplorermainwin.ui \
    genomeconfigpanel.ui \
    heatmapwidget.ui \
    brainexplorerwidget.ui

LIBS += -lWs2_32
