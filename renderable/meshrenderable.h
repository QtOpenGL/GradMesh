#ifndef MESHRENDERABLE_H
#define MESHRENDERABLE_H

#include "renderable.h"
#include "../mesh/mesh.h"

class MeshRenderable : public Renderable
{
public:
    MeshRenderable();

    Mesh *mesh = nullptr;

    bool hasMesh(){
        return (mesh != nullptr);
    }

    virtual void fillCoords();
    virtual void fillColours();
    virtual ~MeshRenderable();

};

#endif // MESHRENDERABLE_H
