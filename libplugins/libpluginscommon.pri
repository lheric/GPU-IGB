CONFIG += c++11

INCLUDEPATH  += ../../src \
                ../../libgitlmvc/src \
                ../../libgitlmvc/libgitlevtbus/src \
                ../../libtrkfileio

LIBS += -L../../libgitlmvc/libgitlevtbus -L../../libgitlmvc
CONFIG(debug, debug|release){
    LIBS += -lGitlMVCd -lGitlEvtBusd
}
CONFIG(release, debug|release){
    LIBS += -lGitlMVC  -lGitlEvtBus
}


HEADERS         += ../../src/algorithm/commonutils.h
SOURCES         += ../../src/algorithm/commonutils.cpp


include (../vtk.pri)
