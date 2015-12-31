#-------------------------------------------------
#
# Project created by QtCreator 2015-12-08T15:07:51
#
#-------------------------------------------------

QT       += core gui
QT += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SocketServerTest
TEMPLATE = app


SOURCES += main.cpp\
        socketserver.cpp \
    mythread.cpp \
    mymodel.cpp


HEADERS  += socketserver.h \
    mythread.h \
    mymodel.h

FORMS    += socketserver.ui
