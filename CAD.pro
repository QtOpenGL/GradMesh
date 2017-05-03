#-------------------------------------------------
#
# Project created by QtCreator 2016-11-22T14:42:43
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SubdivCurves
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    mainview.cpp \
    mousehandler.cpp \
    mesh/mesh.cpp \
    objfile.cpp \
    subdiv/ternary.cpp \
    subdiv/meshtools.cpp \
    renderable/renderable.cpp \
    renderable/meshrenderable.cpp \
    renderable/renderables.cpp \
    renderable/controlrenderable.cpp \
    utilities.cpp \
    subdiv/catmullclark.cpp \
    subdiv/ccsingleface.cpp \
    subdiv/makengon.cpp

HEADERS  += mainwindow.h \
    mainview.h \
    mousehandler.h \
    mesh/face.h \
    mesh/vertex.h \
    mesh/halfedge.h \
    mesh/mesh.h \
    objfile.h \
    enums.h \
    subdiv/meshtools.h \
    mesh/vertinfo.h \
    subdiv/subdiv.h \
    renderable/renderable.h \
    renderable/meshrenderable.h \
    renderable/renderables.h \
    renderable/controlrenderable.h \
    utilities.h

FORMS    += mainwindow.ui

RESOURCES += \
    resources.qrc

CONFIG += c++11

QMAKE_CXXFLAGS += -Wall -pedantic -Werror


