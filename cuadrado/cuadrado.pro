#-------------------------------------------------
#
# Project created by QtCreator 2017-05-30T15:02:03
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = cuadrado
TEMPLATE = app
DEFINES += NO_DEBUG_ARUCO

unix:LIBS += "/usr/local/lib/libopencv_core.so"         # OpenCV
unix:LIBS += "/usr/local/lib/libopencv_highgui.so"      # OpenCV
unix:LIBS += "/usr/local/lib/libopencv_imgproc.so"      # OpenCV
unix:LIBS += "/usr/local/lib/libopencv_objdetect.so"    # OpenCV
unix:LIBS += "/usr/local/lib/libopencv_calib3d.so"      # OpenCV
unix:LIBS += "/usr/local/lib/libopencv_ml.so"           # OpenCV
unix:LIBS += "/usr/local/lib/libopencv_videoio.so"      # OpenCv
unix:LIBS += "/usr/local/lib/libopencv_video.so"      # OpenCv

SOURCES += \
    main.cpp \
    widget.cpp \
    aruco/subpixelcorner.cpp \
    aruco/markerdetector.cpp \
    aruco/marker.cpp \
    aruco/highlyreliablemarkers.cpp \
    aruco/cameraparameters.cpp \
    aruco/boarddetector.cpp \
    aruco/board.cpp \
    aruco/arucofidmarkers.cpp \
    aruco/ar_omp.cpp

HEADERS  += \
    widget.h \
    aruco/subpixelcorner.h \
    aruco/markerdetector.h \
    aruco/marker.h \
    aruco/highlyreliablemarkers.h \
    aruco/exports.h \
    aruco/cameraparameters.h \
    aruco/boarddetector.h \
    aruco/board.h \
    aruco/arucofidmarkers.h \
    aruco/aruco.h \
    aruco/ar_omp.h

RESOURCES += \
    textures.qrc \
    shaders.qrc

DISTFILES += \
    cube.png \
    fshader.fsh \
    vshader.vsh
