#ifndef MOUSEHANDLER_H
#define MOUSEHANDLER_H

#include <QMouseEvent>
#include <QWidget>
#include "renderable/renderables.h"
#include <enums.h>

class MainView;

class MouseHandler
{
public:
    MouseHandler(Renderables *_rndrbles, MainView *_widget);
    int selectedPt = -1;
    int selectedFace = -1;
    int selectedEdge = -1;
    int selectedGrad = -1;
    int height, width;

    Renderables *rndrbles;

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    short int findClosestPoint(float x, float y);
    short int findClosestEdge(float x, float y);
    short int findClosestFace(float x, float y);
    short int findClosestGrad(float x, float y);

    QVector<QVector2D> *grad = nullptr; // For setting gradients of refined levels
    Vertex *refVert = nullptr;

    MainView *mainview;
    SELECTED_POINT_TYPE type;
    SELECT_TYPE selectType = FACES;

    bool isStarted = false;
};

#endif // MOUSEHANDLER_H
