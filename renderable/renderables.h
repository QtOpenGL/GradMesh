#ifndef RENDERABLES_H
#define RENDERABLES_H

#include "renderable.h"
#include "meshrenderable.h"
#include "controlrenderable.h"
#include "enums.h"

class Renderables
{
public:
    Renderables();
    virtual ~Renderables();
    MeshRenderable *controlMesh;
    MeshRenderable *colourSurface;
    ControlRenderable *skeletonMesh;

    Renderable *edgeRenderable;
    Renderable *faceRenderable;

    QVector<Renderable *> *renderableList;

    QVector<MeshRenderable *> meshVector;

    size_t ccSteps = 3;
    size_t cc_steps = 4;
    void updateEm();
    void init(OBJFile* loadedOBJFile);
    void init(Mesh *mesh);
    void init();
    float alpha = 1.0;
};

#endif // RENDERABLES_H
