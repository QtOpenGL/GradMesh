#include "mousehandler.h"
#include <mainview.h>
#include <QColor>
#include <QColorDialog>
#include "subdiv/subdiv.h"
#include "subdiv/meshtools.h"
#include "utilities.h"


MouseHandler::MouseHandler(QVector<Renderables *> *_rndrbles, MainView *_mainview)
  : rndrbles(_rndrbles),
    mainview(_mainview)
{
}

void MouseHandler::mousePressEvent(QMouseEvent *event) {
    if (not isStarted){
        qDebug() << "No mesh";
        return;
    }
    float xRatio, yRatio, xScene, yScene;

    xRatio = (float)event->x() / width;
    yRatio = (float)event->y() / height;

    xScene = 2 * xRatio - 1;
    yScene = 1 - 2 * yRatio;

//    findQuadrant(&xScene, &yScene);

    rndrblesIndex = static_cast<short int>(mainview->disp);

    switch (event->buttons()) {
    case Qt::LeftButton:
        selectedPt   = selectType == POINTS ? findClosestPoint(xScene, yScene) : -1;
        selectedEdge = selectType == EDGES  ? findClosestEdge(xScene, yScene)  : -1;
        selectedFace = selectType == FACES  ? findClosestFace(xScene, yScene)  : -1;
        break;
    case Qt::RightButton:
        selectedPt = findClosestPoint(xScene, yScene);
        if (type != CONTROL)
            break;
        QColor c = QColorDialog::getColor(Qt::white, static_cast<QWidget*>(mainview));
        if (c.isValid()){
            (*rndrbles)[rndrblesIndex]->controlMesh->mesh->Vertices[selectedPt].colour = QVector3D(c.redF(), c.greenF(), c.blueF());
            (*rndrbles)[rndrblesIndex]->controlMesh->fillColours();

            (*rndrbles)[rndrblesIndex]->updateEm();
      }
      break;
    }
}

void MouseHandler::findQuadrant(float *_x, float *_y) {

    if ((*rndrbles)[0]){
        rndrblesIndex = 0;
        return;
    }
    size_t qdrnt;
    if (*_x > 0.0){
        if (*_y < 0.0)
            qdrnt = 4;
        else
            qdrnt = 2;
    }
    else {
        if (*_y < 0.0)
            qdrnt = 3;
        else
            qdrnt = 1;
    }

    if (not (*rndrbles)[qdrnt]){
        rndrblesIndex = -1;
        return;
    }
    rndrblesIndex = qdrnt;

    *_x += mainview->quadrantEntries[qdrnt][0];
    *_y += mainview->quadrantEntries[qdrnt][1];
    *_x /= mainview->quadrantEntries[qdrnt][2];
    *_y /= mainview->quadrantEntries[qdrnt][2];
}

void MouseHandler::mouseMoveEvent(QMouseEvent *event) {
    if (type == NONE || rndrblesIndex == -1)
        return;

    float xRatio, yRatio, xScene, yScene;

    xRatio = (float)event->x() / width;
    yRatio = (float)event->y() / height;

    xScene = (1-xRatio)*-1 + xRatio*1;
    yScene = yRatio*-1 + (1-yRatio)*1;

//    xScene += mainview->quadrantEntries[rndrblesIndex][0];
//    yScene += mainview->quadrantEntries[rndrblesIndex][1];
//    xScene /= mainview->quadrantEntries[rndrblesIndex][2];
//    yScene /= mainview->quadrantEntries[rndrblesIndex][2];

    // Maybe here some code that prevents user from dragging point outside of the quadrant

    if (selectType == POINTS){
        if (type == CONTROL){
            // Update position of the control point
            (*rndrbles)[rndrblesIndex]->controlMesh->mesh->Vertices[selectedPt].coords = QVector2D(xScene, yScene);
            (*rndrbles)[rndrblesIndex]->controlMesh->fillCoords();
            (*rndrbles)[rndrblesIndex]->skeletonMesh->fillCoords();
        } else if (type == GRADIENT){
            HalfEdge *currentEdge = &((*rndrbles)[rndrblesIndex])->controlMesh->mesh->HalfEdges[selectedPt];
            currentEdge->colGrad = QVector2D(xScene, yScene) - (*rndrbles)[rndrblesIndex]->controlMesh->mesh->Vertices[currentEdge->twin->target->index].coords;
            (*rndrbles)[rndrblesIndex]->skeletonMesh->fillCoords();
        }

        (*rndrbles)[rndrblesIndex]->updateEm();
    }
}

short int MouseHandler::findClosestPoint(float _x, float _y) {

    short int ptIndex = -1;
    float currentDist, minDist = 1;

    type = NONE;
    for (size_t k = 0; k < (size_t)(*rndrbles)[rndrblesIndex]->skeletonMesh->coords->size(); k++) {
        currentDist = pow(((*rndrbles)[rndrblesIndex]->skeletonMesh->coords->at(k)[0] - _x),2) + pow(( (*rndrbles)[rndrblesIndex]->skeletonMesh->coords->at(k)[1] - _y),2);
        if (currentDist < minDist) {
            minDist = currentDist;
            ptIndex = k;
        }
    }

    if (ptIndex == -1){
        type = NONE;
        return -1;
    }

    size_t vertIndex = 0;
    while (ptIndex >= (*rndrbles)[rndrblesIndex]->skeletonMesh->mesh->Vertices[vertIndex].val + 1){
        ptIndex -= 1 + (*rndrbles)[rndrblesIndex]->skeletonMesh->mesh->Vertices[vertIndex].val;
        vertIndex++;
    }
    type = ptIndex == 0 ? CONTROL : GRADIENT;

    if (ptIndex == 0)
        return (*rndrbles)[rndrblesIndex]->skeletonMesh->mesh->Vertices[vertIndex].index;

    HalfEdge *currentEdge = (*rndrbles)[rndrblesIndex]->skeletonMesh->mesh->Vertices[vertIndex].out;
    while (ptIndex != 1){
        currentEdge = currentEdge->prev->twin;
        --ptIndex;
    }
    return currentEdge->index;
}

void buildFaceRenderable(short int faceIndex, Renderables *renderables){
    Face *currentFace = &renderables->skeletonMesh->mesh->Faces[faceIndex];

    renderables->faceRenderable->coords->clear();
    renderables->faceRenderable->coords->squeeze();
    renderables->faceRenderable->colours->clear();
    renderables->faceRenderable->colours->squeeze();
    renderables->faceRenderable->indices->clear();
    renderables->faceRenderable->indices->squeeze();

    HalfEdge *currentEdge = currentFace->side;
    for (size_t i = 0; i < currentFace->val; ++i){
        renderables->faceRenderable->coords->append(currentEdge->start());
        renderables->faceRenderable->colours->append(QVector3D(1.0, 1.0, 1.0));
        renderables->faceRenderable->indices->append(i);
        currentEdge = currentEdge->next;
    }
}

void buildEdgeRenderable(short int edgeIndex, Renderables *renderables){
    HalfEdge *currentEdge = &renderables->skeletonMesh->mesh->HalfEdges[edgeIndex];

    renderables->edgeRenderable->coords->clear();
    renderables->edgeRenderable->coords->squeeze();
    renderables->edgeRenderable->coords->push_back(currentEdge->start());
    renderables->edgeRenderable->coords->push_back(currentEdge->end());
}

short int MouseHandler::findClosestFace(float _x, float _y) {
    QVector2D point = QVector2D(_x, _y);
    float minDist = 1000;
    float dist;
    int minIndex = -1;
    Face* currentFace;
    for (int i = 0; i < (*rndrbles)[rndrblesIndex]->skeletonMesh->mesh->Faces.size(); ++i){
        currentFace = &(*rndrbles)[rndrblesIndex]->skeletonMesh->mesh->Faces[i];
        dist = (centroid(currentFace) - point).lengthSquared();
        if (dist < minDist){
            minDist = dist;
            minIndex = i;
        }
    }

    if (minIndex != -1)
        buildFaceRenderable(minIndex, (*rndrbles)[rndrblesIndex]);

    return minIndex;
}

short int MouseHandler::findClosestEdge(float _x, float _y) {
    QVector2D point = QVector2D(_x, _y);
    float minDist = 1000;
    float dist;
    short int minIndex = -1;
    HalfEdge* currentEdge;

    for (unsigned int i = 0; i < (unsigned int)(*rndrbles)[rndrblesIndex]->skeletonMesh->mesh->HalfEdges.size(); ++i){
        currentEdge = &(*rndrbles)[rndrblesIndex]->skeletonMesh->mesh->HalfEdges[i];
        dist = distToLine(point, currentEdge->start(), currentEdge->end());
        if (dist < minDist){
            minDist = dist;
            minIndex = i;
        }
    }
    if (minIndex != -1)
        buildEdgeRenderable(minIndex, (*rndrbles)[rndrblesIndex]);

    return minIndex;
}
