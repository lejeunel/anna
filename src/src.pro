#-------------------------------------------------
#
# Project created by QtCreator 2017-02-14T14:49:16
#
#-------------------------------------------------

CONFIG += c++14

DESTDIR = ../bin
MOC_DIR = ../build/moc
RCC_DIR = ../build/rcc
UI_DIR = ../build/ui
unix:OBJECTS_DIR = ../build/o/unix
win32:OBJECTS_DIR = ../build/o/win32
macx:OBJECTS_DIR = ../build/o/mac
QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = anna
TEMPLATE = app

unix:!mac {     # linux

    QMAKE_RPATHDIR += /usr/lib
    QMAKE_CXXFLAGS= -fpermissive
    # Boost:
    #INCLUDEPATH += /usr/include
    LIBS += -L/usr/lib \
        -lboost_system \
        -lboost_thread \
        -lboost_chrono

    # GL:
    LIBS += -lGL

    # OpenCV:
    INCLUDEPATH += /usr/include/opencv4
    LIBS += -L/usr/lib/x86_64-linux-gnu \
        -lopencv_core \
        #-lopencv_imgcodecs \
        -lopencv_highgui \
        -lopencv_videoio \
        -lopencv_imgproc

    # ITK:
    ITK_VERSION_LINUX = 4.13
    INCLUDEPATH += /usr/include/ITK-$$ITK_VERSION_LINUX
    LIBS += -L/usr/lib \
        -lITKCommon-$$ITK_VERSION_LINUX \
        -lITKDICOMParser-$$ITK_VERSION_LINUX \
        #-litkdouble-conversion-$$ITK_VERSION_LINUX \
        #-lITKEXPAT-$$ITK_VERSION_LINUX \
        #-litkgdcmcharls-$$ITK_VERSION_LINUX \
        #-litkgdcmCommon-$$ITK_VERSION_LINUX \
        #-litkgdcmDICT-$$ITK_VERSION_LINUX \
        #-litkgdcmDSED-$$ITK_VERSION_LINUX \
        #-litkgdcmIOD-$$ITK_VERSION_LINUX \
        #-litkgdcmjpeg8-$$ITK_VERSION_LINUX \
        #-litkgdcmjpeg12-$$ITK_VERSION_LINUX \
        #-litkgdcmjpeg16-$$ITK_VERSION_LINUX \
        #-litkgdcmMEXD-$$ITK_VERSION_LINUX \
        #-litkgdcmMSFF-$$ITK_VERSION_LINUX \
        #-litkgdcmopenjpeg-$$ITK_VERSION_LINUX \
        #-litkgdcmsocketxx-$$ITK_VERSION_LINUX \
        #-litkgdcmuuid-$$ITK_VERSION_LINUX \
        -lITKIOGDCM-$$ITK_VERSION_LINUX \
        -lITKIOImageBase-$$ITK_VERSION_LINUX \
        -lITKIOJPEG-$$ITK_VERSION_LINUX \
        -lITKIOPNG-$$ITK_VERSION_LINUX \
        -lITKIOTIFF-$$ITK_VERSION_LINUX \
        #-litkjpeg-$$ITK_VERSION_LINUX \
        #-litkpng-$$ITK_VERSION_LINUX \
        -litksys-$$ITK_VERSION_LINUX \
        #-litktiff-$$ITK_VERSION_LINUX \
        #-litkv3p_lsqr-$$ITK_VERSION_LINUX \
        #-litkv3p_netlib-$$ITK_VERSION_LINUX \
        -litkvcl-$$ITK_VERSION_LINUX \
        -litkvnl_algo-$$ITK_VERSION_LINUX \
        -litkvnl-$$ITK_VERSION_LINUX \
        -lITKVNLInstantiation-$$ITK_VERSION_LINUX \
        #-litkzlib-$$ITK_VERSION_LINUX

    # libdl
    LIBS += -ldl
}

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS


SOURCES += main.cpp\
    mainwindow.cpp \
    videoreader.cpp \
    eyetribe/gazeapi_parser.cpp \
    eyetribe/gazeapi_socket.cpp \
    eyetribe/gazeapi.cpp \
    framelabel.cpp \
    eyetribetracker.cpp \
    touchbutton.cpp \
    intuitiveslider.cpp \
    speedslider.cpp \
    volumereader.cpp \
    colorfulstatusbar.cpp \
    annotationdatamanager.cpp \
    annotationlabel.cpp

HEADERS  += mainwindow.h \
    eyetribe/gazeapi_observable.hpp \
    eyetribe/gazeapi_parser.hpp \
    eyetribe/gazeapi_socket.hpp \
    eyetribe/gazeapi.hpp \
    eyetribe/gazeapi_interfaces.hpp \
    eyetribe/gazeapi_types.hpp \
    framelabel.h \
    eyetrackerinterface.h \
    eyetribetracker.h \
    framereaderinterface.h \
    videoreader.h \
    touchbutton.h \
    intuitiveslider.h \
    speedslider.h \
    volumereader.h \
    colorfulstatusbar.h \
    annotationdatamanager.h \
    annotationlabel.h

RESOURCES += \
    anna.qrc \
    qdarkstyle/style.qrc
