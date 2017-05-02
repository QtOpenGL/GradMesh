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

    Mesh *tMeshes[2];
    struct refiner {
        void (*meshRefiner)(Mesh *, Mesh *);
        REFINERS refinerID;
    };
    QVector <refiner> refiners;

    size_t cc_steps = 2;
    size_t quadrant = 0;
    /*
     *  split screen:
     *
     * | 1 | 2 |
     * |-------|
     * | 3 | 4 |
     *
     * 0 is whole screen
     *
     */
    void updateEm();
    void init(OBJFile* loadedOBJFile);
    void init(Mesh *mesh);
    void init();
};

#endif // RENDERABLES_H
