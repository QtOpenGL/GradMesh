#include "renderables.h"
#include "../subdiv/subdiv.h"
#include "../subdiv/meshtools.h"
#include "mainview.h"
#include <assert.h>
#include <tuple>

Renderables::Renderables()
    :
      controlMesh(new MeshRenderable),
      colourSurface(new MeshRenderable),
      skeletonMesh(new ControlRenderable),
      gradRenderable(new Renderable),
      edgeRenderable(new Renderable),
      faceRenderable(new Renderable),
      renderableList(new QVector<Renderable *>),
      controlVectors(new QVector<QVector <controlVec> >)
{
    renderableList->clear();
    renderableList->squeeze();

    renderableList->append(static_cast<Renderable *>(controlMesh));
    renderableList->append(static_cast<Renderable *>(colourSurface));
    renderableList->append(static_cast<Renderable *>(skeletonMesh));
    renderableList->append(gradRenderable);
    renderableList->append(edgeRenderable);
    renderableList->append(faceRenderable);
}

Renderables::~Renderables(){
    delete controlMesh;
    delete colourSurface;
    delete skeletonMesh;
    delete edgeRenderable;
    delete faceRenderable;
}

void Renderables::init(OBJFile* loadedOBJFile){
    controlMesh->mesh = new Mesh(loadedOBJFile);
    init();
}

void Renderables::init(Mesh *mesh){
    controlMesh->mesh = new Mesh;
    controlMesh->mesh->copy(mesh);
    init();
}

void Renderables::init(){    
    controlMesh->fillCoords();
    controlMesh->fillColours();

    controlVectors->reserve(ccSteps);

    skeletonMesh->mesh = &(*controlMesh->mesh);
    updateEm();

    *edgeRenderable->colours = QVector<QVector3D>{ {1.0,1.0,1.0}, {1.0,1.0,1.0} };
    *edgeRenderable->indices = QVector<unsigned int> {0, 1};
}

QVector3D limitPoint(Vertex *vtx){
    QVector3D result = QVector3D(0.0, 0.0, 0.0);
    HalfEdge *currentEdge = vtx->out;
    size_t n = vtx->val;
    for (size_t i = 0; i < n; ++i){
        result += 4 * currentEdge->target->colour;
        result += currentEdge->next->target->colour;
        currentEdge = currentEdge->prev->twin;
    }
    // 4 * edges + faces
    result += n * n * vtx->colour;
    return (result / (n * (n + 5)));
}

bool isEdgeVertex(Vertex *vtx){
    size_t n = vtx->val;
    HalfEdge *currentEdge = vtx->out;
    for (size_t i = 0; i < n; ++i){
        if (not currentEdge->polygon)
            return true;
        currentEdge = currentEdge->twin->next;
    }
    return false;
}

HalfEdge *getBoundaryOut(Vertex *vtx){
    HalfEdge *currentEdge = vtx->out;
    while (currentEdge->twin->polygon)
        currentEdge = currentEdge->twin->next;
    return currentEdge;
}

void Renderables::threeRing(Vertex *vertex, QVector<unsigned int> *pts, int counter){
    int n = vertex->val;
    HalfEdge *currentEdge;

    bool isEdgeVtx = isEdgeVertex(vertex);
    if (isEdgeVtx){
        currentEdge = getBoundaryOut(vertex);
        --n;
    }
    else
        currentEdge = vertex->out;

    for (int j = 0; j < 2; ++j)
        currentEdge = currentEdge->next->twin->next;
    currentEdge = currentEdge->next;


    for (int k = 0; k < n; ++k){
        pts->append(counter);

        pts->append(currentEdge->twin->target->index);

        for (int dummy = 0; dummy < 2; ++dummy)
            currentEdge = currentEdge->next->twin->next;
        currentEdge = currentEdge->next;

        pts->append(currentEdge->twin->target->index);

        for (int dummy = 0; dummy < 2; ++dummy)
            currentEdge = currentEdge->next->twin->next;
        currentEdge = currentEdge->next;

        currentEdge = currentEdge->twin->next;


        pts->append(currentEdge->twin->target->index);
        pts->append(counter);

        pts->append(controlMesh->maxInt);
    }
}

QVector2D getFacePoint(HalfEdge *currentEdge){
    float d1, d2, N1, N2, V01, V02;
    QVector2D V0, V1, V2, D1, D2, C;      // V1 ---- V0 ---- V2

    HalfEdge *midEdge = currentEdge->next->twin->prev->twin;
    for (int i = 0; i < midEdge->polygon->val; ++i){
        C += midEdge->target->coords;
        midEdge = midEdge->next;
    }
    C /= midEdge->polygon->val;

    V1 = currentEdge->prev->twin->next->twin->next->next->twin->next->target->coords;
    V0 = currentEdge->twin->target->coords;
    V2 = currentEdge->next->twin->next->next->twin->next->target->coords;
    D1 = currentEdge->prev->twin->target->coords;
    D2 = currentEdge->target->coords;
    N1 = D1.length();
    N2 = D2.length();
    C = V1 + V2;
    C /= 2;
    if (N1 == 0.0 || N2 == 0.0)
        qDebug() << "Zero length colour gradient";

    V01 = QVector2D(V0 - V1).length();
    V02 = QVector2D(V0 - V2).length();

    d1 = 2 * N1 / V01;
    d2 = 2 * N2 / V02;

    QVector2D newPosition =
        (1 - d1) * (1-d2) * V0 +
        d1 * d2 * C +
        d1 * (1 - d2) * (V0 + V01 * D1 / (2 * N1)) +
        d2 * (1 - d1) * (V0 + V02 * D2 / (2 * N2));
    return newPosition;
}

void Renderables::setRing(Vertex *vtx, controlVec &ctr){
    int index;
    QVector3D colour;
    float alpha;
    QVector<QVector2D> *grads;
    std::tie(index, colour, alpha, grads) = ctr;
    size_t n = vtx->val;
    float beta = (n * (n + 5.0f - n * alpha)) / (5.0f * n);

    if (beta < 0){
        beta = 0;  // We set beta = 0 and alpha to alpha_max
        alpha = (n * (n + 5)) / static_cast<float>(n * n);
    }

    if (beta < 0)
        qDebug() << alpha;

    vtx->colour = alpha * colour;
    bool isEdgeVtx = isEdgeVertex(vtx);
    HalfEdge *currentEdge;
    if (isEdgeVtx){
        currentEdge = getBoundaryOut(vtx);
        --n;
    }
    else
        currentEdge = vtx->out;

    colour *= beta;
    for (size_t i = 0; i < n; ++i){
        currentEdge->target->colour = colour;
        currentEdge->next->target->colour = colour;
        currentEdge->target->coords = (*grads)[i] + vtx->coords;
        currentEdge = currentEdge->prev->twin;
    }
    if (isEdgeVtx){
        currentEdge->target->colour = colour;
        currentEdge->target->coords += (alpha - 1) * (currentEdge->target->coords - vtx->coords);
    }
    if (isEdgeVtx)
        currentEdge = getBoundaryOut(vtx);
    else
        currentEdge = vtx->out;

    for (size_t i = 0; i < n; ++i){
        currentEdge->next->target->coords = currentEdge->end()+ currentEdge->prev->twin->end() - vtx->coords;
        currentEdge = currentEdge->prev->twin;
    }
}

void Renderables::updateEm(){
    controlVectors->resize(ccSteps);
    skeletonMesh->fillCoords();

    size_t old_size = meshVector.size();
    for (size_t i = 0; i < old_size; ++i){
        delete meshVector[i]->mesh;
        meshVector[i]->mesh = new Mesh;
    }

    for (size_t i = old_size; i < ccSteps + 1; ++i)
        meshVector.append(new MeshRenderable);

    ternaryStep(skeletonMesh->mesh, meshVector[0]->mesh);

    for (size_t refIndex = 0; refIndex < ccSteps; ++refIndex){
        subdivideCatmullClark(meshVector[refIndex]->mesh, meshVector[refIndex + 1]->mesh);
        for (controlVec ctr : controlVectors->at(refIndex))
            setRing(&meshVector[refIndex + 1]->mesh->Vertices[std::get<0>(ctr)], ctr);
    }

//    for (size_t i = 0; i < ccSteps + 1; ++i){
//        meshVector[i]->fillCoords();
//        meshVector[i]->fillColours();
//    }

    colourSurface->mesh = meshVector[ccSteps]->mesh;

    colourSurface->fillCoords();
    colourSurface->fillColours();

    ptIndices.clear();
    ptIndices.squeeze();
    ptIndices.append(new QVector<unsigned int>);

    QVector<Face> const *cFaces = &controlMesh->mesh->Faces;
    int counter = -1;
//    for (size_t i = 0; i < static_cast<size_t>(cFaces->size()); ++i){
    for (auto const &face : *cFaces){
        counter += 2 * face.val + 1;
        threeRing(&meshVector[1]->mesh->Vertices[counter], ptIndices[0], counter);
    }

    for (size_t i = 1; i < ccSteps; i++){
        counter = meshVector[i]->mesh->Faces.size();
        ptIndices.append(new QVector<unsigned int>);
        for (size_t index : *ptIndices[i - 1]){
            if (index == controlMesh->maxInt)
                continue;
            threeRing(&meshVector[i + 1]->mesh->Vertices[counter + index], ptIndices[i], counter + index);
        }
    }
}
