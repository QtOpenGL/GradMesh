#ifndef RENDERABLES_H
#define RENDERABLES_H

#include "renderable.h"
#include "meshrenderable.h"
#include "controlrenderable.h"
#include "enums.h"
#include <tuple>
#include <memory>

class Renderables
{
public:
    Renderables();
    virtual ~Renderables();
    MeshRenderable *controlMesh;
    MeshRenderable *colourSurface;
    ControlRenderable *skeletonMesh;

    Renderable *gradRenderable;
    Renderable *edgeRenderable;
    Renderable *faceRenderable;

    QVector<Renderable *> *renderableList;

    QVector<MeshRenderable *> meshVector;
    QVector<QVector<unsigned int>* > ptIndices;
    typedef std::tuple<int, QVector3D, float, QVector<QVector2D>* > controlVec;
    QVector<QVector<controlVec> > *controlVectors;

    size_t ccSteps = 3;
    void updateEm();
    void init(OBJFile* loadedOBJFile);
    void init(Mesh *mesh);
    void init();
    float alpha = 1.0;
    void setRing(Vertex *vtx, controlVec &vec);

    void threeRing(Vertex *vertex, QVector<unsigned int> *pts, int counter);
};

#endif // RENDERABLES_H
