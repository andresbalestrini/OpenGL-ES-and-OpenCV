#---------------------------------
#
# Proyecto de Realidad Aumentada con OpenGL-ES
#
#---------------------------------

QT += core gui network multimedia widgets opengl

TEMPLATE = app

DEFINES += NO_DEBUG_ARUCO

! contains( ANDROID_TARGET_ARCH, armeabi-v7a )  {  # Si no es para Android entonces para Desktop

#    message( Compilacion para Desktop )

###############################################################################################
    unix:DIR_OPENCV_LIBS = /usr/local/lib  ####################################################
###############################################################################################

    unix:INCLUDEPATH += "/usr/include/GL/"                             # OpenGL
    unix:LIBS += "/usr/lib/x86_64-linux-gnu/libglut.so"                # OpenGL

    unix:LIBS += $$DIR_OPENCV_LIBS/libopencv_core.so         # OpenCV
    unix:LIBS += $$DIR_OPENCV_LIBS/libopencv_highgui.so      # OpenCV
    unix:LIBS += $$DIR_OPENCV_LIBS/libopencv_imgproc.so      # OpenCV
    unix:LIBS += $$DIR_OPENCV_LIBS/libopencv_objdetect.so    # OpenCV
    unix:LIBS += $$DIR_OPENCV_LIBS/libopencv_calib3d.so      # OpenCV
    unix:LIBS += $$DIR_OPENCV_LIBS/libopencv_ml.so           # OpenCV
    unix:LIBS += $$DIR_OPENCV_LIBS/libopencv_features2d.so
    unix:LIBS += $$DIR_OPENCV_LIBS/libopencv_video.so

    unix:LIBS += $$DIR_OPENCV_LIBS/libopencv_flann.so
    unix:LIBS += $$DIR_OPENCV_LIBS/libopencv_photo.so
    unix:LIBS += $$DIR_OPENCV_LIBS/libopencv_stitching.so
    unix:LIBS += $$DIR_OPENCV_LIBS/libopencv_superres.so
    unix:LIBS += $$DIR_OPENCV_LIBS/libopencv_video.so
    unix:LIBS += $$DIR_OPENCV_LIBS/libopencv_videostab.so
    unix:LIBS += $$DIR_OPENCV_LIBS/libopencv_imgcodecs.so
    unix:LIBS += $$DIR_OPENCV_LIBS/libopencv_videoio.so

    unix:LIBS += "/usr/lib/x86_64-linux-gnu/lib3ds.so"                 # Modelos 3D

    DEFINES += DESKTOP
}

SOURCES += main.cpp\
           aruco/ar_omp.cpp \
           aruco/arucofidmarkers.cpp \
           aruco/board.cpp \
           aruco/boarddetector.cpp \
           aruco/cameraparameters.cpp \
           aruco/highlyreliablemarkers.cpp \
           aruco/marker.cpp \
           aruco/markerdetector.cpp \
           aruco/subpixelcorner.cpp \
    principal.cpp \
    escena.cpp \
    geometryengine.cpp

HEADERS += \
           aruco/ar_omp.h \
           aruco/aruco.h \
           aruco/arucofidmarkers.h \
           aruco/board.h \
           aruco/boarddetector.h \
           aruco/cameraparameters.h \
           aruco/exports.h \
           aruco/highlyreliablemarkers.h \
           aruco/marker.h \
           aruco/markerdetector.h \
           aruco/subpixelcorner.h \
    principal.h \
    escena.h \
    geometryengine.h

FORMS += \
    principal.ui

contains( ANDROID_TARGET_ARCH, armeabi-v7a )  {  # Para Android

    DIR_ANDROID_OPENCV = /home/cosimani/Proyecto/UBP/PGE2016/OpenCV-android-sdk/sdk/native

    INCLUDEPATH += $$DIR_ANDROID_OPENCV/jni/include

    LIBS += $$DIR_ANDROID_OPENCV/libs/armeabi-v7a/libopencv_java.so
    LIBS += $$DIR_ANDROID_OPENCV/libs/armeabi-v7a/libnative_camera_r4.2.0.so
    LIBS += $$DIR_ANDROID_OPENCV/libs/armeabi-v7a/libopencv_info.so

    LIBS += -L"$$DIR_ANDROID_OPENCV/libs/armeabi-v7a" \
            -L"$$DIR_ANDROID_OPENCV/3rdparty/libs/armeabi-v7a"
    LIBS += \
        -llibtiff\
        -llibjpeg\
        -llibjasper\
        -llibpng\
        -lIlmImf\
        -ltbb\
        -lopencv_core\
        -lopencv_imgproc\
        -lopencv_highgui\
        -lopencv_androidcamera

    contains( ANDROID_TARGET_ARCH,armeabi-v7a )  {
        ANDROID_EXTRA_LIBS = /home/cosimani/Proyecto/2016/pruebaOsiAndroid/menuopencv-master2/RA-Menu-Sin-2/../../../../UBP/PGE2016/OpenCV-android-sdk/sdk/native/libs/armeabi-v7a/libopencv_info.so /home/cosimani/Proyecto/2016/pruebaOsiAndroid/menuopencv-master2/RA-Menu-Sin-2/../../../../UBP/PGE2016/OpenCV-android-sdk/sdk/native/libs/armeabi-v7a/libopencv_java.so /home/cosimani/Proyecto/2016/pruebaOsiAndroid/menuopencv-master2/RA-Menu-Sin-2/../../../../UBP/PGE2016/OpenCV-android-sdk/sdk/native/libs/armeabi-v7a/libnative_camera_r4.2.0.so
    }

}

RESOURCES += \
    textures.qrc \
    shaders.qrc

