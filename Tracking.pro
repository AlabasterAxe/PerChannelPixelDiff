#-------------------------------------------------
#
# Project created by QtCreator 2012-11-18T18:09:19
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Tracking
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

win32{
LIBS += -LC:\\opencv\\build\\x86\\vc10\\lib
Release: LIBS += -lopencv_core242 -lopencv_highgui242 -lopencv_imgproc242
Debug: LIBS += -lopencv_core242d -lopencv_highgui242d -lopencv_imgproc242d
INCLUDEPATH += C:\\opencv\\build\\include
INCLUDEPATH += C:\\opencv\\build\\include\\opencv
#the CRT_SECURE_NO_WARNINGS is our way to avoid a warning about fopen being unsafe
#in the OpenCV headers. We tried _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES, but
#it does not cover fopen. We don't own OpenCV so we can't change their headers...
DEFINES += NOMS_WINDOWS=1 _CRT_SECURE_NO_WARNINGS=1
}
