#ifndef RENDERABLES_H
#define RENDERABLES_H

#include "renderable.h"
#include "meshrenderable.h"
#include "controlrenderable.h"
#include "enums.h"

class Renderables
{
public:

    struct controlVec {
        int index;
        QVector3D colour;
        float alpha = 1.0;
        controlVec(int _index, QVector3D &&_colour, float _alpha){
            index = _index;
            colour = _colour;
            alpha = _alpha;
        }
        controlVec(controlVec &&ctr)
            :
            index(ctr.index),
            colour(ctr.colour),
            alpha(ctr.alpha)
        {}
        controlVec(controlVec const &ctr)
            :
            index(ctr.index),
            colour(ctr.colour),
            alpha(ctr.alpha)
        {}
        controlVec() = default;
        controlVec operator=(controlVec const &other){
            if (this != &other) {
                index = other.index;
                colour = other.colour;
                alpha = other.alpha;
            }
            return *this;
        }

        void print(){
            qDebug() << "index: " << index << ", colour: " << colour << ", alpha: " << alpha;
        }
    };

    Renderables();
    virtual ~Renderables();
    MeshRenderable *controlMesh;
    MeshRenderable *colourSurface;
    ControlRenderable *skeletonMesh;

    Renderable *edgeRenderable;
    Renderable *faceRenderable;

    QVector<Renderable *> *renderableList;

    QVector<MeshRenderable *> meshVector;
    QVector<QVector<unsigned int>* > ptIndices;
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
