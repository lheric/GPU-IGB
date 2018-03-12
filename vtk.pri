#--------------------
#  CONFIG VTK HERE  #
#--------------------
VTK_ROOT =  D:\VTK-7.0.0_Install # change the path when you switch between release and debug version
VTK_MAIN_VERSION = 7.0


unix:!macx{
    VTK_LIB_FILES = $$files($${VTK_ROOT}/lib/*.so)

    INCLUDEPATH += $${VTK_ROOT}/include/vtk-$${VTK_MAIN_VERSION}
    LIBS += -L$${VTK_ROOT}/lib

    for(FILE, VTK_LIB_FILES) {
        BASENAME = $$basename(FILE)
        SUFFIX_REMOVED = $$replace(BASENAME,\.so$,)
        PREFIX_REMOVED = $$replace(SUFFIX_REMOVED, ^lib, )
        LIBS += -l$$PREFIX_REMOVED
    }

}


win32{
    VTK_LIB_FILES = $$files($${VTK_ROOT}\lib\*.lib)

    QMAKE_LFLAGS += /LARGEADDRESSAWARE
    INCLUDEPATH += $${VTK_ROOT}\include\vtk-$${VTK_MAIN_VERSION}
    LIBS += -L$${VTK_ROOT}\lib


    for(FILE, VTK_LIB_FILES) {
        BASENAME = $$basename(FILE)
        LIBS += -l$$replace(BASENAME,\.lib,)
    }
}

macx{
    VTK_LIB_FILES = $$files($${VTK_ROOT}/lib/*.dylib)

    INCLUDEPATH += $${VTK_ROOT}/include/vtk-$${VTK_MAIN_VERSION}
    LIBS += -L$${VTK_ROOT}/lib

    for(FILE, VTK_LIB_FILES) {
        BASENAME = $$basename(FILE)
        SUFFIX_REMOVED = $$replace(BASENAME,\.dylib$,)
        PREFIX_REMOVED = $$replace(SUFFIX_REMOVED, ^lib, )
        LIBS += -l$$PREFIX_REMOVED
    }
}
