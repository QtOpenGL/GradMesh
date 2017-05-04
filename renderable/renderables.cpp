#include "renderables.h"
#include "../subdiv/subdiv.h"
#include "../subdiv/meshtools.h"
#include "mainview.h"
#include <assert.h>

Renderables::Renderables()
    :
      controlMesh(new MeshRenderable),
      colourSurface(new MeshRenderable),
      refineMesh(new MeshRenderable),
      skeletonMesh(new ControlRenderable),
      edgeRenderable(new Renderable),
      faceRenderable(new Renderable),
      renderableList(new QVector<Renderable *>)
{
    renderableList->clear();
    renderableList->squeeze();

    renderableList->append(static_cast<Renderable *>(controlMesh));
    renderableList->append(static_cast<Renderable *>(colourSurface));
    renderableList->append(static_cast<Renderable *>(refineMesh));
    renderableList->append(static_cast<Renderable *>(skeletonMesh));
    renderableList->append(edgeRenderable);
    renderableList->append(faceRenderable);

}

Renderables::~Renderables(){
    delete controlMesh;
    delete colourSurface;
    delete refineMesh;
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

    refineMesh->mesh = new Mesh;
    ternaryStep(controlMesh->mesh, refineMesh->mesh);
    Mesh *temp = new Mesh;
    temp->copy(refineMesh->mesh);
    subdivideCatmullClark(temp, refineMesh->mesh);
    for (Vertex& vtx : refineMesh->mesh->Vertices)
        vtx.colour = QVector3D(0.0, 0.0, 0.0);


    for (Vertex& vtx : refineMesh->mesh->Vertices)
        vtx.colour = QVector3D(0.0, 0.0, 0.0);

    setRefineMeshColours();
    refineMesh->fillCoords();
    refineMesh->fillColours();

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

void Renderables::setRefineMeshColours(){
    int counter = -1;
    HalfEdge *currentEdge;
    Face *currentFace;
    Vertex *currentVertex;
    for (int i = 0; i < controlMesh->mesh->Faces.size(); ++i){
        currentFace = &controlMesh->mesh->Faces[i];
        counter += 2 * currentFace->val + 1;
        currentVertex = &refineMesh->mesh->Vertices[counter];
//        QVector3D r0 = limitPoint(&colourSurface->mesh->Vertices[counter]);
//        float r0 = colourSurface->mesh->Vertices[counter].colour[1];
//        r0 = 0.9;
//        qDebug() << r0;
        currentEdge = currentVertex->out;
        for (int k = 0; k < currentVertex->val; ++k){
            for (int j = 0; j < currentEdge->polygon->val; ++j){
                currentEdge->target->colour = QVector3D(1.0, 1.0, 1.0);
                currentEdge = currentEdge->next;
            }
            currentEdge = currentEdge->prev->twin;
        }
        currentVertex->colour = QVector3D(1.0, 1.0, 1.0);
    }
}

void Renderables::updateEm(){
    skeletonMesh->fillCoords();

    tMeshes[0] = skeletonMesh->mesh;
    tMeshes[1] = new Mesh;

    size_t tIndex = 1;

    ternaryStep(tMeshes[1 - tIndex], tMeshes[tIndex]);
    tIndex = 1 - tIndex;
    tMeshes[tIndex] = new Mesh;

    for (size_t refIndex = 0; refIndex < ccSteps; ++refIndex){
        subdivideCatmullClark(tMeshes[1 - tIndex], tMeshes[tIndex]);
        tIndex = 1 - tIndex;
        tMeshes[tIndex] = new Mesh;
    }

    colourSurface->mesh = tMeshes[1 - tIndex];

    refineMesh->mesh = new Mesh;
    ternaryStep(controlMesh->mesh, refineMesh->mesh);
    Mesh *temp = new Mesh;
    temp->copy(refineMesh->mesh);
    subdivideCatmullClark(temp, refineMesh->mesh);
    for (Vertex& vtx : refineMesh->mesh->Vertices)
        vtx.colour = QVector3D(0.0, 0.0, 0.0);


    setRefineMeshColours();
    refMeshes[0] = refineMesh->mesh;
    refMeshes[1] = new Mesh;
    tIndex = 1;
    for (size_t refIndex = 1; refIndex < ccSteps; ++refIndex){
        subdivideCatmullClark(refMeshes[1 - tIndex], refMeshes[tIndex]);
        tIndex = 1 - tIndex;
        refMeshes[tIndex] = new Mesh;
    }

    refineMesh->mesh = refMeshes[1 - tIndex];
    refineMesh->fillCoords();
    refineMesh->fillColours();

    assert(colourSurface->mesh->Vertices.size() == refineMesh->mesh->Vertices.size());

    for (int i = 0; i < colourSurface->mesh->Vertices.size(); ++i){
        colourSurface->mesh->Vertices[i].colour += (QVector3D(1.0, 1.0, 1.0) -  colourSurface->mesh->Vertices[i].colour) * refineMesh->mesh->Vertices[i].colour;
        if (colourSurface->mesh->Vertices[i].colour[0] > 1.0f ||
                colourSurface->mesh->Vertices[i].colour[1] > 1.0f ||
                colourSurface->mesh->Vertices[i].colour[2] > 1.0f )
            qDebug() << colourSurface->mesh->Vertices[i].colour[0];
    }


    colourSurface->fillCoords();
    colourSurface->fillColours();

    return;
}
