# Created by and for Qt Creator This file was created for editing the project sources only.
# You may attempt to use it for building too, by modifying this file here.

TARGET = saclient

QT       += core gui
QT       += network
QT       += widgets

CONFIG   += c++11

HEADERS = \
   $$PWD/client.h \
   $$PWD/mainwindow.h \
   $$PWD/ui_mainwindow.h

SOURCES = \
   $$PWD/client.cpp \
   $$PWD/main.cpp \
   $$PWD/mainwindow.cpp

INCLUDEPATH = \
    $$PWD/.

FORMS = mainwindow.ui
RESOURCES = assets.qrc

#DEFINES = 

