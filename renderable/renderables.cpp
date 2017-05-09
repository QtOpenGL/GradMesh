#include "renderables.h"
#include "../subdiv/subdiv.h"
#include "../subdiv/meshtools.h"
#include "mainview.h"
#include <assert.h>

Renderables::Renderables()
    :
      controlMesh(new MeshRenderable),
      colourSurface(new MeshRenderable),
      skeletonMesh(new ControlRenderable),
      edgeRenderable(new Renderable),
      faceRenderable(new Renderable),
      renderableList(new QVector<Renderable *>)
{
    renderableList->clear();
    renderableList->squeeze();

    renderableList->append(static_cast<Renderable *>(controlMesh));
    renderableList->append(static_cast<Renderable *>(colourSurface));
    renderableList->append(static_cast<Renderable *>(skeletonMesh));
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

void Renderables::updateEm(){
    skeletonMesh->fillCoords();

    size_t old_size = meshVector.size();
    for (size_t i = 0; i < old_size; ++i){
        delete meshVector[i]->mesh;
        meshVector[i]->mesh = new Mesh;
    }

    for (size_t i = old_size; i < ccSteps + 1; ++i)
        meshVector.append(new MeshRenderable);

    ternaryStep(skeletonMesh->mesh, meshVector[0]->mesh);

    for (size_t refIndex = 0; refIndex < ccSteps; ++refIndex)
        subdivideCatmullClark(meshVector[refIndex]->mesh, meshVector[refIndex + 1]->mesh);

    colourSurface->mesh = meshVector[ccSteps]->mesh;

    colourSurface->fillCoords();
    colourSurface->fillColours();

    ptIndices.clear();
    ptIndices.squeeze();
    ptIndices.append(new QVector<unsigned int>);

    QVector<Face> const *cFaces = &controlMesh->mesh->Faces;
    int counter = -1;
    for (size_t i = 0; i < static_cast<size_t>(cFaces->size()); ++i){
        counter += 2 * controlMesh->mesh->Faces[i].val + 1;
        ptIndices[0]->append(counter);
    }

}




























