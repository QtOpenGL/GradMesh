#include "mousehandler.h"
#include "renderable/renderables.h"
#include <mainview.h>
#include <QColor>
#include <QColorDialog>
#include "subdiv/subdiv.h"
#include "subdiv/meshtools.h"
#include "utilities.h"
#include <memory>

MouseHandler::MouseHandler(Renderables *_rndrbles, MainView *_mainview)
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


    switch (event->buttons()) {
    case Qt::LeftButton:
        selectedPt   = selectType == POINTS ? findClosestPoint(xScene, yScene) : -1;
        selectedEdge = selectType == EDGES  ? findClosestEdge(xScene, yScene)  : -1;
        selectedFace = selectType == FACES  ? findClosestFace(xScene, yScene)  : -1;
        selectedGrad = selectType == GRADS  ? findClosestGrad(xScene, yScene)  : -1;
        break;
    case Qt::RightButton:
        selectedPt = findClosestPoint(xScene, yScene);
        QColor c = QColorDialog::getColor(Qt::white, static_cast<QWidget*>(mainview), "Choose colour");
        if (c.isValid()){
            if (mainview->ref_level == 0){
                rndrbles->controlMesh->mesh->Vertices[selectedPt].colour = QVector3D(c.redF(), c.greenF(), c.blueF());
                rndrbles->controlMesh->fillColours();

                rndrbles->updateEm();
            }
            else {
                QVector<QVector2D> *ptr = new QVector<QVector2D>;
                Vertex *currentVertex = &rndrbles->meshVector[mainview->ref_level]->mesh->Vertices[selectedPt];
                HalfEdge *currentEdge = currentVertex->out;
                for (int k = 0; k < currentVertex->val; ++k){
                    if (not currentEdge->twin->polygon)
                        break;
                    currentEdge = currentEdge->prev->twin;
                }
                for (int i = 0; i < currentVertex->val; ++i){
                    ptr->append(currentEdge->target->coords - currentVertex->coords);
                    currentEdge = currentEdge->prev->twin;
                }
                Renderables::controlVec ctrVec = Renderables::controlVec(selectedPt, QVector3D(c.redF(), c.greenF(), c.blueF()), 1.0, ptr);
                (*mainview->rndrbles->controlVectors)[mainview->ref_level - 1].append(ctrVec);
                rndrbles->updateEm();
            }
      }
      break;
    }
}

void MouseHandler::mouseMoveEvent(QMouseEvent *event) {
    if (type == NONE || not isStarted)
        return;

    float xRatio, yRatio, xScene, yScene;

    xRatio = (float)event->x() / width;
    yRatio = (float)event->y() / height;

    xScene = (1-xRatio)*-1 + xRatio*1;
    yScene = yRatio*-1 + (1-yRatio)*1;

    QVector2D mousePt = QVector2D(xScene, yScene);
    switch (selectType){
    case POINTS:
        if (mainview->ref_level == 0){
            rndrbles->controlMesh->mesh->Vertices[selectedPt].coords = mousePt;
            rndrbles->controlMesh->fillCoords();
            rndrbles->skeletonMesh->fillCoords();
        }
        break;
    case GRADS:
        if (selectedGrad == -1)
            return;

        if (mainview->ref_level == 0){
            HalfEdge *currentEdge = &(rndrbles)->controlMesh->mesh->HalfEdges[selectedGrad];
            currentEdge->colGrad = mousePt - refVert->coords;
            rndrbles->skeletonMesh->fillCoords();
        }
        else
            (*grad)[selectedGrad] = mousePt - refVert->coords;

        break;
    }
    rndrbles->updateEm();
}

short int MouseHandler::findClosestPoint(float _x, float _y) {

    unsigned int ptIndex = -1;
    float currentDist, minDist = 1;

    if (mainview->ref_level == 0){
        for (Vertex &vtx : rndrbles->controlMesh->mesh->Vertices) {
            currentDist = pow((vtx.coords[0] - _x),2) + pow(( vtx.coords[1] - _y),2);
            if (currentDist < minDist) {
                minDist = currentDist;
                ptIndex = vtx.index;
            }
        }
    }
    else {
        QVector<unsigned int> *indices = mainview->rndrbles->ptIndices[mainview->ref_level - 1];
        Mesh *mesh = mainview->rndrbles->meshVector[mainview->ref_level]->mesh;
        QVector2D point = QVector2D(_x,_y);
        for (unsigned int k : *indices) {
            if (k == mainview->rndrbles->controlMesh->maxInt)
                continue;
            currentDist = (mesh->Vertices.at(k).coords - point).lengthSquared();
            if (currentDist < minDist) {
                minDist = currentDist;
                ptIndex = k;
            }
        }
    }
    return ptIndex;
}


short int MouseHandler::findClosestGrad(float _x, float _y) {
    int ptIndex = -1;
    float currentDist, minDist = 1;
    if (mainview->ref_level == 0){
        for (HalfEdge &edge : rndrbles->controlMesh->mesh->HalfEdges){
            QVector2D pt = edge.prev->target->coords + edge.colGrad;
            currentDist = pow((pt[0] - _x),2) + pow(( pt[1] - _y),2);
            if (currentDist < minDist) {
                minDist = currentDist;
                ptIndex = edge.index;
                refVert = edge.twin->target;
            }
        }
    }
    else {
        grad = nullptr;
        unsigned int ref_level = mainview->ref_level;
        QVector<Vertex> *vertices = &rndrbles->meshVector[ref_level]->mesh->Vertices;
        QVector2D vertexPoint;
        for (Renderables::controlVec const &ctr : rndrbles->controlVectors->at(mainview->ref_level - 1)){
            vertexPoint = vertices->at(std::get<0>(ctr)).coords;
            short int index = 0;
            for (QVector2D &vec : *std::get<3>(ctr)){

                QVector2D pt = vertexPoint + vec;
                currentDist = pow((pt[0] - _x),2) + pow(( pt[1] - _y),2);
                if (currentDist < minDist) {
                    minDist = currentDist;
                    ptIndex = index;
                    grad = std::get<3>(ctr);
                    refVert = &((*vertices)[std::get<0>(ctr)]);
                }
                ++index;
            }
        }
        qDebug() << ptIndex;
    }
    return ptIndex;
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
    for (int i = 0; i < rndrbles->skeletonMesh->mesh->Faces.size(); ++i){
        currentFace = &rndrbles->skeletonMesh->mesh->Faces[i];
        dist = (centroid(currentFace) - point).lengthSquared();
        if (dist < minDist){
            minDist = dist;
            minIndex = i;
        }
    }

    if (minIndex != -1)
        buildFaceRenderable(minIndex, rndrbles);

    return minIndex;
}

short int MouseHandler::findClosestEdge(float _x, float _y) {
    QVector2D point = QVector2D(_x, _y);
    float minDist = 1000;
    float dist;
    short int minIndex = -1;
    HalfEdge* currentEdge;

    for (unsigned int i = 0; i < (unsigned int)rndrbles->skeletonMesh->mesh->HalfEdges.size(); ++i){
        currentEdge = &rndrbles->skeletonMesh->mesh->HalfEdges[i];
        dist = distToLine(point, currentEdge->start(), currentEdge->end());
        if (dist < minDist){
            minDist = dist;
            minIndex = i;
        }
    }
    if (minIndex != -1)
        buildEdgeRenderable(minIndex, rndrbles);

    return minIndex;
}
