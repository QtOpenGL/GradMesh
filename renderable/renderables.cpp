#include "renderables.h"
#include "../subdiv/subdiv.h"
#include "../subdiv/meshtools.h"
#include "mainview.h"

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

    refiners.append({&ternaryStep, TERNARY});
    refiners.append({&subdivideCatmullClark, CATMULLCLARK});
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

    refineMesh->fillCoords();
    refineMesh->fillColours();

    *edgeRenderable->colours = QVector<QVector3D>{ {1.0,1.0,1.0}, {1.0,1.0,1.0} };
    *edgeRenderable->indices = QVector<unsigned int> {0, 1};
}

void Renderables::updateEm(){
    skeletonMesh->fillCoords();

    tMeshes[0] = skeletonMesh->mesh;
    tMeshes[1] = new Mesh;

    size_t tIndex = 1;

    for (size_t refIndex = 0; refIndex < static_cast<size_t>(refiners.size()); ++refIndex){
        refiners[refIndex].meshRefiner(tMeshes[1 - tIndex], tMeshes[tIndex]);
        tIndex = 1 - tIndex;
        tMeshes[tIndex] = new Mesh;
    }

    colourSurface->mesh = tMeshes[1 - tIndex];
    colourSurface->fillCoords();
    colourSurface->fillColours();

    refineMesh->mesh = new Mesh;
    ternaryStep(controlMesh->mesh, refineMesh->mesh);
    Mesh *temp = new Mesh;
    temp->copy(refineMesh->mesh);
    subdivideCatmullClark(temp, refineMesh->mesh);
    for (Vertex& vtx : refineMesh->mesh->Vertices)
        vtx.colour = QVector3D(0.0, 0.0, 0.0);

    refineMesh->fillCoords();
    refineMesh->fillColours();

    return;
}
